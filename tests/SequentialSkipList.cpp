#include <gtest/gtest.h>
#include <memory>

#include "SequentialSkipList.h"

class SequentialSkipListTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        list = std::make_unique<SequentialSkipList<int, 16>>();
    }

    std::unique_ptr<SkipList<int>> list;
};

#define ABSTRACT_SKIP_LIST_TEST_IMPL SequentialSkipListTest
#include "AbstractSkipListTest.h"
