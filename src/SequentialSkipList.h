#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <random>

#include "SkipList.h"
#include "SkipListStatistics.h"

template <typename T, std::uint16_t MaximumHeight>
class SequentialSkipList final : public SkipList<T>
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
#ifndef NDEBUG
            // if stack trace contains coffee, then there went something
            // somewhere terrible wrong
            next.fill(reinterpret_cast<Node*>(0xC0FFEE));
#endif
        }

        const value_type value;
        std::array<Node*, MaximumHeight> next;
        const std::uint16_t height;
    };

  public:
    SequentialSkipList()
        : m_head(
              new Node(std::numeric_limits<value_type>::min(), MaximumHeight))
        , m_sentinel(
              new Node(std::numeric_limits<value_type>::max(), MaximumHeight))
        , m_height(0)
        , m_size(0)
    {
        m_head->next.fill(m_sentinel); // connect head with sentinel
        m_sentinel->next.fill(nullptr);

        checkConsistency();
    }

    ~SequentialSkipList() override
    {
        for (auto* current = m_head; current != nullptr;) {
            auto* next = current->next[0];
            delete current;
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
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().insertionStart();
#endif

        std::array<Node*, MaximumHeight> predecessors;
        auto* current = searchNodeAndRememberPredecessors(value, predecessors);
        if (current->value == value) { // already in list
#ifdef COLLECT_STATISTICS
            SkipListStatistics::threadLocalInstance().insertionFailure();
#endif
            return false;
        }

        const auto newHeight = randomHeight();
        if (newHeight > m_height) {
            // new node is higher than all other inserted nodes, connect slots
            // above current max. height with head node
            for (std::uint16_t level = m_height + 1; level <= newHeight;
                 ++level) {
                predecessors[level] = m_head;
            }
            m_height = newHeight;
        }

        // add a new node between predecessors and the predecessors's
        // postdecessors
        auto* newNode = new Node(value, newHeight);
        for (std::uint16_t level = 0; level <= newHeight; ++level) {
            newNode->next[level] = predecessors[level]->next[level];
            predecessors[level]->next[level] = newNode;
        }

        ++m_size;

        checkConsistency();

#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().insertionSuccess();
#endif

        return true;
    }

    bool remove(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().deletionStart();
#endif

        std::array<Node*, MaximumHeight> predecessors;
        auto* current = searchNodeAndRememberPredecessors(value, predecessors);
        if (current->value != value) { // not in list
#ifdef COLLECT_STATISTICS
            SkipListStatistics::threadLocalInstance().deletionFailure();
#endif
            return false;
        }

        const auto nodeHeight = current->height;
        for (std::uint16_t level = 0; level <= nodeHeight; ++level) {
            predecessors[level]->next[level] = current->next[level];
        }
        delete current;

        // minimize the height (max. height of all nodes between head and
        // sentinel)
        for (std::uint16_t level = m_height; level >= 1; --level) {
            if (m_head->next[level] != m_sentinel) {
                // no direct connection between head and sentinel -> there is a
                // node with height = level between
                m_height = level;
                break;
            }
        }

        --m_size;

        checkConsistency();

#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().deletionSuccess();
#endif

        return true;
    }

    bool contains(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupStart();
#endif

        auto* current = m_head;
        for (std::int32_t level = m_height; level >= 0; --level) {
            while (current->next[level]->value < value) {
                current = current->next[level];
            }
        }
        current = current->next[0];

#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupDone();
#endif

        return current->value == value;
    }

    void clear() override
    {
        // remove all nodes between head and sentinel
        for (auto* current = m_head->next[0]; current != m_sentinel;) {
            auto* next = current->next[0];
            delete current;
            current = next;
        }

        // set all changed next pointers of head node back to sentinel node
        for (std::uint16_t level = 0; level <= m_height; ++level) {
            m_head->next[level] = m_sentinel;
        }

        m_size = 0;
        m_height = 0;

        checkConsistency();
    }

  private:
    Node* searchNodeAndRememberPredecessors(
        const_reference value,
        std::array<Node*, MaximumHeight>& predecessors) const
    {
        auto* current = m_head;
        for (std::int32_t level = m_height; level >= 0; --level) {
            while (current->next[level]->value < value) {
                current = current->next[level];
            }
            predecessors[level] = current;
        }
        return current->next[0];
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

    void checkConsistency() const
    {
#ifndef NDEBUG
        // the next pointers are filled with coffee (see Node ctor)
        // a correct implementation should never perform any operations on a
        // next pointer which is equal to coffee
        const auto* coffee = reinterpret_cast<Node*>(0xC0FFEE);

        // check all non-sentinel nodes
        for (auto* current = m_head; current != m_sentinel;
             current = current->next[0]) {
            const auto nodeHeight = current->height;

            // non-coffee pointers up to node.height
            for (std::int16_t level = 1; level <= nodeHeight; level++) {
                assert(current->next[level] != nullptr);
                assert(current->next[level] != coffee);
            }

            // only coffee pointers above node.height
            for (std::int16_t level = nodeHeight + 1; level < MaximumHeight;
                 level++) {
                assert(current->next[level] == coffee);
            }
        }

        // sentinel node should only contain nullptrs
        for (std::int16_t level = 0; level < MaximumHeight; level++) {
            assert(m_sentinel->next[level] == nullptr);
        }
#endif
    }

  private:
    Node* const m_head;
    Node* const m_sentinel;
    std::uint16_t m_height;
    std::size_t m_size;
};
