#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Benchmarking.h"
#include "ConcurrentSkipList.h"
#include "LazySkipList.h"
#include "LockFreeSkipList.h"
#include "MemoryManagedLazySkipList.h"
#include "WorkStrategy.h"

template <template <typename, std::uint16_t> class T,
          std::uint16_t SkipListHeight>
static void
createBenchmarksForListHeight(std::vector<BenchmarkConfiguration>& benchmarks)
{
    BenchmarkConfiguration benchmarkTemplate;
    benchmarkTemplate.repetitions = 5;
    benchmarkTemplate.listHeight = SkipListHeight;
    benchmarkTemplate.numberOfItems = 250000;
    benchmarkTemplate.initialNumberOfItems = 10000;
    benchmarkTemplate.listFactory = [] {
        return std::make_unique<T<long, SkipListHeight>>();
    };

    for (std::size_t threads = 1;
         threads <= std::thread::hardware_concurrency(); threads *= 2) {
        benchmarkTemplate.numberOfThreads = threads;

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "ascending insert - no failed inserts";
            benchmark.workStrategy =
                WorkStrategy::createAscendingInsertWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "descending insert - no failed inserts";
            benchmark.workStrategy =
                WorkStrategy::createDescendingInsertWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "interleaving insert - no failed inserts";
            benchmark.workStrategy =
                WorkStrategy::createInterleavingInsertWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "ascending remove - no failed removes";
            benchmark.workStrategy =
                WorkStrategy::createAscendingRemoveWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "descending remove - no failed removes";
            benchmark.workStrategy =
                WorkStrategy::createDescendingRemoveWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "interleaving remove - no failed removes";
            benchmark.workStrategy =
                WorkStrategy::createInterleavingRemoveWorkload();
            benchmarks.push_back(benchmark);
        }

        {
            auto benchmark = benchmarkTemplate;
            benchmark.description = "mixed workload - 70% insert, 30% remove";
            benchmark.workStrategy =
                WorkStrategy::createMixedWorkload(0.7, 0.3);
            benchmarks.push_back(benchmark);
        }
    }
}

int main(int argc, char** argv)
{
    auto benchmark_enabled = [argc, argv](std::string name) {
        return argc == 1 or
               std::find(argv + 1, argv + argc, name) != argv + argc;
    };

    if (benchmark_enabled("ConcurrentSkipList")) {
        std::cout << "Running ConcurrentSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<ConcurrentSkipList, 8>(benchmarks);
        createBenchmarksForListHeight<ConcurrentSkipList, 16>(benchmarks);
        createBenchmarksForListHeight<ConcurrentSkipList, 64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks),
                                  "ConcurrentSkipList");
    }

    if (benchmark_enabled("LazySkipList")) {
        std::cout << "Running LazySkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<LazySkipList, 8>(benchmarks);
        createBenchmarksForListHeight<LazySkipList, 16>(benchmarks);
        createBenchmarksForListHeight<LazySkipList, 64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks),
                                  "LazySkipList");
    }

    if (benchmark_enabled("MemoryManagedLazySkipList")) {
        std::cout << "Running MemoryManagedLazySkipList benchmark:"
                  << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<MemoryManagedLazySkipList, 8>(benchmarks);
        createBenchmarksForListHeight<MemoryManagedLazySkipList, 16>(
            benchmarks);
        createBenchmarksForListHeight<MemoryManagedLazySkipList, 64>(
            benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks),
                                  "MemoryManagedLazySkipList");
    }

    if (benchmark_enabled("LockFreeSkipList")) {
        std::cout << "Running LockFreeSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<LockFreeSkipList, 8>(benchmarks);
        createBenchmarksForListHeight<LockFreeSkipList, 16>(benchmarks);
        createBenchmarksForListHeight<LockFreeSkipList, 64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks),
                                  "LockFreeSkipList");
    }

    return EXIT_SUCCESS;
}
