#pragma once

#include <cassert>

#include "Benchmarking.h"
#include "SkipList.h"
#include "Thread.h"

namespace WorkStrategy
{
const auto AscendingInsert = [](const BenchmarkConfiguration& config,
                                SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();

    const long begin =
        threadId * config.numberOfItems + config.initialNumberOfItems;
    const long end = begin + config.numberOfItems;

    for (long i = begin; i < end; i++) {
        list.insert(i);
    }
};

const auto DescendingInsert = [](const BenchmarkConfiguration& config,
                                 SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();

    const long begin =
        threadId * config.numberOfItems + config.initialNumberOfItems;
    const long end = begin + config.numberOfItems;

    for (long i = end; i > begin; i--) {
        list.insert(i);
    }
};

const auto InterleavingInsert = [](const BenchmarkConfiguration& config,
                                   SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();
    const auto itemsPerThread = config.numberOfItems;

    long number = config.initialNumberOfItems + threadId;
    for (long i = 0; i < itemsPerThread; i++) {
        list.insert(number);
        number += itemsPerThread;
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto AscendingRemove = [](const BenchmarkConfiguration& config,
                                SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();

    const long begin = threadId * config.numberOfItems;
    const long end = begin + config.numberOfItems;

    for (long i = begin; i < end; i++) {
        list.remove(i);
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto DescendingRemove = [](const BenchmarkConfiguration& config,
                                 SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();

    const long begin = threadId * config.numberOfItems;
    const long end = begin + config.numberOfItems;

    for (long i = end - 1; i >= begin; i--) {
        list.remove(i);
    }
};

// Requires at least config.numberOfThreads * config.numberOfItems items
const auto InterleavingRemove = [](const BenchmarkConfiguration& config,
                                   SkipList<long>& list) {
    const auto threadId = Thread::currentThreadId();
    const auto itemsPerThread = config.numberOfItems;

    long number = threadId;
    for (long i = 0; i < itemsPerThread; i++) {
        list.remove(number);
        number += itemsPerThread;
    }
};

std::function<void(const BenchmarkConfiguration&, SkipList<long>&)>
createMixedWorkload(double insertingThreads, double removingThreads)
{
    assert(insertingThreads >= 0.0);
    assert(removingThreads >= 0.0);
    assert((insertingThreads + removingThreads) <= 1.0);

    return [insertingThreads, removingThreads](
        const BenchmarkConfiguration& config, SkipList<long>& list) {
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
