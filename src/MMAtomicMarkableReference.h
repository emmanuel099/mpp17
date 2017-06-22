#pragma once

#include <atomic>

template <typename T>
class MMAtomicMarkableReference
{
  public:
    MMAtomicMarkableReference(T* ref = nullptr, bool marked = false)
    {
        set(ref, marked);
    }

    T* getReference()
    {
        return (T*)(value & ~mask);
    }

    T* get(bool& marked)
    {
        uintptr_t tmp = value;
        marked = (bool)(tmp & mask);
        return (T*)(tmp & ~mask);
    }

    bool marked()
    {
        return (value & mask);
    }

    void set(T* ref, bool marked)
    {
        if (ref != nullptr) {
            // store shared pointer for memory management
            std::atomic_store(&this->ref, ref->getptr());
        }
        
        this->value = ((uintptr_t)ref & ~mask) | (marked ? 1 : 0);
    }

    bool compareAndSet(T* oldRef, T* newRef, bool oldMarked, bool newMarked)
    {
        uintptr_t oldValue = ((uintptr_t)oldRef & ~mask) | (oldMarked ? 1 : 0);
        uintptr_t newValue = ((uintptr_t)newRef & ~mask) | (newMarked ? 1 : 0);

        if (value.compare_exchange_strong(oldValue, newValue) && newRef != nullptr) {
            // update shared pointer for memory management
            std::atomic_store(&this->ref, newRef->getptr());
            return true;
        }
        return false;
    }

  private:
    std::atomic_uintptr_t value;
    std::shared_ptr<T> ref;
    static const uintptr_t mask = 1;
};
