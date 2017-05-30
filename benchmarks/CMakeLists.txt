include_directories (${SkipList_SOURCE_DIR}/src)

add_library(benchmarkcore
    Benchmarking.cpp
    Thread.cpp
)

target_compile_options(benchmarkcore
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET benchmarkcore PROPERTY CXX_STANDARD 14)
set_property(TARGET benchmarkcore PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)

################################################################################

add_executable(skiplist_benchmarks
    SkipListBenchmark.cpp
)

target_link_libraries(skiplist_benchmarks
    skiplistcore
    benchmarkcore
    ${Boost_LIBRARIES}
)

target_compile_options(skiplist_benchmarks
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
    PRIVATE
        -DCOLLECT_STATISTICS
)

set_property(TARGET skiplist_benchmarks PROPERTY CXX_STANDARD 14)
set_property(TARGET skiplist_benchmarks PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)

add_test(AllBenchmarks skiplist_benchmarks)
