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

struct BenchmarkData {
    BenchmarkConfiguration config;
    std::vector<BenchmarkResult> results; // one result per repetition
};

BenchmarkData runBenchmark(const BenchmarkConfiguration& config);

std::vector<BenchmarkData>
runBenchmarks(const std::vector<BenchmarkConfiguration>& configs);

void saveBenchmarksAsCsv(const std::vector<BenchmarkData>& benchmarks,
                         const std::string& fileNamePrefix);
