#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <limits>
#include <mutex>
#include <random>

#include "SkipList.h"
#include "SkipListStatistics.h"

template <typename T, std::uint16_t MaximumHeight>
class MemoryManagedLazySkipList final : public SkipList<T>
{
  public:
    static_assert(MaximumHeight > 0, "Maximum height must be greater than 0");

    using value_type = typename SkipList<T>::value_type;
    using reference = typename SkipList<T>::reference;
    using const_reference = typename SkipList<T>::const_reference;
    using pointer = typename SkipList<T>::pointer;
    using const_pointer = typename SkipList<T>::const_pointer;
    using difference_type = typename SkipList<T>::difference_type;
    using size_type = typename SkipList<T>::size_type;

  private:
    struct Node {
        Node(const_reference value, std::uint16_t height)
            : value(value)
            , height(height)
        {
        }

        ~Node()
        {
            for (std::uint16_t i = 0; i < MaximumHeight; ++i) {
                next[i].reset();
            }
        }

        const value_type value;
        std::array<std::shared_ptr<Node>, MaximumHeight> next;
        const std::uint16_t height;
        std::recursive_mutex mutex;
        volatile bool marked = false;
        volatile bool fullyLinked = false;
    };

  public:
    MemoryManagedLazySkipList()
        : m_head(std::make_shared<Node>(std::numeric_limits<value_type>::min(),
                                        MaximumHeight))
        , m_sentinel(std::make_shared<Node>(
              std::numeric_limits<value_type>::max(), MaximumHeight))
        , m_size(0)
    {
        m_head->next.fill(m_sentinel); // connect head with sentinel
    }

    bool empty() override
    {
        return m_size == 0;
    }

    size_type size() override
    {
        return m_size;
    }

    bool insert(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().insertionStart();
#endif

        const auto newHeight = randomHeight();
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;

        while (true) {
            const auto foundLevel = find(value, predecessors, successors);
            if (foundLevel != -1) { // already in list
                const auto& foundNode = successors[foundLevel];
                if (!foundNode->marked) {
                    while (!foundNode->fullyLinked) {
                    } // wait until found node is completely inserted
#ifdef COLLECT_STATISTICS
                    SkipListStatistics::threadLocalInstance()
                        .insertionFailure();
#endif
                    return false;
                }
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().insertionRetry();
#endif
                continue; // retry until found node is removed
            }

            // insert node
            bool valid = true;
            std::uint16_t maxLockedLevel = 0;
            for (std::uint16_t level = 0; valid && (level <= newHeight);
                 ++level) {
                const auto& pred = predecessors[level];
                const auto& succ = successors[level];
                pred->mutex.lock();
                maxLockedLevel = level;
                valid =
                    !pred->marked && !succ->marked && pred->next[level] == succ;
            }

            if (!valid) { // invalid state -> retry
                // unlock predecessors
                for (std::uint16_t level = 0; level <= maxLockedLevel;
                     ++level) {
                    predecessors[level]->mutex.unlock();
                }
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().insertionRetry();
#endif
                continue;
            }

            // update successors and predecessors
            const auto newNode = std::make_shared<Node>(value, newHeight);
            newNode->next = std::move(successors);
            for (std::uint16_t level = 0; level <= newHeight; ++level) {
                std::atomic_store(&predecessors[level]->next[level], newNode);
            }
            newNode->fullyLinked = true; // insert linearization point
            ++m_size;

            // unlock predecessors
            for (std::uint16_t level = 0; level <= maxLockedLevel; ++level) {
                predecessors[level]->mutex.unlock();
            }

#ifdef COLLECT_STATISTICS
            SkipListStatistics::threadLocalInstance().insertionSuccess();
#endif
            return true;
        }
    }

