//
// Created by JinHai on 2022/11/14.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class CharTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(CharTypeTest, Char0) {
    using namespace infinity;

    {
        ptr_t str = new char[6]{};
        GlobalResourceUsage::IncrRawMemCount();

        memcpy(str, "hello", 6);
        CharT char1(std::move(str));
        EXPECT_EQ(str, nullptr);

        EXPECT_EQ(char1.size(), 6);
        CharT char2(char1);
        char1.SetNull();
        EXPECT_EQ(char2.size(), 6);

        CharT char3(6);
        memcpy(char3.ptr, "a", 2);
        char3.Reset();

        char3 = std::move(char2);
        EXPECT_EQ(char3.size(), 6);
        EXPECT_EQ(char2.ptr, nullptr);

        EXPECT_STREQ(char3.ptr, "hello");
    }
    {
        CharT char1(6);
        memcpy(char1.ptr, "hello", 6);
    }
}


