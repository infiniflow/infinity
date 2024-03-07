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

import infinity_exception;

import stl;
import profiler;

class BaseProfilerTest : public BaseTest {};

TEST_F(BaseProfilerTest, test1) {
    infinity::BaseProfiler prof("test1");
    prof.Begin();
    prof.End();
    //    std::cout << prof.Elapsed() << std::endl;
    EXPECT_LT(prof.Elapsed(), 1000);
    EXPECT_NE(prof.ElapsedToString().find("ns"), std::string::npos);

    // Test the time duration larger than 1us
    prof.Begin();
    usleep(1);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000);
    EXPECT_NE(prof.ElapsedToString().find("us"), std::string::npos);

    // Test the time duration larger than 1ms
    prof.Begin();
    usleep(1000);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000 * 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_NE(prof.ElapsedToString().find("ms"), std::string::npos);

    // Test the time duration larger than 1s
    prof.Begin();
    usleep(1000 * 1000);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000 * 1000 * 1000L);
    EXPECT_NE(prof.ElapsedToString().find('s'), std::string::npos);

    //    std::cout << prof.Elapsed() << std::endl;
    //    std::cout << prof.ElapsedToString() << std::endl;
}
