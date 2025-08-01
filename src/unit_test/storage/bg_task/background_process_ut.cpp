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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.background_process;

import :ut.base_test;
import :infinity_context;
import :infinity_exception;
import :stl;
import :third_party;
import :logger;
import :table_def;
import :value;
import :data_block;
import :default_values;
import :status;
import :background_process;
import :bg_task;
#endif

import global_resource_usage;

using namespace infinity;

class BGProcessTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, BGProcessTest, ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));

TEST_P(BGProcessTest, test1) {
    using namespace infinity;

    BGTaskProcessor processor;

    processor.Start();

    processor.Stop();
}
