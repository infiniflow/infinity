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
#include "parser/type/complex/varchar.h"

class VarcharTest : public BaseTest {};

TEST_F(VarcharTest, structure_check) {
    using namespace infinity;
    EXPECT_EQ(sizeof(InlineVarchar), 13u);
    EXPECT_EQ(sizeof(VectorVarchar), 13u);
    EXPECT_EQ(sizeof(Varchar), 16u);
}
