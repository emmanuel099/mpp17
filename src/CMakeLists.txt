add_executable(skiplist
    main.cpp
)

target_link_libraries(skiplist
    Threads::Threads
)

target_compile_options(skiplist
    PUBLIC
        -Wall
        -Wextra
        -Wno-deprecated
)

set_property(TARGET skiplist PROPERTY CXX_STANDARD 11)
set_property(TARGET skiplist PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_PATH};-checks=*)
