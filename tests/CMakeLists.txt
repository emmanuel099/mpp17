include_directories (${SkipList_SOURCE_DIR}/src)

add_executable(skiplist_tests
    SkipListTest.cpp
    ConcurrentSkipListTest.cpp
)

target_link_libraries(skiplist_tests
    GTest::GTest
    GTest::Main
    Threads::Threads
)

target_compile_options(skiplist_tests
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET skiplist_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET skiplist_tests PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)

add_test(AllTests skiplist_tests)
GTEST_ADD_TESTS(skiplist_tests "" AUTO)
