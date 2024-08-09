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

import logger;
import heap_chunk;
import third_party;
import stl;
import global_resource_usage;
import infinity_context;
import default_values;

class StringChunkTest : public BaseTestParamStr {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        system(("mkdir -p " + std::string(GetFullPersistDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        std::string config_path_str = GetParam();
        std::shared_ptr<std::string> config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTestParamStr::TearDown();
    }
};
INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         StringChunkTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_CONFIG_PATH));

//
//
//TEST_P(StringChunkTest, heap_chunk_test) {
//    using namespace infinity;
//
//    {
//        HeapChunk heap_chunk(0);
//        EXPECT_EQ(heap_chunk.ptr_, nullptr);
//    }
//
//    {
//        HeapChunk heap_chunk(MIN_VECTOR_CHUNK_SIZE);
//        EXPECT_NE(heap_chunk.ptr_, nullptr);
//    }
//}

TEST_P(StringChunkTest, string_chunk_a) {
    using namespace infinity;

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), UnrecoverableException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26u);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192u);
}
