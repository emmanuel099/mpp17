#pragma once

#include <mutex>

#include "SkipList.h"

template <typename T, std::uint16_t MaximumHeight>
class ConcurrentSkipList
{
    using SequentialSkipList = SkipList<T, MaximumHeight>;

  public:
    using value_type = typename SequentialSkipList::value_type;
    using reference = typename SequentialSkipList::reference;
    using const_reference = typename SequentialSkipList::const_reference;
    using pointer = typename SequentialSkipList::pointer;
    using const_pointer = typename SequentialSkipList::const_pointer;
    using difference_type = typename SequentialSkipList::difference_type;
    using size_type = typename SequentialSkipList::size_type;

  public:
    ConcurrentSkipList()
        : m_mutex()
        , m_list()
    {
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.empty();
    }

    size_type size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.size();
    }

    bool insert(const_reference value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.insert(value);
    }

    bool remove(const_reference value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.remove(value);
    }

    bool contains(const_reference value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.contains(value);
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_list.clear();
    }

  private:
    std::mutex m_mutex;
    SequentialSkipList m_list;
};
