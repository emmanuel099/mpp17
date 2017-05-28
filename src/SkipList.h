#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <random>

template <typename T, std::uint16_t MaximumHeight>
class SkipList
{
  public:
    static_assert(std::is_integral<T>::value, "T must be an integral type");
    static_assert(MaximumHeight > 0, "Maximum height must be greater than 0");

    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

  private:
    struct Node {
        Node(const_reference value)
            : value(value)
        {
#ifndef NDEBUG
            // if stack trace contains coffee, then there went something
            // somewhere terrible wrong
            next.fill(reinterpret_cast<Node*>(0xC0FFE));
#endif
        }

        value_type value;
        std::array<Node*, MaximumHeight> next;
    };

  public:
    SkipList()
        : m_head(new Node(std::numeric_limits<value_type>::min()))
        , m_sentinel(new Node(std::numeric_limits<value_type>::max()))
        , m_height(0)
        , m_size(0)
    {
        m_head->next.fill(m_sentinel); // connect head with sentinel
        m_sentinel->next.fill(nullptr);
    }

    ~SkipList()
    {
        for (auto* current = m_head; current != nullptr;) {
            auto* next = current->next[0];
            delete current;
            current = next;
        }
    }

    bool empty() const
    {
        return m_size == 0;
    }

    size_type size() const
    {
        return m_size;
    }

    bool insert(const_reference value)
    {
        std::array<Node*, MaximumHeight> predecessors;
        auto* current = searchNodeAndRememberPredecessors(value, predecessors);
        if (current->value == value) { // already in list
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
        auto* newNode = new Node(value);
        for (std::uint16_t level = 0; level <= newHeight; ++level) {
            newNode->next[level] = predecessors[level]->next[level];
            predecessors[level]->next[level] = newNode;
        }

        ++m_size;

        return true;
    }

    bool remove(const_reference value)
    {
        std::array<Node*, MaximumHeight> predecessors;
        auto* current = searchNodeAndRememberPredecessors(value, predecessors);
        if (current->value != value) { // not in list
            return false;
        }

        for (std::uint16_t level = 0; level <= m_height; ++level) {
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

        return true;
    }

    bool contains(const_reference value)
    {
        auto* current = m_head;
        for (std::int32_t level = m_height; level >= 0; --level) {
            while (current->next[level]->value < value) {
                current = current->next[level];
            }
        }
        current = current->next[0];

        return current->value == value;
    }

    void clear()
    {
        // remove all nodes between head and sentinel
        for (auto* current = m_head->next[0]; current != m_sentinel;) {
            auto* next = current->next[0];
            delete current;
            current = next;
        }
        m_size = 0;

        // set all changed next pointers of head node back to sentinel node
        for (std::uint16_t level = 0; level <= m_height; ++level) {
            m_head->next[level] = m_sentinel;
        }
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
    Node* const m_head;
    Node* const m_sentinel;
    std::uint16_t m_height;
    std::size_t m_size;
};
