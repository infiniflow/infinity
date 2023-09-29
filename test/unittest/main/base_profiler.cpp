//
// Created by JinHai on 2022/9/6.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class BaseProfilerTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(BaseProfilerTest, test1) {
    infinity::BaseProfiler profiler;
    profiler.Begin();
    profiler.End();
//    std::cout << profiler.Elapsed() << std::endl;
    EXPECT_LT(profiler.Elapsed(), 1000);
    EXPECT_NE(profiler.ElapsedToString().find("ns"), std::string::npos);

    // Test the time duration larger than 1us
    profiler.Begin();
    usleep(1);
    profiler.End();
    EXPECT_GT(profiler.Elapsed(), 1000);
    EXPECT_LT(profiler.Elapsed(), 1000 * 1000);
    EXPECT_NE(profiler.ElapsedToString().find("us"), std::string::npos);

    // Test the time duration larger than 1ms
    profiler.Begin();
    usleep(1000);
    profiler.End();
    EXPECT_GT(profiler.Elapsed(), 1000 * 1000);
    EXPECT_LT(profiler.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_NE(profiler.ElapsedToString().find("ms"), std::string::npos);

    // Test the time duration larger than 1s
    profiler.Begin();
    usleep(1000 * 1000);
    profiler.End();
    EXPECT_GT(profiler.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_LT(profiler.Elapsed(), 1000 * 1000 * 1000 * 1000L);
    EXPECT_NE(profiler.ElapsedToString().find('s'), std::string::npos);

//    std::cout << profiler.Elapsed() << std::endl;
//    std::cout << profiler.ElapsedToString() << std::endl;
}