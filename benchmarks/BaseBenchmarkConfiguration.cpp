#include "BaseBenchmarkConfiguration.h"

#include <iostream>

std::ostream& operator<<(std::ostream& out,
                         const BaseBenchmarkConfiguration& config)
{
    out << "Description: " << config.description
        << "\nRepetitions: " << std::to_string(config.repetitions)
        << "\nSkip-List height: " << std::to_string(config.listHeight)
        << "\nNumber of threads: " << std::to_string(config.numberOfThreads)
        << "\nNumber of items: " << std::to_string(config.numberOfItems)
        << "\nInitial number of items: "
        << std::to_string(config.initialNumberOfItems);

    return out;
}
