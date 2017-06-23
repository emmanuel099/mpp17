#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include "LockFreeSkipList.h"
#include "MMLockFreeSkipList.h"

class LockFreeSkipListTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        list = std::make_unique<MMLockFreeSkipList<int, 16>>();
    }

    std::unique_ptr<SkipList<int>> list;
};

TEST_F(LockFreeSkipListTest, InsertingMultipleElementsInParallelShouldWork)
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

#define ABSTRACT_SKIP_LIST_TEST_IMPL LockFreeSkipListTest
#include "AbstractSkipListTest.h"
