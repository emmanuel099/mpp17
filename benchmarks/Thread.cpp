#include "Thread.h"

#include <cassert>
#include <mutex>
#include <vector>

namespace Thread
{
static thread_local std::size_t tl_ThreadId;
static thread_local std::mutex* tl_CriticalMutex = nullptr;

void parallel(std::function<void()> workFunction, std::size_t numberOfThreads)
{
    std::mutex criticalMutex;

    std::vector<std::thread> threads;
    threads.reserve(numberOfThreads);
    for (std::size_t threadId = 0; threadId < numberOfThreads; threadId++) {
        threads.emplace_back([&, threadId] {
            tl_ThreadId = threadId;
            tl_CriticalMutex = &criticalMutex; // mutex outlives the threads
            workFunction();
        });
    }

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
    assert(tl_CriticalMutex != nullptr);
    std::lock_guard<std::mutex> lock(*tl_CriticalMutex);
    workFunction();
}

std::size_t currentThreadId()
{
    return tl_ThreadId;
}
}
