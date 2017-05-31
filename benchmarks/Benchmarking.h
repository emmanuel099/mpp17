#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "SkipList.h"

struct BenchmarkConfiguration {
    std::string description;
    std::size_t numberOfThreads;
    std::size_t numberOfItems;        /**< number of items per thread */
    std::size_t initialNumberOfItems; /**< fill list before benchmarking */
    std::function<std::unique_ptr<SkipList<int>>()> listFactory;
    std::function<void(const BenchmarkConfiguration&, SkipList<int>&)>
        workStrategy;
};

std::ostream& operator<<(std::ostream& out,
                         const BenchmarkConfiguration& config);

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

BenchmarkResult runBenchmark(const BenchmarkConfiguration& config);

std::vector<BenchmarkResult>
runBenchmarks(const std::vector<BenchmarkConfiguration>& configs);

void saveBenchmarkResultsAsCsv(
    const std::vector<BenchmarkConfiguration>& configs,
    const std::vector<BenchmarkResult>& results,
    const std::string& fileNamePrefix);
