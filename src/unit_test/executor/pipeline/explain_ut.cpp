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

module infinity_core:ut.explain;

import :ut.base_test;
import :stl;
import :physical_explain;
#endif

using namespace infinity;
class ExplainTest : public BaseTest {};

TEST_F(ExplainTest, test1) {
    using namespace infinity;

    Vector<std::shared_ptr<String>> fragment_text;
    Vector<std::shared_ptr<String>> task_text;

    fragment_text.push_back(std::make_shared<String>("FRAGMENT (1)"));
    fragment_text.push_back(std::make_shared<String>("-> SINK (7)"));
    fragment_text.push_back(std::make_shared<String>("-> PROJECT (4)"));
    fragment_text.push_back(std::make_shared<String>("   - table index: #4"));
    fragment_text.push_back(std::make_shared<String>("   - expressions: [c1 (#0)]"));
    fragment_text.push_back(std::make_shared<String>("-> AGGREGATE (3)"));
    fragment_text.push_back(std::make_shared<String>("   - aggregate table index: #3"));
    fragment_text.push_back(std::make_shared<String>("   - aggregate: []"));
    fragment_text.push_back(std::make_shared<String>("   - group by table index: #2"));
    fragment_text.push_back(std::make_shared<String>("   - group by: [c1 (#0)]"));
    fragment_text.push_back(std::make_shared<String>("-> SOURCE (11): FRAGMENT #2"));
    fragment_text.push_back(std::make_shared<String>());

    fragment_text.push_back(std::make_shared<String>("FRAGMENT (2)"));
    fragment_text.push_back(std::make_shared<String>("-> SINK (9)"));
    fragment_text.push_back(std::make_shared<String>("-> TABLE SCAN (2)"));
    fragment_text.push_back(std::make_shared<String>("   - table name: t1(default_db.t1)"));
    fragment_text.push_back(std::make_shared<String>("   - table index: #1"));
    fragment_text.push_back(std::make_shared<String>("   - output_columns: [c1]"));
    fragment_text.push_back(std::make_shared<String>("-> SOURCE (10)"));
    fragment_text.push_back(std::make_shared<String>());

    task_text.push_back(std::make_shared<String>("Fragment #1 * 4 Task"));
    task_text.push_back(std::make_shared<String>("-> Task #0"));
    task_text.push_back(std::make_shared<String>("-> Task #1"));
    task_text.push_back(std::make_shared<String>("-> Task #2"));
    task_text.push_back(std::make_shared<String>("-> Task #3"));
    task_text.push_back(std::make_shared<String>());

    task_text.push_back(std::make_shared<String>("Fragment #2 * 2 Task"));
    task_text.push_back(std::make_shared<String>("-> Task #0"));
    task_text.push_back(std::make_shared<String>("-> Task #1"));
    task_text.push_back(std::make_shared<String>());

    PhysicalExplain::AlignParagraphs(fragment_text, task_text);

    EXPECT_EQ(fragment_text.size(), task_text.size());
}
