#include "WorkStrategy.h"

#include <cassert>
#include <cmath>
#include <random>

#include "Benchmarking.h"
#include "Thread.h"

namespace WorkStrategy
{
static void DefaultPrepare(const BaseBenchmarkConfiguration& config,
                           SkipList<long>& list)
{
    Thread::single([&] {
        for (long i = 0; i < config.initialNumberOfItems; i++) {
            list.insert(i);
        }
    });
}

static void DefaultCleanup(const BaseBenchmarkConfiguration&, SkipList<long>&)
{
}

Workload createAscendingInsertWorkload()
{
    const auto AscendingInsert = [](const BaseBenchmarkConfiguration& config,
                                    SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const long begin =
            threadId * config.numberOfItems + config.initialNumberOfItems;
        const long end = begin + config.numberOfItems;

        for (long i = begin; i < end; i++) {
            list.insert(i);
        }
    };

    return {&DefaultPrepare, AscendingInsert, &DefaultCleanup};
}

Workload createDescendingInsertWorkload()
{
    const auto DescendingInsert = [](const BaseBenchmarkConfiguration& config,
                                     SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const long begin =
            threadId * config.numberOfItems + config.initialNumberOfItems;
        const long end = begin + config.numberOfItems;

        for (long i = end; i > begin; i--) {
            list.insert(i);
        }
    };

    return {&DefaultPrepare, DescendingInsert, &DefaultCleanup};
}

Workload createInterleavingInsertWorkload()
{
    const auto InterleavingInsert = [](const BaseBenchmarkConfiguration& config,
                                       SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();
        const auto itemsPerThread = config.numberOfItems;

        long number = config.initialNumberOfItems + threadId;
        for (long i = 0; i < itemsPerThread; i++) {
            list.insert(number);
            number += itemsPerThread;
        }
    };

    return {&DefaultPrepare, InterleavingInsert, &DefaultCleanup};
}

static void RemoveWorkloadPrepare(const BaseBenchmarkConfiguration& config,
                                  SkipList<long>& list)
{
    Thread::single([&] {
        const auto items = config.initialNumberOfItems +
                           config.numberOfItems * config.numberOfThreads;
        for (long i = 0; i < items; i++) {
            list.insert(i);
        }
    });
}

Workload createAscendingRemoveWorkload()
{
    const auto AscendingRemove = [](const BaseBenchmarkConfiguration& config,
                                    SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const long begin = threadId * config.numberOfItems;
        const long end = begin + config.numberOfItems;

        for (long i = begin; i < end; i++) {
            list.remove(i);
        }
    };

    return {&RemoveWorkloadPrepare, AscendingRemove, &DefaultCleanup};
}

Workload createDescendingRemoveWorkload()
{
    const auto DescendingRemove = [](const BaseBenchmarkConfiguration& config,
                                     SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const long begin = threadId * config.numberOfItems;
        const long end = begin + config.numberOfItems;

        for (long i = end - 1; i >= begin; i--) {
            list.remove(i);
        }
    };

    return {&RemoveWorkloadPrepare, DescendingRemove, &DefaultCleanup};
}

Workload createInterleavingRemoveWorkload()
{
    const auto InterleavingRemove = [](const BaseBenchmarkConfiguration& config,
                                       SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();
        const auto itemsPerThread = config.numberOfItems;

        long number = threadId;
        for (long i = 0; i < itemsPerThread; i++) {
            list.remove(number);
            number += itemsPerThread;
        }
    };

    return {&RemoveWorkloadPrepare, InterleavingRemove, &DefaultCleanup};
}

static thread_local std::vector<long> tl_randomNumbers;

Workload createMixedWorkload(double insertingThreads, double removingThreads)
{
    assert(insertingThreads >= 0.0);
    assert(removingThreads >= 0.0);
    assert((insertingThreads + removingThreads) <= 1.0);

    const auto Prepare = [](const BaseBenchmarkConfiguration& config,
                            SkipList<long>& list) {
        DefaultPrepare(config, list);

        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<> distribution(
            0, config.initialNumberOfItems + config.numberOfItems);

        tl_randomNumbers.reserve(config.numberOfItems);
        for (std::size_t i = 0; i < config.numberOfItems; i++) {
            tl_randomNumbers.emplace_back(distribution(generator));
        }
    };

    const auto Work = [insertingThreads, removingThreads](
        const BaseBenchmarkConfiguration& config, SkipList<long>& list) {
        //  0 insert RT removing ST searching
        //  [ ...... | .......... | ........ [
        const std::size_t RT =
            std::ceil(insertingThreads * config.numberOfThreads);
        const std::size_t ST = std::ceil((insertingThreads + removingThreads) *
                                         config.numberOfThreads);

        const auto threadId = Thread::currentThreadId();

        if (threadId >= ST) { // searching
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.contains(tl_randomNumbers[i]);
            }
        } else if (threadId >= RT) { // removing
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.remove(tl_randomNumbers[i]);
            }
        } else { // inserting
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.insert(tl_randomNumbers[i]);
            }
        }
    };

    const auto Cleanup = [](const BaseBenchmarkConfiguration& config,
                            SkipList<long>& list) {
        DefaultCleanup(config, list);

        tl_randomNumbers.clear();
    };

    return {Prepare, Work, Cleanup};
}
}
