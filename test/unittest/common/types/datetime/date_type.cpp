//
// Created by jinhai on 23-3-10.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/data_type.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class DateTypeTest : public BaseTest {
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

TEST_F(DateTypeTest, test1) {
    using namespace infinity;

    DateType date1;

    EXPECT_EQ(date1.ToString(), "1970-01-01");

    date1.FromString("2000-01-01");
    EXPECT_EQ(date1.ToString(), "2000-01-01");

    date1.FromString("2000/10/31");
    EXPECT_EQ(date1.ToString(), "2000-10-31");

    date1.FromString("1-1-1");
    EXPECT_EQ(date1.ToString(), "1-01-01");

    date1.FromString("9999-12-31");
    EXPECT_EQ(date1.ToString(), "9999-12-31");

    EXPECT_THROW(date1.FromString("0-0-0"), ExecutorException);
    EXPECT_THROW(date1.FromString("0/0/0"), ExecutorException);
}