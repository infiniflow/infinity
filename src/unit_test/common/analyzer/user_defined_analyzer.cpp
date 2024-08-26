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
import user_defined_analyzer;
import compilation_config;
import status;
import python_instance;

using namespace infinity;

class UserDefinedAnalyzerTest : public BaseTest {};

TEST_F(UserDefinedAnalyzerTest, test1) {
    PythonInstance::Init();

    UserDefinedAnalyzer analyzer(std::string(test_data_path()) + "/scripts/uda1.py");
    Status s1 = analyzer.Init();
    EXPECT_TRUE(s1.ok());
    auto [term_vector, status] = analyzer.Analyze("text1 text2 text3");
    EXPECT_TRUE(status.ok());
    EXPECT_STREQ(term_vector[0].c_str(), "text1");
    EXPECT_STREQ(term_vector[1].c_str(), "text2");
    EXPECT_STREQ(term_vector[2].c_str(), "text3");
    analyzer.UnInit();

    PythonInstance::UnInit();
}



TEST_F(UserDefinedAnalyzerTest, test2) {
    PythonInstance::Init();

    SizeT thread_count = 1;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (SizeT i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread([&] {
            UserDefinedAnalyzer analyzer(std::string(test_data_path()) + "/scripts/uda1.py");
            Status s1 = analyzer.Init();
            EXPECT_TRUE(s1.ok());
            auto [term_vector, status] = analyzer.Analyze("text1 text2 text3");
            EXPECT_TRUE(status.ok());
            EXPECT_STREQ(term_vector[0].c_str(), "text1");
            EXPECT_STREQ(term_vector[1].c_str(), "text2");
            EXPECT_STREQ(term_vector[2].c_str(), "text3");
            analyzer.UnInit();
        }));
    }

    for (SizeT i = 0; i < thread_count; ++i) {
        threads[i].join();
    }

    PythonInstance::UnInit();
}

//TEST_F(UserDefinedAnalyzerTest, test3) {
//    UserDefinedAnalyzer analyzer(std::string(test_data_path()) + "/scripts/uda2.py");
//    Status s1 = analyzer.Init();
//    EXPECT_FALSE(s1.ok());
//    EXPECT_EQ(s1.code(), ErrorCode::kFailToRunPython);
//    analyzer.UnInit();
//}
//
//TEST_F(UserDefinedAnalyzerTest, test4) {
//    UserDefinedAnalyzer analyzer(std::string(test_data_path()) + "/scripts/uda3.py");
//    Status s1 = analyzer.Init();
//    EXPECT_FALSE(s1.ok());
//    EXPECT_EQ(s1.code(), ErrorCode::kFailToRunPython);
//    analyzer.UnInit();
//}

TEST_F(UserDefinedAnalyzerTest, test5) {
    PythonInstance::Init();

    UserDefinedAnalyzer analyzer(std::string(test_data_path()) + "/scripts/uda4.py");
    Status s1 = analyzer.Init();
    EXPECT_TRUE(s1.ok());

    auto [term_vector, status] = analyzer.Analyze("text1 text2 text3");
    EXPECT_EQ(status.code(), ErrorCode::kFailToRunPython);
    analyzer.UnInit();

    PythonInstance::UnInit();
}

