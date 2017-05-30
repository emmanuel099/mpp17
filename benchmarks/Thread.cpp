#include "Thread.h"

#include <cassert>
#include <mutex>
#include <vector>

namespace Thread
{
static thread_local std::size_t ThreadId;

void parallel(std::function<void()> workFunction, std::size_t numberOfThreads)
{
    assert(numberOfThreads >= 1 &&
           "Parallel execution requires at least one thread");

    std::vector<std::thread> threads;
    threads.reserve(numberOfThreads - 1);
    for (std::size_t i = 1; i < numberOfThreads; i++) {
        threads.emplace_back([=] {
            ThreadId = i;
            workFunction();
        });
    }
    ThreadId = 0;
    workFunction();

    for (auto& thread : threads) {
        thread.join();
    }
}

void single(std::function<void()> workFunction)
{
    if (currentThreadId() == 0) {
        workFunction();
    }
}

void critical(std::function<void()> workFunction)
{
    static std::mutex mutex;

    std::lock_guard<std::mutex> lock(mutex);
    workFunction();
}

std::size_t currentThreadId()
{
    return ThreadId;
}
}