    bool remove(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().deletionStart();
#endif
        bool retryInProgress = false;
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;

        while (true) {
            const auto foundLevel = find(value, predecessors, successors);
            if (foundLevel == -1) { // node not found
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().deletionFailure();
#endif
                return false;
            }

            const auto& node = successors[foundLevel];
            if (retryInProgress || (node->fullyLinked && !node->marked &&
                                    node->height == foundLevel)) {
                if (!retryInProgress) { // executed only on first try
                    node->mutex.lock();
                    if (node->marked) {
                        node->mutex.unlock();
#ifdef COLLECT_STATISTICS
                        SkipListStatistics::threadLocalInstance()
                            .deletionFailure();
#endif
                        return false;
                    }
                    node->marked = true; // remove linearization point
                    m_size--;
                    retryInProgress = true;
                }

                // lock predecessors
                bool valid = true;
                std::uint16_t maxLockedLevel = 0;
                for (std::uint16_t level = 0; valid && (level <= node->height);
                     ++level) {
                    const auto& pred = predecessors[level];
                    pred->mutex.lock();
                    maxLockedLevel = level;
                    valid = !pred->marked && pred->next[level] == node;
                }

                if (!valid) { // invalid state -> retry
                    // unlock predecessors
                    for (std::uint16_t level = 0; level <= maxLockedLevel;
                         ++level) {
                        predecessors[level]->mutex.unlock();
                    }
#ifdef COLLECT_STATISTICS
                    SkipListStatistics::threadLocalInstance().deletionRetry();
#endif
                    continue;
                }

                // remove node
                for (std::int32_t level = node->height; level >= 0; --level) {
                    std::atomic_store(&predecessors[level]->next[level],
                                      node->next[level]);
                }
                node->mutex.unlock();

                // unlock predecessors
                for (std::uint16_t level = 0; level <= maxLockedLevel;
                     ++level) {
                    predecessors[level]->mutex.unlock();
                }
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().deletionSuccess();
#endif
                return true;
            } else { // node virtually not in list
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().deletionFailure();
#endif
                return false;
            }
        }
    }

    bool contains(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupStart();
#endif
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;
        const auto onLevel = find(value, predecessors, successors);

#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupDone();
#endif
        return onLevel != -1 && successors[onLevel]->fullyLinked &&
               !successors[onLevel]->marked;
    }

    void clear() override
    {
        std::lock_guard<std::recursive_mutex> lock(m_head->mutex);

        // mark all nodes (expect of head and sentinel)
        for (auto current = std::atomic_load(&m_head->next[0]);
             current != m_sentinel;
             current = std::atomic_load(&current->next[0])) {
            while (not current->fullyLinked or current->marked) {
            }
            std::lock_guard<std::recursive_mutex> currentLock(current->mutex);
            current->marked = true; // ignore if it was marked in the meantime
        }

        // fully re-connect head with sentinel
        for (std::uint16_t level = 0; level < MaximumHeight; ++level) {
            std::atomic_store(&m_head->next[level], m_sentinel);
        }

        m_size = 0;
    }

  private:
    std::int32_t
    find(const_reference value,
         std::array<std::shared_ptr<Node>, MaximumHeight>& predecessors,
         std::array<std::shared_ptr<Node>, MaximumHeight>& successors) const
    {
        std::int32_t foundLevel = -1;
        auto pred = m_head;
        for (std::int32_t level = (MaximumHeight - 1); level >= 0; --level) {
            auto curr = std::atomic_load(&pred->next[level]);
            while (curr->value < value) {
                pred = std::move(curr);
                curr = std::atomic_load(&pred->next[level]);
            }

            if (foundLevel == -1 && curr->value == value) {
                foundLevel = level;
            }
            predecessors[level] = pred;
            successors[level] = curr;
        }
        return foundLevel;
    }

    /**
     * @return Random height in range [0..MaximumHeight[
     */
    static std::uint16_t randomHeight()
    {
        std::random_device randomDevice;
        static thread_local std::mt19937 generator(randomDevice());
        std::bernoulli_distribution distribution(0.5);
        const auto flipCoinAndCheckIfHead = [&] {
            return distribution(generator) == true;
        };

        std::uint16_t height = 0;
        while (not flipCoinAndCheckIfHead() and
               (height < (MaximumHeight - 1))) {
            ++height;
        }

        assert(height < MaximumHeight);
        return height;
    }

  private:
    const std::shared_ptr<Node> m_head;
    const std::shared_ptr<Node> m_sentinel;
    std::atomic_size_t m_size;
};
