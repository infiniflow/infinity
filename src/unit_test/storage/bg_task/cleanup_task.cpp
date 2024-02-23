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
import storage;
import cleanup_task;
import infinity_context;

using namespace infinity;

class CleanupTaskTest : public BaseTest {
    void SetUp() override {
        // system("rm -rf /tmp/infinity");
    }

    void TearDown() override {
        // system("tree  /tmp/infinity");
    }
};

TEST_F(CleanupTaskTest, simple_test) {
    Storage *storage = infinity::InfinityContext::instance().storage();
    //
}
