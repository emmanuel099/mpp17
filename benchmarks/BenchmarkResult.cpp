#include "BenchmarkResult.h"

#include <iostream>

std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result)
{
    out << "Repetition: " << std::to_string(result.repetition)
        << "\nTotal time: " << std::to_string(result.totalTime) << " s"
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
