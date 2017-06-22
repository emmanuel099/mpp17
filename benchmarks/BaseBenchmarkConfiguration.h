#pragma once

#include <cstdint>
#include <string>

enum class Scaling { Weak, Strong };

std::ostream& operator<<(std::ostream& out, Scaling scaling);

struct BaseBenchmarkConfiguration {
    std::uint16_t repetitions;
    std::uint16_t listHeight;
    std::string description;
    std::size_t numberOfThreads;
    std::size_t numberOfItems;        /**< number of items per thread */
    std::size_t initialNumberOfItems; /**< fill list before benchmarking */
    Scaling scalingMode;
};

std::ostream& operator<<(std::ostream& out,
                         const BaseBenchmarkConfiguration& config);
