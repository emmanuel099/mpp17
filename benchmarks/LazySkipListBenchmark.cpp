#include <algorithm>
#include <iostream>

#include "Benchmarking.h"
#include "LazySkipList.h"
#include "WorkStrategy.h"

int main(int argc, char** argv)
{
    std::vector<BenchmarkConfiguration> benchmarks;

    BenchmarkConfiguration benchmarkTemplate;
    benchmarkTemplate.numberOfThreads = 8;
    benchmarkTemplate.numberOfItems = 250000;
    benchmarkTemplate.initialNumberOfItems = 10000;
    benchmarkTemplate.listFactory = [] {
        return std::make_unique<LazySkipList<long, 16>>();
    };

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ascending insert";
        benchmark.workStrategy = WorkStrategy::AscendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "descending insert";
        benchmark.workStrategy = WorkStrategy::DescendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "interleaving insert";
        benchmark.workStrategy = WorkStrategy::InterleavingInsert;
        benchmarks.push_back(benchmark);
    }

    const auto results = runBenchmarks(benchmarks);
    saveBenchmarkResultsAsCsv(benchmarks, results, "LazySkipList");

    return EXIT_SUCCESS;
}
