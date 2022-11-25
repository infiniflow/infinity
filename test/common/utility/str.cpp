//
// Created by JinHai on 2022/7/25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/utility/str.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class StrTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(StrTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");
}
