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

import stl;
import var_buffer;
import infinity_exception;

using namespace infinity;

class VarBufferTest : public BaseTest {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(VarBufferTest, test1) {
    VarBuffer var_buffer;
    auto data = MakeUnique<char[]>(26);
    for (int i = 0; i < 26; ++i) {
        data[i] = 'a' + i;
    }
    SizeT offset1 = var_buffer.Append(data.get(), 26);
    EXPECT_EQ(offset1, 0);

    SizeT offset2 = var_buffer.Append(std::move(data), 26);
    EXPECT_EQ(offset2, 26);

    auto test = [&](const VarBuffer &var_buffer) {
        const auto *res1 = var_buffer.Get(10, 10);
        EXPECT_EQ(std::string_view(res1, 10), "klmnopqrst");

        const auto *res2 = var_buffer.Get(36, 10);
        EXPECT_EQ(std::string_view(res2, 10), "klmnopqrst");

        try {
            [[maybe_unused]] const auto *res3 = var_buffer.Get(10, 26);
            FAIL();
        } catch (UnrecoverableException &e) {
        }

        try {
            [[maybe_unused]] const auto *res4 = var_buffer.Get(36, 10);
            FAIL();
        } catch (UnrecoverableException &e) {
        }
    };

    test(var_buffer);
    auto [size, res4] = var_buffer.Finish();
    VarBuffer var_buffer2;
    var_buffer2.Append(std::move(res4), size);
    test(var_buffer2);
}
