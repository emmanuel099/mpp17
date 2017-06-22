#include "Benchmarking.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>

#include <boost/thread/barrier.hpp>

#include "SkipListStatistics.h"
#include "Thread.h"
#include "Timer.h"

BenchmarkData runBenchmark(const BenchmarkConfiguration& config)
{
    const auto workStrategy = config.workStrategy;

    boost::barrier barrier(config.numberOfThreads);

    // collect data of each repetition
    struct RepetitionData {
        std::uint16_t repetition;
        std::chrono::nanoseconds duration;
        SkipListStatistics statistics;
    };
    std::vector<RepetitionData> repData(config.repetitions);

    std::unique_ptr<SkipList<long>> list;

    Thread::parallel(
        [&] {
            Timer<std::chrono::high_resolution_clock> timer;

            for (std::uint16_t i = 0; i < config.repetitions; i++) {
                RepetitionData& data = repData[i];

                Thread::single([&] { list = config.listFactory(); });
                workStrategy.prepare(config, *list);

                SkipListStatistics::threadLocalInstance().reset();

                barrier.wait();
                timer.start();

                workStrategy.work(config, *list);

                barrier.wait();
                timer.stop();

                Thread::single([&] {
                    data.repetition = i + 1;
                    data.duration = timer.elapsed();
                });

                // merge the collected performance statistics of each thread
                Thread::critical([&] {
                    SkipListStatistics::threadLocalInstance().mergeInto(
                        data.statistics);
                });

                workStrategy.cleanup(config, *list);
            }
        },
        config.numberOfThreads);

    // post-compute the collected data
    BenchmarkData benchmarkData;
    benchmarkData.config = config;
    for (const auto& rep : repData) {
        const auto& statistics = rep.statistics;

        BenchmarkResult result;
        result.repetition = rep.repetition;
        result.totalTime = rep.duration.count() / 1000.0 / 1000.0 / 1000.0;
        result.totalThroughput =
            (statistics.numberOfInserts() + statistics.numberOfDeletions() +
             statistics.numberOfLookups()) /
            result.totalTime;
        result.numberOfInsertions = statistics.numberOfInserts();
        result.percentageFailedInsert = statistics.percentageFailedInserts();
        result.averageNumberOfRetriesDuringInsert =
            statistics.averageNumberOfRetriesDuringInsert();
        result.insertThroughput = result.numberOfInsertions / result.totalTime;
        result.numberOfRemovals = statistics.numberOfDeletions();
        result.percentageFailedRemove = statistics.percentageFailedDeletions();
        result.averageNumberOfRetriesDuringRemove =
            statistics.averageNumberOfRetriesDuringDeletion();
        result.removeThroughput = result.numberOfRemovals / result.totalTime;
        result.numberOfFinds = statistics.numberOfLookups();
        result.averageNumberOfRetriesDuringFind =
            statistics.averageNumberOfRetriesDuringLookup();
        result.findThroughput = result.numberOfFinds / result.totalTime;

        benchmarkData.results.push_back(result);
    }

    return benchmarkData;
}

std::vector<BenchmarkData>
runBenchmarks(const std::vector<BenchmarkConfiguration>& configs)
{
    std::vector<BenchmarkData> results;
    results.reserve(configs.size());

    Timer<std::chrono::steady_clock> timer;

    std::size_t counter = 0;
    const auto totalNumberStr = std::to_string(configs.size());
    for (const auto& config : configs) {
        ++counter;

        std::cout << "[" << std::to_string(counter) << "/" << totalNumberStr
                  << "] Running benchmark with configuration:\n"
                  << config << "\n"
                  << "--------------------------------------------"
                  << std::endl;

        timer.start();
        const auto result = runBenchmark(config);
        timer.stop();

        std::cout << "Finished benchmark after "
                  << std::to_string(timer.elapsed().count() / 1000.0 / 1000.0 /
                                    1000.0)
                  << " seconds\n"
                  << "============================================"
                  << std::endl;

        results.push_back(result);
    }

    return results;
}

static std::string currentDateTimeStr()
{
    const auto now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d_%X");
    return ss.str();
}

void saveBenchmarksAsCsv(const std::vector<BenchmarkData>& benchmarks,
                         const std::string& fileNamePrefix)
{
    const auto seperator = ";";

    const auto dumpConfig = [&](std::ostream& out,
                                const BenchmarkConfiguration& config) {
        out << config.description << seperator
            << std::to_string(config.listHeight) << seperator
            << std::to_string(config.numberOfThreads) << seperator
            << std::to_string(config.numberOfItems) << seperator
            << std::to_string(config.initialNumberOfItems) << seperator
            << config.scalingMode << seperator;
    };

    const auto dumpResult = [&](std::ostream& out,
                                const BenchmarkResult& result) {
        out << std::to_string(result.repetition) << seperator
            << std::to_string(result.totalTime) << seperator
            << std::to_string(result.totalThroughput) << seperator
            << std::to_string(result.numberOfInsertions) << seperator
            << std::to_string(result.percentageFailedInsert) << seperator
            << std::to_string(result.averageNumberOfRetriesDuringInsert)
            << seperator << std::to_string(result.insertThroughput) << seperator
            << std::to_string(result.numberOfRemovals) << seperator
            << std::to_string(result.percentageFailedRemove) << seperator
            << std::to_string(result.averageNumberOfRetriesDuringRemove)
            << seperator << std::to_string(result.removeThroughput) << seperator
            << std::to_string(result.numberOfFinds) << seperator
            << std::to_string(result.averageNumberOfRetriesDuringFind)
            << seperator << std::to_string(result.findThroughput) << seperator;
    };

    char hostname[50];
    gethostname(hostname, 50);
    const auto fileName =
        fileNamePrefix + "_" + hostname + "_" + currentDateTimeStr() + ".csv";

    std::ofstream file(fileName, std::ofstream::trunc);
    for (const auto& benchmark : benchmarks) {
        for (const auto& result : benchmark.results) {
            dumpConfig(file, benchmark.config);
            dumpResult(file, result);
            file << "\n";
        }
    }
    file.close();

    std::cout << "Saved benchmark results to `" << fileName << "`" << std::endl;
}
