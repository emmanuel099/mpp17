#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "BaseBenchmarkConfiguration.h"
#include "BenchmarkResult.h"
#include "SkipList.h"
#include "WorkStrategy.h"

struct BenchmarkConfiguration : public BaseBenchmarkConfiguration {
    std::function<std::unique_ptr<SkipList<long>>()> listFactory;
    WorkStrategy::Workload workStrategy;
};

BenchmarkResult runBenchmark(const BenchmarkConfiguration& config);

std::vector<BenchmarkResult>
runBenchmarks(const std::vector<BenchmarkConfiguration>& configs);

void saveBenchmarkResultsAsCsv(
    const std::vector<BenchmarkConfiguration>& configs,
    const std::vector<BenchmarkResult>& results,
    const std::string& fileNamePrefix);
