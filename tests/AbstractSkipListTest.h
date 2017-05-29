#include <gtest/gtest.h>

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, ShouldBeEmptyAfterInitialization)
{
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, ShouldNotBeEmptyAfterInsert)
{
    // WHEN
    list->insert(42);

    // THEN
    EXPECT_FALSE(list->empty());
    EXPECT_EQ(1, list->size());
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL,
       ShouldBeEmptyAfterRemovingLastElementInList)
{
    // PREPARE
    list->insert(42);

    // WHEN
    list->remove(42);

    // THEN
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(0, list->size());
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, ShouldBeEmptyAfterClear)
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

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, InsertingNonExistingElementShouldWork)
{
    // WHEN
    EXPECT_FALSE(list->contains(12));

    // THEN
    EXPECT_TRUE(list->insert(12));
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, InsertingExistingElementShouldFail)
{
    // WHEN
    list->insert(12);
    EXPECT_TRUE(list->contains(12));

    // THEN
    EXPECT_FALSE(list->insert(12));
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, ShouldFindInsertedElement)
{
    // WHEN
    list->insert(42);

    // THEN
    EXPECT_TRUE(list->contains(42));
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, InsertingMultipleElementsShouldWork)
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

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, InsertingElementsAfterClearShouldWork)
{
    // PREPARE
    for (int i = 0; i < 3; i++) {
        list->insert(i);
    }
    list->clear();

    // WHEN
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(list->insert(i));
    }

    // THEN
    EXPECT_EQ(3, list->size());
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, RemovingExistingElementShouldWork)
{
    // WHEN
    list->insert(12);
    EXPECT_TRUE(list->contains(12));

    // THEN
    EXPECT_TRUE(list->remove(12));
    EXPECT_FALSE(list->contains(12));
}

TEST_F(ABSTRACT_SKIP_LIST_TEST_IMPL, RemovingNonExistingElementShouldFail)
{
    // WHEN
    EXPECT_FALSE(list->contains(12));

    // THEN
    EXPECT_FALSE(list->remove(12));
}
