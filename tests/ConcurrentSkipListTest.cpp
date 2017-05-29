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

    // virtual void TearDown() {}

    std::unique_ptr<ConcurrentSkipList<int, 16>> list;
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

// basic method tests to verify that the correct sequential skip list methods
// are called

TEST_F(ConcurrentSkipListTest, EmptyAndSizeShouldWork)
{
    // INITIALLY
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());

    // WHEN
    list->insert(21);
    list->insert(42);

    // THEN
    EXPECT_FALSE(list->empty());
    EXPECT_EQ(2, list->size());
}

TEST_F(ConcurrentSkipListTest, InsertAndContainsShouldWork)
{
    // WHEN
    list->insert(12);

    // THEN
    EXPECT_TRUE(list->contains(12));
}

TEST_F(ConcurrentSkipListTest, RemoveShouldWork)
{
    // WHEN
    list->insert(12);
    EXPECT_TRUE(list->contains(12));

    // THEN
    EXPECT_TRUE(list->remove(12));
    EXPECT_FALSE(list->contains(12));
}

TEST_F(ConcurrentSkipListTest, ClearShouldWork)
{
    // PREPARE
    list->insert(21);
    list->insert(42);

    // WHEN
    list->clear();

    // THEN
    EXPECT_TRUE(list->empty());
}
