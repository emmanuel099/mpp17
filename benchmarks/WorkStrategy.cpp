#include "WorkStrategy.h"

#include <cassert>
#include <cmath>
#include <random>

namespace WorkStrategy
{
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

        std::random_device randomDevice;
        static thread_local std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<> distribution(
            0, config.initialNumberOfItems + config.numberOfItems);

        if (threadId >= ST) { // searching
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.contains(distribution(generator));
            }
        } else if (threadId >= RT) { // removing
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.remove(distribution(generator));
            }
        } else { // inserting
            for (std::size_t i = 0; i < config.numberOfItems; i++) {
                list.insert(distribution(generator));
            }
        }
    };
}
}
