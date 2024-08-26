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
import user_defined_term_weight;
import compilation_config;
import status;
import python_instance;

using namespace infinity;

class UserDefinedTermWeightTest : public BaseTest {};

TEST_F(UserDefinedTermWeightTest, test1) {
    PythonInstance::Init();

    UserDefinedTermWeight term_weight(std::string(test_data_path()) + "/scripts/uda5.py");
    Status s1 = term_weight.Init();
    EXPECT_TRUE(s1.ok());
    Vector<String> vec_str;
    vec_str.push_back("text1");
    vec_str.push_back("text2");
    vec_str.push_back("text3");
    auto [term_map, status] = term_weight.Run(vec_str);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(term_map["a"], 1.0);
    EXPECT_EQ(term_map["b"], 2.0);
    EXPECT_EQ(term_map["c"], 3.0);
    term_weight.UnInit();

    PythonInstance::UnInit();
}

TEST_F(UserDefinedTermWeightTest, test2) {
    PythonInstance::Init();

    SizeT thread_count = 4;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (SizeT i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread([&] {
            UserDefinedTermWeight term_weight(std::string(test_data_path()) + "/scripts/uda5.py");
            Status s1 = term_weight.Init();
            EXPECT_TRUE(s1.ok());
            Vector<String> vec_str;
            vec_str.push_back("text1");
            vec_str.push_back("text2");
            vec_str.push_back("text3");
            auto [term_map, status] = term_weight.Run(vec_str);
            EXPECT_TRUE(status.ok());
            EXPECT_EQ(term_map["a"], 1.0);
            EXPECT_EQ(term_map["b"], 2.0);
            EXPECT_EQ(term_map["c"], 3.0);
            term_weight.UnInit();
        }));
    }

    for (SizeT i = 0; i < thread_count; ++i) {
        threads[i].join();
    }

    PythonInstance::UnInit();
}

//TEST_F(UserDefinedTermWeightTest, test3) {
//    UserDefinedTermWeight term_weight(std::string(test_data_path()) + "/scripts/uda2.py");
//    Status s1 = term_weight.Init();
//    EXPECT_FALSE(s1.ok());
//    EXPECT_EQ(s1.code(), ErrorCode::kFailToRunPython);
//    term_weight.UnInit();
//}
//
//TEST_F(UserDefinedTermWeightTest, test4) {
//    UserDefinedTermWeight term_weight(std::string(test_data_path()) + "/scripts/uda3.py");
//    Status s1 = term_weight.Init();
//    EXPECT_FALSE(s1.ok());
//    EXPECT_EQ(s1.code(), ErrorCode::kFailToRunPython);
//    term_weight.UnInit();
//}

TEST_F(UserDefinedTermWeightTest, test5) {
    PythonInstance::Init();

    UserDefinedTermWeight term_weight(std::string(test_data_path()) + "/scripts/uda6.py");
    Status s1 = term_weight.Init();
    EXPECT_TRUE(s1.ok());

    Vector<String> vec_str;
    vec_str.push_back("text1");
    vec_str.push_back("text2");
    vec_str.push_back("text3");
    auto [term_map, status] = term_weight.Run(vec_str);
    EXPECT_EQ(status.code(), ErrorCode::kFailToRunPython);
    term_weight.UnInit();

    PythonInstance::UnInit();
}
