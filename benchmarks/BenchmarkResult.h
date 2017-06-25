#pragma once

#include <cstdint>
#include <string>

struct BenchmarkResult {
    std::uint16_t repetition;
    double totalTime;       // s
    double totalThroughput; // per s
    std::size_t numberOfInsertions;
    double percentageFailedInsert;
    double averageNumberOfRetriesDuringInsert;
    double insertThroughput; // per s
    std::size_t numberOfRemovals;
    double percentageFailedRemove;
    double averageNumberOfRetriesDuringRemove;
    double removeThroughput; // per s
    std::size_t numberOfFinds;
    double averageNumberOfRetriesDuringFind;
    double findThroughput; // per s
};

std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result);
