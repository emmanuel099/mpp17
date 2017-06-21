#pragma once

#include <functional>

#include "BaseBenchmarkConfiguration.h"
#include "SkipList.h"

namespace WorkStrategy
{
struct Workload {
    using Function =
        std::function<void(const BaseBenchmarkConfiguration&, SkipList<long>&)>;

    Function prepare;
    Function work;
    Function cleanup;
};

Workload createAscendingInsertWorkload();

Workload createDescendingInsertWorkload();

Workload createInterleavingInsertWorkload();

Workload createAscendingRemoveWorkload();

Workload createDescendingRemoveWorkload();

Workload createInterleavingRemoveWorkload();

Workload createMixedWorkload(double insertingThreads, double removingThreads);
}
