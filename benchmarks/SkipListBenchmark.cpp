#include <algorithm>
#include <iostream>
#include <string> 
#include <vector> 

#include "Benchmarking.h"
#include "LockFreeSkipList.h"
#include "LazySkipList.h"
#include "MemoryManagedLazySkipList.h"
#include "ConcurrentSkipList.h"
#include "WorkStrategy.h"

template <template <typename, std::uint16_t> class T, std::uint16_t SkipListHeight>
static void
createBenchmarksForListHeight(std::vector<BenchmarkConfiguration>& benchmarks)
{
    BenchmarkConfiguration benchmarkTemplate;
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
}

int main(int argc, char** argv)
{
    auto benchmark_enabled = [argc, argv](std::string name) {
        return argc == 1 or std::find(argv + 1, argv + argc, name) != argv + argc;
    };
    
    if (benchmark_enabled("ConcurrentSkipList")) {
        std::cout << "Running ConcurrentSkipList benchmark:" << std::endl;
        
        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<ConcurrentSkipList,8>(benchmarks);
        createBenchmarksForListHeight<ConcurrentSkipList,16>(benchmarks);
        createBenchmarksForListHeight<ConcurrentSkipList,64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks), "ConcurrentSkipList");
    }
    
    if (benchmark_enabled("LazySkipList")) {
        std::cout << "Running LazySkipList benchmark:" << std::endl;
        
        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<LazySkipList,8>(benchmarks);
        createBenchmarksForListHeight<LazySkipList,16>(benchmarks);
        createBenchmarksForListHeight<LazySkipList,64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks), "LazySkipList");
    }
    
    if (benchmark_enabled("MemoryManagedLazySkipList")) {
        std::cout << "Running MemoryManagedLazySkipList benchmark:" << std::endl;
        
        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<MemoryManagedLazySkipList,8>(benchmarks);
        createBenchmarksForListHeight<MemoryManagedLazySkipList,16>(benchmarks);
        createBenchmarksForListHeight<MemoryManagedLazySkipList,64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks), "MemoryManagedLazySkipList");
    }
    
    if (benchmark_enabled("LockFreeSkipList")) {
        std::cout << "Running LockFreeSkipList benchmark:" << std::endl;
        
        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarksForListHeight<LockFreeSkipList,8>(benchmarks);
        createBenchmarksForListHeight<LockFreeSkipList,16>(benchmarks);
        createBenchmarksForListHeight<LockFreeSkipList,64>(benchmarks);

        saveBenchmarkResultsAsCsv(benchmarks, runBenchmarks(benchmarks), "LockFreeSkipList");
    }

    return EXIT_SUCCESS;
}
