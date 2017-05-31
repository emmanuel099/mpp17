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

set(BENCHMARKS
    ConcurrentSkipListBenchmark.cpp
    LazySkipListBenchmark.cpp
)

foreach(BENCHMARK ${BENCHMARKS})
    set(TARGET_NAME Benchmark_${BENCHMARK})
    string(REPLACE ".cpp" "" TARGET_NAME ${TARGET_NAME})

    add_executable(${TARGET_NAME}
        ${BENCHMARK}
    )

    target_link_libraries(${TARGET_NAME}
        skiplistcore
        benchmarkcore
        ${Boost_LIBRARIES}
    )

    target_compile_options(${TARGET_NAME}
        PUBLIC
            -Wall
            -Wextra
            -Wno-deprecated
        PRIVATE
            -DCOLLECT_STATISTICS
    )

    set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${TARGET_NAME} PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)

    add_test(${TARGET_NAME} ${TARGET_NAME})
endforeach()
