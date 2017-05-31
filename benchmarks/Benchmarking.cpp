#include "Benchmarking.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>

#include <boost/thread/barrier.hpp>

#include "SkipListStatistics.h"
#include "Thread.h"
#include "Timer.h"

std::ostream& operator<<(std::ostream& out,
                         const BenchmarkConfiguration& config)
{
    out << "Description: " << config.description
        << "\nSkip-List height: " << std::to_string(config.listHeight)
        << "\nNumber of threads: " << std::to_string(config.numberOfThreads)
        << "\nNumber of items: " << std::to_string(config.numberOfItems)
        << "\nInitial number of items: "
        << std::to_string(config.initialNumberOfItems);

    return out;
}

std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result)
{
    out << "Total time: " << std::to_string(result.totalTime) << " s"
        << "\nTotal throughput: " << std::to_string(result.totalThroughput)
        << " Ops/s"
        << "\nInsertions: " << std::to_string(result.numberOfInsertions)
        << "\nFailed Insertions: "
        << std::to_string(result.percentageFailedInsert) << " %"
        << "\nAvr. No. Retries during Insert: "
        << std::to_string(result.averageNumberOfRetriesDuringInsert)
        << "\nInsert throughput: " << std::to_string(result.insertThroughput)
        << " Ops/s"
        << "\nRemovals: " << std::to_string(result.numberOfRemovals)
        << "\nFailed Removals: "
        << std::to_string(result.percentageFailedRemove) << " %"
        << "\nAvr. No. Retries during Remove: "
        << std::to_string(result.averageNumberOfRetriesDuringRemove)
        << "\nRemove throughput: " << std::to_string(result.removeThroughput)
        << " Ops/s"
        << "\nFind: " << std::to_string(result.numberOfFinds)
        << "\nAvr. No. Retries during Find: "
        << std::to_string(result.averageNumberOfRetriesDuringFind)
        << "\nFind throughput: " << std::to_string(result.findThroughput)
        << " Ops/s";

    return out;
}

BenchmarkResult runBenchmark(const BenchmarkConfiguration& config)
{
    const auto list = config.listFactory();

    // pre-fill list
    for (int i = 0; i < config.initialNumberOfItems; i++) {
        list->insert(i);
    }

    std::chrono::nanoseconds benchmarkDuration;
    SkipListStatistics statistics;

    boost::barrier barrier(config.numberOfThreads);

    Thread::parallel(
        [&] {
            SkipListStatistics::threadLocalInstance().reset();

            Timer<std::chrono::high_resolution_clock> timer;

            barrier.wait();
            timer.start();

            config.workStrategy(config, *list);

            barrier.wait();
            timer.stop();

            Thread::single([&] { benchmarkDuration = timer.elapsed(); });

            // merge the collected performance statistics of each thread
            Thread::critical([&] {
                SkipListStatistics::threadLocalInstance().mergeInto(statistics);
            });
        },
        config.numberOfThreads);

    BenchmarkResult result;
    result.totalTime = benchmarkDuration.count() / 1000.0 / 1000.0 / 1000.0;
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

    return result;
}

std::vector<BenchmarkResult>
runBenchmarks(const std::vector<BenchmarkConfiguration>& configs)
{
    std::vector<BenchmarkResult> results;
    results.reserve(configs.size());

    std::size_t counter = 0;
    const auto totalNumberStr = std::to_string(configs.size());
    for (const auto& config : configs) {
        ++counter;

        std::cout << "[" << std::to_string(counter) << "/" << totalNumberStr
                  << "] Running benchmark with configuration:\n"
                  << config << "\n"
                  << "--------------------------------------------"
                  << std::endl;

        const auto result = runBenchmark(config);
        results.push_back(result);

        std::cout << "[" << std::to_string(counter) << "/" << totalNumberStr
                  << "] Finished benchmark, result is:\n"
                  << result << "\n"
                  << "============================================"
                  << std::endl;
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

void saveBenchmarkResultsAsCsv(
    const std::vector<BenchmarkConfiguration>& configs,
    const std::vector<BenchmarkResult>& results,
    const std::string& fileNamePrefix)
{
    const auto seperator = ";";

    const auto dumpConfig = [&](std::ostream& out,
                                const BenchmarkConfiguration& config) {
        out << config.description << seperator
            << std::to_string(config.listHeight) << seperator
            << std::to_string(config.numberOfThreads) << seperator
            << std::to_string(config.numberOfItems) << seperator
            << std::to_string(config.initialNumberOfItems) << seperator;
    };

    const auto dumpResult = [&](std::ostream& out,
                                const BenchmarkResult& result) {
        out << std::to_string(result.totalTime) << seperator
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
    for (std::size_t i = 0; i < configs.size(); i++) {
        dumpConfig(file, configs.at(i));
        dumpResult(file, results.at(i));
        file << "\n";
    }
    file.close();

    std::cout << "Saved benchmark results to `" << fileName << "`" << std::endl;
}
