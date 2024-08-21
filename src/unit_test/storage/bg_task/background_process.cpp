// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unit_test/base_test.h"

import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import value;

import data_block;
import default_values;
import txn_manager;
import txn;
import status;
import background_process;
import bg_task;

class BGProcessTest : public BaseTestParamStr {
    void SetUp() override {
        RemoveDbDirs();
        infinity::GlobalResourceUsage::Init();
        system(("mkdir -p " + std::string(GetFullPersistDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        std::string config_path_str = GetParam();
        std::shared_ptr<std::string> config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        BaseTestParamStr::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BGProcessTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(BGProcessTest, test1) {
    using namespace infinity;

    BGTaskProcessor processor(infinity::InfinityContext::instance().storage()->wal_manager(), nullptr);

    processor.Start();

    processor.Stop();
}
