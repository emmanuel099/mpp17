#pragma once

#include <cstdint>
#include <string>

struct BenchmarkResult {
    double totalTime;       // s
    double totalThroughput; // per s
    std::size_t numberOfInsertions;
    double percentageFailedInsert;
    std::size_t averageNumberOfRetriesDuringInsert;
    double insertThroughput; // per s
    std::size_t numberOfRemovals;
    double percentageFailedRemove;
    std::size_t averageNumberOfRetriesDuringRemove;
    double removeThroughput; // per s
    std::size_t numberOfFinds;
    std::size_t averageNumberOfRetriesDuringFind;
    double findThroughput; // per s
};

std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result);
