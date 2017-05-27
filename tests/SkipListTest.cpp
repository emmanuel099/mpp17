#include <gtest/gtest.h>
#include <memory>

#include "SkipList.h"

class SkipListTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        list = std::make_unique<SkipList<int, 16>>();
    }

    // virtual void TearDown() {}

    std::unique_ptr<SkipList<int, 16>> list;
};

TEST_F(SkipListTest, ShouldBeEmptyAfterInitialization)
{
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());
}

TEST_F(SkipListTest, ShouldNotBeEmptyAfterInsert)
{
    // WHEN
    list->insert(42);

    // THEN
    EXPECT_FALSE(list->empty());
    EXPECT_EQ(1, list->size());
}

TEST_F(SkipListTest, ShouldBeEmptyAfterRemovingLastElementInList)
{
    // PREPARE
    list->insert(42);

    // WHEN
    list->remove(42);

    // THEN
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());
}

TEST_F(SkipListTest, ShouldBeEmptyAfterClear)
{
    // PREPARE
    list->insert(21);
    list->insert(42);

    // WHEN
    list->clear();

    // THEN
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());
}

TEST_F(SkipListTest, InsertingNonExistingElementShouldWork)
{
    // WHEN
    EXPECT_FALSE(list->contains(12));

    // THEN
    EXPECT_TRUE(list->insert(12));
}

TEST_F(SkipListTest, InsertingExistingElementShouldFail)
{
    // WHEN
    list->insert(12);
    EXPECT_TRUE(list->contains(12));

    // THEN
    EXPECT_FALSE(list->insert(12));
}

TEST_F(SkipListTest, ShouldFindInsertedElement)
{
    // WHEN
    list->insert(42);

    // THEN
    EXPECT_TRUE(list->contains(42));
}

TEST_F(SkipListTest, InsertingMultipleElementsShouldWork)
{
    // WHEN
    list->insert(12);
    list->insert(42);
    list->insert(21);

    // THEN
    EXPECT_EQ(3, list->size());
    EXPECT_TRUE(list->contains(12));
    EXPECT_TRUE(list->contains(21));
    EXPECT_TRUE(list->contains(42));
}

TEST_F(SkipListTest, InsertingAHugeAmountOfElementsShouldWork)
{
    // WHEN
    for (int i = 0; i < 1000000; i++) {
        list->insert(i);
    }

    // THEN
    EXPECT_EQ(1000000, list->size());
}

TEST_F(SkipListTest, RemovingExistingElementShouldWork)
{
    // WHEN
    list->insert(12);
    EXPECT_TRUE(list->contains(12));

    // THEN
    EXPECT_TRUE(list->remove(12));
    EXPECT_FALSE(list->contains(12));
}

TEST_F(SkipListTest, RemovingNonExistingElementShouldFail)
{
    // WHEN
    EXPECT_FALSE(list->contains(12));

    // THEN
    EXPECT_FALSE(list->remove(12));
}
