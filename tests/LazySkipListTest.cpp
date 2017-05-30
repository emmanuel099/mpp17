#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include "LazySkipList.h"

class LazySkipListTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        list = std::make_unique<LazySkipList<int, 16>>();
    }

    std::unique_ptr<SkipList<int>> list;
};

TEST_F(LazySkipListTest, InsertingMultipleElementsInParallelShouldWork)
{
    // WHEN
    const int numberOfThreads = 50;
    const int elementsPerThread = 200;

    std::vector<std::thread> threads;
    for (int i = 0; i < numberOfThreads; i++) {
        const int start = i * elementsPerThread;
        const int end = start + elementsPerThread;
        threads.emplace_back([&, start, end] {
            for (int i = start; i < end; ++i) {
                list->insert(i);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // THEN
    EXPECT_EQ(numberOfThreads * elementsPerThread, list->size());
}

#define ABSTRACT_SKIP_LIST_TEST_IMPL LazySkipListTest
#include "AbstractSkipListTest.h"
