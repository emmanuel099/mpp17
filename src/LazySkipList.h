#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <limits>
#include <mutex>
#include <random>

#include "SkipList.h"

template <typename T, std::uint16_t MaximumHeight>
class LazySkipList final : public SkipList<T>
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

        const value_type value;
        std::array<std::shared_ptr<Node>, MaximumHeight> next;
        const std::uint16_t height;
        std::recursive_mutex mutex;
        bool marked = false;
        bool fullyLinked = false;
    };

  public:
    LazySkipList()
        : m_head(std::make_shared<Node>(std::numeric_limits<value_type>::min(),
                                        MaximumHeight))
        , m_sentinel(std::make_shared<Node>(
              std::numeric_limits<value_type>::max(), MaximumHeight))
        , m_height(MaximumHeight - 1)
        , m_size(0)
    {
        m_head->next.fill(m_sentinel); // connect head with sentinel
        m_sentinel->next.fill(nullptr);
    }

    ~LazySkipList() override
    {
        for (auto current = m_head; current != nullptr;) {
            auto next = current->next[0];
            current.reset();
            current = next;
        }
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
        const auto newHeight = randomHeight();
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;

        while (true) {
            auto foundLevel = find(value, predecessors, successors);
            if (foundLevel != -1) { // already in list
                auto foundNode = successors[foundLevel];
                if (!foundNode->marked) {
                    while (!foundNode->fullyLinked) {
                    } // wait until found node is completely inserted
                    return false;
                }
                continue; // retry until found node is removed
            }

            // insert node
            bool valid = true;
            std::uint16_t maxLockedLevel = 0;
            for (std::uint16_t level = 0; valid && (level <= newHeight);
                 ++level) {
                std::shared_ptr<Node> pred = predecessors[level];
                std::shared_ptr<Node> succ = successors[level];
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
                continue;
            }

            // update successors and predecessors
            auto newNode = std::make_shared<Node>(value, newHeight);
            for (std::uint16_t level = 0; level <= newHeight; ++level) {
                newNode->next[level] = successors[level];
            }
            for (std::uint16_t level = 0; level <= newHeight; ++level) {
                predecessors[level]->next[level] = newNode;
            }
            newNode->fullyLinked = true; // insert linearization point
            ++m_size;

            // unlock predecessors
            for (std::uint16_t level = 0; level <= maxLockedLevel; ++level) {
                predecessors[level]->mutex.unlock();
            }
            return true;
        }
    }

    bool remove(const_reference value) override
    {
        bool retryInProgress = false;
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;

        while (true) {
            auto foundLevel = find(value, predecessors, successors);
            if (foundLevel == -1) { // node not found
                return false;
            }

            auto node = successors[foundLevel];
            if (retryInProgress || (node->fullyLinked && !node->marked &&
                                    node->height == foundLevel)) {
                if (!retryInProgress) { // executed only on first try
                    node->mutex.lock();
                    if (node->marked) {
                        node->mutex.unlock();
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
                    std::shared_ptr<Node> pred = predecessors[level];
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
                    continue;
                }

                // remove node
                for (std::int32_t level = node->height; level >= 0; --level) {
                    predecessors[level]->next[level] = node->next[level];
                }
                node->mutex.unlock();

                // unlock predecessors
                for (std::uint16_t level = 0; level <= maxLockedLevel;
                     ++level) {
                    predecessors[level]->mutex.unlock();
                }
                return true;
            } else { // node virtually not in list
                return false;
            }
        }
    }

    bool contains(const_reference value) override
    {
        std::array<std::shared_ptr<Node>, MaximumHeight> predecessors;
        std::array<std::shared_ptr<Node>, MaximumHeight> successors;
        auto onLevel = find(value, predecessors, successors);

        return onLevel != -1 && successors[onLevel]->fullyLinked &&
               !successors[onLevel]->marked;
    }

    void clear() override
    {
        // lock all nodes
        for (auto current = m_head; current != m_sentinel;) {
            current->mutex.lock();
            current = current->next[0];
        }

        // delete all nodes
        for (auto current = m_head; current != nullptr;) {
            auto next = current->next[0];
            current.reset();
            current = next;
        }

        m_head->next.fill(m_sentinel);
        m_size = 0;
        m_head->mutex.unlock();
    }

  private:
    std::int32_t
    find(const_reference value,
         std::array<std::shared_ptr<Node>, MaximumHeight>& predecessors,
         std::array<std::shared_ptr<Node>, MaximumHeight>& successors) const
    {
        predecessors.fill(m_head);
        successors.fill(m_sentinel);

        std::int32_t foundLevel = -1;
        auto current = m_head;
        for (std::int32_t level = m_height; level >= 0; --level) {
            while (current->next[level]->value < value) {
                current = current->next[level];
            }

            if (foundLevel == -1 && current->next[level]->value == value) {
                foundLevel = level;
            }
            predecessors[level] = current;
            successors[level] = current->next[level];
        }
        return foundLevel;
    }

    /**
     * @return Random height in range [0..MaximumHeight[
     */
    static std::uint16_t randomHeight()
    {
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
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
    std::shared_ptr<Node> m_head;
    std::shared_ptr<Node> m_sentinel;
    std::uint16_t m_height;
    std::atomic_size_t m_size;
};
