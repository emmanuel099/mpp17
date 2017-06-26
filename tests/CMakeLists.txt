include_directories (${SkipList_SOURCE_DIR}/src)

add_executable(skiplist_tests
    SequentialSkipList.cpp
    ConcurrentSkipListTest.cpp
    LazySkipListTest.cpp
    LockFreeSkipListTest.cpp
)

target_link_libraries(skiplist_tests
    skiplistcore
    GTest::GTest
    GTest::Main
)

target_compile_options(skiplist_tests
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET skiplist_tests PROPERTY CXX_STANDARD 14)

add_test(AllTests skiplist_tests)
GTEST_ADD_TESTS(skiplist_tests "" AUTO)
