#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "Benchmarking.h"
#include "ConcurrentSkipList.h"
#include "LazySkipList.h"
#include "LockFreeSkipList.h"
#include "MMLazySkipList.h"
#include "MMLockFreeSkipList.h"
#include "SequentialSkipList.h"
#include "WorkStrategy.h"

template <template <typename, std::uint16_t> class T,
          std::uint16_t SkipListHeight>
static void createBenchmarks(std::vector<BenchmarkConfiguration>& benchmarks,
                             const std::vector<Scaling>& scalingModes,
                             const std::vector<std::size_t>& threadCounts,
                             const std::vector<std::size_t>& initialSizes)
{
    BenchmarkConfiguration benchmarkTemplate;
    benchmarkTemplate.repetitions = 30;
    benchmarkTemplate.listHeight = SkipListHeight;
    benchmarkTemplate.numberOfItems = 1000000;
    benchmarkTemplate.listFactory = [] {
        return std::make_unique<T<long, SkipListHeight>>();
    };

    for (auto initialSize : initialSizes) {
        benchmarkTemplate.initialNumberOfItems = initialSize;

        for (auto scalingMode : scalingModes) {
            benchmarkTemplate.scalingMode = scalingMode;

            for (auto threads : threadCounts) {
                benchmarkTemplate.numberOfThreads = threads;

                /*{
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "ascending insert - no failed inserts";
                    benchmark.workStrategy =
                        WorkStrategy::createAscendingInsertWorkload();
                    benchmarks.push_back(benchmark);
                }

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "descending insert - no failed inserts";
                    benchmark.workStrategy =
                        WorkStrategy::createDescendingInsertWorkload();
                    benchmarks.push_back(benchmark);
                }*/

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "interleaving insert - no failed inserts";
                    benchmark.workStrategy =
                        WorkStrategy::createInterleavingInsertWorkload();
                    benchmarks.push_back(benchmark);
                }

                /*{
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "ascending remove - no failed removes";
                    benchmark.workStrategy =
                        WorkStrategy::createAscendingRemoveWorkload();
                    benchmarks.push_back(benchmark);
                }

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "descending remove - no failed removes";
                    benchmark.workStrategy =
                        WorkStrategy::createDescendingRemoveWorkload();
                    benchmarks.push_back(benchmark);
                }*/

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "interleaving remove - no failed removes";
                    benchmark.workStrategy =
                        WorkStrategy::createInterleavingRemoveWorkload();
                    benchmarks.push_back(benchmark);
                }

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                        "mixed workload - 70% insert / 30% remove";
                    benchmark.workStrategy =
                        WorkStrategy::createMixedWorkload(0.7, 0.3);
                    benchmarks.push_back(benchmark);
                }

                {
                    auto benchmark = benchmarkTemplate;
                    benchmark.description =
                    "mixed workload - 50% insert / 20% remove / 30% search";
                    benchmark.workStrategy =
                        WorkStrategy::createMixedWorkload(0.5, 0.2);
                    benchmarks.push_back(benchmark);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    auto benchmark_enabled = [argc, argv](std::string name) {
        return argc == 1 or
               std::find(argv + 1, argv + argc, name) != argv + argc;
    };

    const std::vector<Scaling> scalingModes = {Scaling::Strong};
    const std::vector<std::size_t> threadCounts = {1, 2, 4, 8, 12, 16, 24, 32, 40, 48};
    const std::vector<std::size_t> initialSizes = {0};

    if (benchmark_enabled("SequentialSkipList")) {
        std::cout << "Running SequentialSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<SequentialSkipList, 16>(benchmarks, scalingModes, {1},
                                                 initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "SequentialSkipList");
    }

    if (benchmark_enabled("ConcurrentSkipList")) {
        std::cout << "Running ConcurrentSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<ConcurrentSkipList, 16>(benchmarks, scalingModes,
                                                 threadCounts, initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "ConcurrentSkipList");
    }

    if (benchmark_enabled("LazySkipList")) {
        std::cout << "Running LazySkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<LazySkipList, 16>(benchmarks, scalingModes,
                                           threadCounts, initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "LazySkipList");
    }

    if (benchmark_enabled("LockFreeSkipList")) {
        std::cout << "Running LockFreeSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<LockFreeSkipList, 16>(benchmarks, scalingModes,
                                               threadCounts, initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "LockFreeSkipList");
    }

    if (benchmark_enabled("MMLazySkipList")) {
        std::cout << "Running MMLazySkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<MMLazySkipList, 16>(benchmarks, scalingModes,
                                             threadCounts, initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "MMLazySkipList");
    }

    if (benchmark_enabled("MMLockFreeSkipList")) {
        std::cout << "Running MMLockFreeSkipList benchmark:" << std::endl;

        std::vector<BenchmarkConfiguration> benchmarks;
        createBenchmarks<MMLockFreeSkipList, 16>(benchmarks, scalingModes,
                                                 {2, 3, 4, 5, 6, 7, 8}, initialSizes);

        saveBenchmarksAsCsv(runBenchmarks(benchmarks), "MMLockFreeSkipList");
    }

    return EXIT_SUCCESS;
}
