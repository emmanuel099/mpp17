#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include "ConcurrentSkipList.h"

class ConcurrentSkipListTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        list = std::make_unique<ConcurrentSkipList<int, 16>>();
    }

    std::unique_ptr<SkipList<int>> list;
};

TEST_F(ConcurrentSkipListTest, InsertingMultipleElementsInParallelShouldWork)
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

#define ABSTRACT_SKIP_LIST_TEST_IMPL ConcurrentSkipListTest
#include "AbstractSkipListTest.h"
