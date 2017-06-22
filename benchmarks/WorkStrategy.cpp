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

static long itemsPerThread(const BaseBenchmarkConfiguration& config)
{
    switch (config.scalingMode) {
    case Scaling::Weak:
        return config.numberOfItems;
    case Scaling::Strong:
        return config.numberOfItems / config.numberOfThreads;
    }
}

Workload createAscendingInsertWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);
        const long begin = threadId * items + config.initialNumberOfItems;
        const long end = begin + items;

        for (long i = begin; i < end; i++) {
            list.insert(i);
        }
    };

    return {&DefaultPrepare, Work, &DefaultCleanup};
}

Workload createDescendingInsertWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);
        const long begin = threadId * items + config.initialNumberOfItems;
        const long end = begin + items;

        for (long i = end; i > begin; i--) {
            list.insert(i);
        }
    };

    return {&DefaultPrepare, Work, &DefaultCleanup};
}

Workload createInterleavingInsertWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);

        long number = config.initialNumberOfItems + threadId;
        for (long i = 0; i < items; i++) {
            list.insert(number);
            number += items;
        }
    };

    return {&DefaultPrepare, Work, &DefaultCleanup};
}

static void RemoveWorkloadPrepare(const BaseBenchmarkConfiguration& config,
                                  SkipList<long>& list)
{
    Thread::single([&] {
        const auto items = config.initialNumberOfItems +
                           itemsPerThread(config) * config.numberOfThreads;
        for (long i = 0; i < items; i++) {
            list.insert(i);
        }
    });
}

Workload createAscendingRemoveWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);
        const long begin = threadId * items;
        const long end = begin + items;

        for (long i = begin; i < end; i++) {
            list.remove(i);
        }
    };

    return {&RemoveWorkloadPrepare, Work, &DefaultCleanup};
}

Workload createDescendingRemoveWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);
        const long begin = threadId * items;
        const long end = begin + items;

        for (long i = end - 1; i >= begin; i--) {
            list.remove(i);
        }
    };

    return {&RemoveWorkloadPrepare, Work, &DefaultCleanup};
}

Workload createInterleavingRemoveWorkload()
{
    const auto Work = [](const BaseBenchmarkConfiguration& config,
                         SkipList<long>& list) {
        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);

        long number = threadId;
        for (long i = 0; i < items; i++) {
            list.remove(number);
            number += items;
        }
    };

    return {&RemoveWorkloadPrepare, Work, &DefaultCleanup};
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

        const auto items = itemsPerThread(config);

        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<> distribution(
            0, config.initialNumberOfItems + items);

        tl_randomNumbers.reserve(items);
        for (std::size_t i = 0; i < items; i++) {
            tl_randomNumbers.emplace_back(distribution(generator));
        }
    };

    const auto Work = [=](const BaseBenchmarkConfiguration& config,
                          SkipList<long>& list) {
        //  0 insert RT removing ST searching
        //  [ ...... | .......... | ........ [
        const std::size_t RT =
            std::ceil(insertingThreads * config.numberOfThreads);
        const std::size_t ST = std::ceil((insertingThreads + removingThreads) *
                                         config.numberOfThreads);

        const auto threadId = Thread::currentThreadId();

        const auto items = itemsPerThread(config);

        if (threadId >= ST) { // searching
            for (std::size_t i = 0; i < items; i++) {
                list.contains(tl_randomNumbers[i]);
            }
        } else if (threadId >= RT) { // removing
            for (std::size_t i = 0; i < items; i++) {
                list.remove(tl_randomNumbers[i]);
            }
        } else { // inserting
            for (std::size_t i = 0; i < items; i++) {
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
