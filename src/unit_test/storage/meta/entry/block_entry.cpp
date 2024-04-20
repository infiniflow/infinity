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
#include <string>

import infinity;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import value;
import default_values;
import block_entry;
import block_version;

class BlockVersionTest : public BaseTest {};

TEST_F(BlockVersionTest, SaveAndLoad) {
    using namespace infinity;
    BlockVersion block_version(8192);
    block_version.created_.emplace_back(10, 3);
    block_version.created_.emplace_back(20, 6);
    block_version.deleted_[2] = 30;
    block_version.deleted_[5] = 40;
    String version_path = String(GetTmpDir()) + "/block_version_test";
    block_version.SaveToFile(version_path);

    BlockVersion block_verson2(8192);
    block_verson2.LoadFromFile(version_path);
    ASSERT_EQ(block_version, block_verson2);
}
