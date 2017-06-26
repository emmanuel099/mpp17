include_directories (${SkipList_SOURCE_DIR}/src)

add_library(benchmarkcore STATIC
    Benchmarking.cpp
    BaseBenchmarkConfiguration.cpp
    BenchmarkResult.cpp
    Thread.cpp
    WorkStrategy.cpp
)

target_link_libraries(benchmarkcore
    skiplistcore
    ${Boost_LIBRARIES}
)

target_compile_options(benchmarkcore
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET benchmarkcore PROPERTY CXX_STANDARD 14)

################################################################################

set(BENCHMARKS
    SkipListBenchmark.cpp
)

foreach(BENCHMARK ${BENCHMARKS})
    set(TARGET_NAME ${BENCHMARK})
    string(REPLACE ".cpp" "" TARGET_NAME ${TARGET_NAME})

    add_executable(${TARGET_NAME}
        ${BENCHMARK}
    )

    target_link_libraries(${TARGET_NAME}
        benchmarkcore
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

    add_test(${TARGET_NAME} ${TARGET_NAME})
endforeach()
