#include "Benchmarking.h"

#include <mutex>

#include <boost/thread/barrier.hpp>

#include "SkipListStatistics.h"
#include "Thread.h"
#include "Timer.h"

std::ostream& operator<<(std::ostream& out,
                         const BenchmarkConfiguration& config)
{
    out << "Description: " << config.description
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
        << std::to_string(result.percentageFailedInsert * 100.0) << " %"
        << "\nAvr. No. Retries during Insert: "
        << std::to_string(result.averageNumberOfRetriesDuringInsert)
        << "\nInsert throughput: " << std::to_string(result.insertThroughput)
        << " Ops/s"
        << "\nRemovals: " << std::to_string(result.numberOfRemovals)
        << "\nFailed Removals: "
        << std::to_string(result.percentageFailedRemove * 100.0) << " %"
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
