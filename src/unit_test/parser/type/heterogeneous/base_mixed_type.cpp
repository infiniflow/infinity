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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import internal_types;

using namespace infinity;
class BaseMixedTypeTest : public BaseTest {};

TEST_F(BaseMixedTypeTest, GetTypeName) {
    using namespace infinity;

    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kInvalid).c_str(), "Invalid");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kInteger).c_str(), "Integer");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kLongStr).c_str(), "Long String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kShortStr).c_str(), "Short String");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kTuple).c_str(), "Tuple");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kArray).c_str(), "Array");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kNull).c_str(), "Null");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kMissing).c_str(), "Missing");
    EXPECT_STREQ(BaseMixedType::GetTypeName(MixedValueType::kDummy).c_str(), "Dummy");
}
