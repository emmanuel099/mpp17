#include <algorithm>
#include <iostream>
#include <thread>

#include "Benchmarking.h"
#include "SequentialSkipList.h"
#include "WorkStrategy.h"

template <std::uint16_t SkipListHeight>
static void
createBenchmarksForListHeight(std::vector<BenchmarkConfiguration>& benchmarks)
{
    BenchmarkConfiguration benchmarkTemplate;
    benchmarkTemplate.listHeight = SkipListHeight;
    benchmarkTemplate.numberOfItems = 250000;
    benchmarkTemplate.initialNumberOfItems = 10000;
    benchmarkTemplate.numberOfThreads = 1;
    benchmarkTemplate.listFactory = [] {
        return std::make_unique<SequentialSkipList<int, SkipListHeight>>();
    };

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ascending insert - no failed inserts";
        benchmark.workStrategy = WorkStrategy::AscendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "descending insert - no failed inserts";
        benchmark.workStrategy = WorkStrategy::DescendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "interleaving insert - no failed inserts";
        benchmark.workStrategy = WorkStrategy::InterleavingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ascending remove - no failed removes";
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::AscendingRemove;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "descending remove - no failed removes";
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::DescendingRemove;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "interleaving remove - no failed removes";
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::InterleavingRemove;
        benchmarks.push_back(benchmark);
    }
}

int main(int argc, char** argv)
{
    std::vector<BenchmarkConfiguration> benchmarks;
    createBenchmarksForListHeight<8>(benchmarks);
    createBenchmarksForListHeight<16>(benchmarks);
    createBenchmarksForListHeight<64>(benchmarks);

    const auto results = runBenchmarks(benchmarks);
    saveBenchmarkResultsAsCsv(benchmarks, results, "SequentialSkipList");

    return EXIT_SUCCESS;
}
