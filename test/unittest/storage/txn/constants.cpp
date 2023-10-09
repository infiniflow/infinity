//
// Created by jinhai on 23-6-2.
//

#include "base_test.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/profiler/base_profiler.h"
#include "main/stats/global_resource_usage.h"
#include "storage/data_block.h"
#include "storage/txn/txn_state.h"
#include <gtest/gtest.h>

class TxnContants : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(TxnContants, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    EXPECT_EQ(ToString(TxnState::kStarted), "Started");
}