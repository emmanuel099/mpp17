#pragma once

#include "Benchmarking.h"
#include "SkipList.h"
#include "Thread.h"

namespace WorkStrategy
{
const auto AscendingInsert = [](const BenchmarkConfiguration& config,
                                SkipList<int>& list) {
    const auto threadId = Thread::currentThreadId();

    const int begin =
        threadId * config.numberOfItems + config.initialNumberOfItems;
    const int end = begin + config.numberOfItems;

    for (int i = begin; i < end; i++) {
        list.insert(i);
    }
};

const auto DescendingInsert = [](const BenchmarkConfiguration& config,
                                 SkipList<int>& list) {
    const auto threadId = Thread::currentThreadId();

    const int begin =
        threadId * config.numberOfItems + config.initialNumberOfItems;
    const int end = begin + config.numberOfItems;

    for (int i = end; i > begin; i--) {
        list.insert(i);
    }
};

const auto InterleavingInsert = [](const BenchmarkConfiguration& config,
                                   SkipList<int>& list) {
    const auto threadId = Thread::currentThreadId();
    const auto itemsPerThread = config.numberOfItems;

    int number = config.initialNumberOfItems + threadId;
    for (int i = 0; i < itemsPerThread; i++) {
        list.insert(number);
        number += itemsPerThread;
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto AscendingRemove = [](const BenchmarkConfiguration& config,
                                SkipList<int>& list) {
    assert(config.initialNumberOfItems >=
           (config.numberOfThreads * config.numberOfItems));

    const auto threadId = Thread::currentThreadId();

    const int begin = threadId * config.numberOfItems;
    const int end = begin + config.numberOfItems;

    for (int i = begin; i < end; i++) {
        list.remove(i);
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto DescendingRemove = [](const BenchmarkConfiguration& config,
                                 SkipList<int>& list) {
    assert(config.initialNumberOfItems >=
           (config.numberOfThreads * config.numberOfItems));

    const auto threadId = Thread::currentThreadId();

    const int begin = threadId * config.numberOfItems;
    const int end = begin + config.numberOfItems;

    for (int i = end - 1; i >= begin; i--) {
        list.remove(i);
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto InterleavingRemove = [](const BenchmarkConfiguration& config,
                                   SkipList<int>& list) {
    assert(config.initialNumberOfItems >=
           (config.numberOfThreads * config.numberOfItems));

    const auto threadId = Thread::currentThreadId();
    const auto itemsPerThread = config.numberOfItems;

    int number = threadId;
    for (int i = 0; i < itemsPerThread; i++) {
        list.remove(number);
        number += itemsPerThread;
    }
};

std::function<void(const BenchmarkConfiguration&, SkipList<int>&)>
createMixedWorkload(double insertingThreads, double removingThreads)
{
    assert(insertingThreads >= 0.0);
    assert(removingThreads >= 0.0);
    assert((insertingThreads + removingThreads) <= 1.0);

    return [insertingThreads, removingThreads](
        const BenchmarkConfiguration& config, SkipList<int>& list) {
        //  0 insert RT removing ST searching
        //  [ ...... | .......... | ........ [
        const std::size_t RT =
            std::ceil(insertingThreads * config.numberOfThreads);
        const std::size_t ST = std::ceil((insertingThreads + removingThreads) *
                                         config.numberOfThreads);

        const auto threadId = Thread::currentThreadId();

        if (threadId >= ST) { // searching

        } else if (threadId >= RT) { // removing

        } else { // inserting
        }
    };
}
}
