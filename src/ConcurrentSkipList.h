#pragma once

#include <mutex>

#include "SequentialSkipList.h"

template <typename T, std::uint16_t MaximumHeight>
class ConcurrentSkipList final : public SkipList<T>
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

  public:
    ConcurrentSkipList()
        : m_mutex()
        , m_list()
    {
    }

    bool empty() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.empty();
    }

    size_type size() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.size();
    }

    bool insert(const_reference value) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.insert(value);
    }

    bool remove(const_reference value) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.remove(value);
    }

    bool contains(const_reference value) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_list.contains(value);
    }

    void clear() override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_list.clear();
    }

  private:
    std::mutex m_mutex;
    SequentialSkipList<T, MaximumHeight> m_list;
};
