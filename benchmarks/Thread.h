#pragma once

#include <cstdint>
#include <functional>
#include <thread>

namespace Thread
{
void parallel(std::function<void()> workFunction, std::size_t numberOfThreads);

void single(std::function<void()> workFunction);

void critical(std::function<void()> workFunction);

std::size_t currentThreadId();
}
