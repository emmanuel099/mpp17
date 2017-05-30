#include <algorithm>
#include <iostream>

#include "Benchmarking.h"
#include "ConcurrentSkipList.h"
#include "WorkStrategy.h"

int main(int argc, char** argv)
{
    std::vector<BenchmarkConfiguration> benchmarks;

    BenchmarkConfiguration benchmarkTemplate;
    benchmarkTemplate.numberOfThreads = 8;
    benchmarkTemplate.numberOfItems = 250000;
    benchmarkTemplate.initialNumberOfItems = 10000;
    benchmarkTemplate.listFactory = [] {
        return std::make_unique<ConcurrentSkipList<int, 16>>();
    };

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - ascending insert";
        benchmark.workStrategy = WorkStrategy::AscendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - descending insert";
        benchmark.workStrategy = WorkStrategy::DescendingInsert;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - interleaving insert";
        benchmark.workStrategy = WorkStrategy::InterleavingInsert;
        benchmarks.push_back(benchmark);
    }

    /*{
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - ascending remove";
        benchmark.numberOfItems = 500000;
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::AscendingRemove;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - descending remove";
        benchmark.numberOfItems = 500000;
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::DescendingRemove;
        benchmarks.push_back(benchmark);
    }

    {
        BenchmarkConfiguration benchmark = benchmarkTemplate;
        benchmark.description = "ConcurrentSkipList - interleaving remove";
        benchmark.numberOfItems = 500000;
        benchmark.initialNumberOfItems =
            benchmark.numberOfThreads * benchmark.numberOfItems;
        benchmark.workStrategy = WorkStrategy::InterleavingRemove;
        benchmarks.push_back(benchmark);
    }*/

    for (const auto& benchmark : benchmarks) {
        std::cout << "Running benchmark with configuration:\n"
                  << benchmark << "\n"
                  << "--------------------------------------------"
                  << std::endl;

        const auto result = runBenchmark(benchmark);

        std::cout << "Finished benchmark, result is:\n"
                  << result << "\n"
                  << "============================================"
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
