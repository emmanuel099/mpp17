add_library(skiplistcore STATIC
    SkipListStatistics.cpp
)

target_link_libraries(skiplistcore
    Threads::Threads
)

target_compile_options(skiplistcore
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET skiplistcore PROPERTY CXX_STANDARD 11)
set_property(TARGET skiplistcore PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)

################################################################################

add_executable(skiplist
    main.cpp
)

target_link_libraries(skiplist
    skiplistcore
)

target_compile_options(skiplist
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET skiplist PROPERTY CXX_STANDARD 11)
set_property(TARGET skiplist PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)
