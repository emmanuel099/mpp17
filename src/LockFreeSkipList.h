#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <limits>
#include <mutex>
#include <random>

#include "SkipList.h"
#include "AtomicMarkableReference.h"
#include "SkipListStatistics.h"

template <typename T, std::uint16_t MaximumHeight>
class LockFreeSkipList final : public SkipList<T>
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
            next.fill(nullptr);
        }
        
        ~Node()
        {
            for (std::uint16_t level = 0; level <= MaximumHeight -1; ++level) {
                delete next[level];
            }
        }

        const value_type value;
        const std::uint16_t height;
        std::array<AtomicMarkableReference<Node>*, MaximumHeight> next;
    };

  public:
    LockFreeSkipList()
        : m_head(new Node(std::numeric_limits<value_type>::min(),
                                        MaximumHeight))
        , m_sentinel(new Node(
              std::numeric_limits<value_type>::max(), MaximumHeight))
        , m_size(0)
    {
        for (std::uint16_t level = 0; level <= MaximumHeight -1; ++level) {
            m_head->next[level] = new AtomicMarkableReference<Node>(m_sentinel, false);
            m_sentinel->next[level] = new AtomicMarkableReference<Node>(nullptr, false);
        }
    }
    
    ~LockFreeSkipList() override
    {
        for (auto* current = m_head; current != nullptr;) {
            auto* next = current->next[0]->getReference();
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
        std::uint16_t topLevel = randomHeight();
        std::array<Node*, MaximumHeight> predecessors;
        std::array<Node*, MaximumHeight> successors;
        
        while (true) {
            // check if value already in list
            if (find(value, predecessors, successors)) {
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().insertionFailure();
#endif
                return false;
            }
            
            // prepare new node
            Node* newNode = new Node(value, topLevel);
            for (std::uint16_t level = 0; level <= topLevel; ++level) {
                Node* succ = successors[level];
                newNode->next[level] = new AtomicMarkableReference<Node>(succ, false);
            }
            
            // set bottom predecessor
            Node* pred = predecessors[0];
            Node* succ = successors[0];
            if (!pred->next[0]->compareAndSet(succ, newNode, false, false)) {
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().insertionRetry();
#endif
                delete newNode;
                continue;
            }
            m_size++;
            
            // set remaining predecessors
            for (std::uint16_t level = 1; level <= topLevel; ++level) {
                while (true) {
                    pred = predecessors[level];
                    succ = successors[level];
                    if (pred->next[level]->compareAndSet(succ, newNode, false, false)) {
                        break;
                    }
                    find(value, predecessors, successors);                    
                }
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
        std::array<Node*, MaximumHeight> predecessors;
        std::array<Node*, MaximumHeight> successors;
        bool marked = false;
        Node* succ;
        
        while(true) {
            // check if value in list
            if (!find(value, predecessors, successors)) {
#ifdef COLLECT_STATISTICS
                SkipListStatistics::threadLocalInstance().deletionFailure();
#endif
                return false;
            }
            
            // mark all links of nodeToRemove from toplevel to 1
            Node* nodeToRemove = successors[0];
            for (std::uint16_t level = nodeToRemove->height; level >= 1; --level) {
                succ = nodeToRemove->next[level]->get(marked);
                while (!marked) {
                    nodeToRemove->next[level]->compareAndSet(succ, succ, false, true);
                    succ = nodeToRemove->next[level]->get(marked);
                }
            }
            
            // mark bottom level links
            succ = nodeToRemove->next[0]->get(marked);
            while (true) {
                bool done = nodeToRemove->next[0]->compareAndSet(succ, succ, false, true);
                succ = successors[0]->next[0]->get(marked);
                if (done) {
#ifdef COLLECT_STATISTICS
                    SkipListStatistics::threadLocalInstance().deletionSuccess();
#endif
                    m_size--;
                    find(value, predecessors, successors); // clean up, optimization
                    return true;
                } else if (marked) {
#ifdef COLLECT_STATISTICS
                    SkipListStatistics::threadLocalInstance().deletionFailure();
#endif
                    return false;
                }
            }
        }
    }

    bool contains(const_reference value) override
    {
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupStart();
#endif
        Node* pred = m_head;
        Node* curr = nullptr;
        Node* succ = nullptr;
        bool marked = false;
        
        for (std::int32_t level = MaximumHeight - 1; level >= 0; --level) {
            curr = pred->next[level]->get(marked);
            while (true) {
                succ = curr->next[level]->get(marked);
                // ignore marked nodes
                while (marked) {
                    curr = curr->next[level]->getReference();
                    succ = curr->next[level]->get(marked);                
                }
                
                if (curr->value < value) {
                    pred = curr;
                    curr = succ;
                } else {
                    break;
                }
            }
        }
        
#ifdef COLLECT_STATISTICS
        SkipListStatistics::threadLocalInstance().lookupDone();
#endif
        
        return (curr->value == value);
    }

    void clear() override
    {
        // TODO not linearizable -> use locks?
        bool marked = false;
        
        // mark all nodes (expect of head and sentinel)
        for (auto* current = m_head->next[0]->getReference(); current != m_sentinel; current = current->next[0]->getReference()) {
            for (std::int32_t level = current->height; level >= 0; --level) {
                Node* succ = current->next[level]->get(marked);
                while (!marked) {
                    current->next[level]->compareAndSet(succ, succ, false, true);
                    succ = current->next[level]->get(marked);
                }
            }
        }
        
        // fully re-connect head with sentinel
        for (std::uint16_t level = 0; level < MaximumHeight; ++level) {
           m_head->next[level]->set(m_sentinel, false);
        }

        m_size = 0;        
    }
    
  private:
      
    bool
    find(const_reference value,
         std::array<Node*, MaximumHeight>& predecessors,
         std::array<Node*, MaximumHeight>& successors) const
    {
        bool marked = false;
        Node* pred = nullptr;
        Node* curr = nullptr;
        Node* succ = nullptr;
        
        retry:
        while (true) {
            pred = m_head;
            for (std::int32_t level = MaximumHeight -1;  level >= 0; --level) {
                curr = pred->next[level]->get(marked);
                while (true) {
                    succ = curr->next[level]->get(marked);
                    // link out marked nodes
                    while(marked) {
                        if (!pred->next[level]->compareAndSet(curr, succ, false, false)) {
                            goto retry; //?
                        }
                        curr = pred->next[level]->getReference();
                        succ = curr->next[level]->get(marked);
                    }
                
                    if (curr->value < value) {
                        pred = curr;
                        curr = succ;
                    } else {
                        break;
                    }
                }
                predecessors[level] = pred;
                successors[level] = curr;
            }
            return (curr->value == value);  
        }
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
    Node* m_head;
    Node* m_sentinel;
    std::atomic_size_t m_size;
};
