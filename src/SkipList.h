#pragma once

#include <cstdint>
#include <type_traits>

template <typename T>
class SkipList
{
  public:
    static_assert(std::is_integral<T>::value, "T must be an integral type");

    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

  public:
    virtual ~SkipList() = default;

    virtual bool empty() = 0;

    virtual size_type size() = 0;

    virtual bool insert(const_reference value) = 0;

    virtual bool remove(const_reference value) = 0;

    virtual bool contains(const_reference value) = 0;

    virtual void clear() = 0;
};
