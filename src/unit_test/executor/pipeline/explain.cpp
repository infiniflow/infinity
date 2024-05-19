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
import physical_explain;

class ExplainTest : public BaseTest {};

TEST_F(ExplainTest, test1) {
    using namespace infinity;

    Vector<SharedPtr<String>> fragment_text;
    Vector<SharedPtr<String>> task_text;

    fragment_text.push_back(MakeShared<String>("FRAGMENT (1)"));
    fragment_text.push_back(MakeShared<String>("-> SINK (7)"));
    fragment_text.push_back(MakeShared<String>("-> PROJECT (4)"));
    fragment_text.push_back(MakeShared<String>("   - table index: #4"));
    fragment_text.push_back(MakeShared<String>("   - expressions: [c1 (#0)]"));
    fragment_text.push_back(MakeShared<String>("-> AGGREGATE (3)"));
    fragment_text.push_back(MakeShared<String>("   - aggregate table index: #3"));
    fragment_text.push_back(MakeShared<String>("   - aggregate: []"));
    fragment_text.push_back(MakeShared<String>("   - group by table index: #2"));
    fragment_text.push_back(MakeShared<String>("   - group by: [c1 (#0)]"));
    fragment_text.push_back(MakeShared<String>("-> SOURCE (11): FRAGMENT #2"));
    fragment_text.push_back(MakeShared<String>());

    fragment_text.push_back(MakeShared<String>("FRAGMENT (2)"));
    fragment_text.push_back(MakeShared<String>("-> SINK (9)"));
    fragment_text.push_back(MakeShared<String>("-> TABLE SCAN (2)"));
    fragment_text.push_back(MakeShared<String>("   - table name: t1(default_db.t1)"));
    fragment_text.push_back(MakeShared<String>("   - table index: #1"));
    fragment_text.push_back(MakeShared<String>("   - output_columns: [c1]"));
    fragment_text.push_back(MakeShared<String>("-> SOURCE (10)"));
    fragment_text.push_back(MakeShared<String>());

    task_text.push_back(MakeShared<String>("Fragment #1 * 4 Task"));
    task_text.push_back(MakeShared<String>("-> Task #0"));
    task_text.push_back(MakeShared<String>("-> Task #1"));
    task_text.push_back(MakeShared<String>("-> Task #2"));
    task_text.push_back(MakeShared<String>("-> Task #3"));
    task_text.push_back(MakeShared<String>());

    task_text.push_back(MakeShared<String>("Fragment #2 * 2 Task"));
    task_text.push_back(MakeShared<String>("-> Task #0"));
    task_text.push_back(MakeShared<String>("-> Task #1"));
    task_text.push_back(MakeShared<String>());

    PhysicalExplain::AlignParagraphs(fragment_text, task_text);

    EXPECT_EQ(fragment_text.size(), task_text.size());
}