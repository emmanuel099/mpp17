#pragma once

#include <cstdint>
#include <string>

struct BaseBenchmarkConfiguration {
    std::uint16_t repetitions;
    std::uint16_t listHeight;
    std::string description;
    std::size_t numberOfThreads;
    std::size_t numberOfItems;        /**< number of items per thread */
    std::size_t initialNumberOfItems; /**< fill list before benchmarking */
};

std::ostream& operator<<(std::ostream& out,
                         const BaseBenchmarkConfiguration& config);
