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
import buffer_manager;
import buffer_obj;
import data_file_worker;
import compilation_config;
import third_party;
import buffer_handle;
import infinity_context;
import local_file_system;
import logger;
import config;

using namespace infinity;

class BufferParallelTest : public BaseTest {
protected:
    SharedPtr<String> data_dir_;
    SharedPtr<String> temp_dir_;

    void SetUp() override {
        Config config;
        config.Init(nullptr);
        Logger::Initialize(&config);

        data_dir_ = MakeShared<String>(std::string(tmp_data_path()) + "/buffer/data");
        temp_dir_ = MakeShared<String>(std::string(tmp_data_path()) + "/buffer/temp");
        LocalFileSystem fs;
        fs.CreateDirectory(*data_dir_);
        fs.CreateDirectory(*temp_dir_);
    }

    void TearDown() override {
        LocalFileSystem fs;
        fs.DeleteDirectory(*data_dir_);
        fs.DeleteDirectory(*temp_dir_);

        Logger::Shutdown();
    }

    struct FileInfo {
        BufferObj *buffer_obj_ = nullptr;
        SizeT file_size_ = 0;
        SizeT visit_cnt_ = 0;
        std::shared_mutex mtx_{};
    };
};

TEST_F(BufferParallelTest, test1) {
    const SizeT thread_n = 4;
    const SizeT file_n = 100;
    const SizeT avg_file_size = 100;
    const SizeT buffer_size = std::max(avg_file_size * file_n / 4, avg_file_size * thread_n); // avg?
    const SizeT loop_n = 100;

    ASSERT_GT(loop_n, 0ull);

    BufferManager buffer_mgr(buffer_size, data_dir_, temp_dir_);

    Vector<Thread> threads;
    Atomic<SizeT> finished_n = 0;

    auto test_routine = [&](Vector<FileInfo> &file_infos, bool alloc_new) {
        while (finished_n.load() < file_n) {
            int op = rand() % 4;
            if (op != 0) {
                op = 1; // read
            }

            int file_id = rand() % file_n;
            auto &file_info = file_infos[file_id];
            if (op == 0) { // write
                std::unique_lock lck(file_info.mtx_);
                SizeT &visit_cnt = file_info.visit_cnt_;
                BufferObj *&buffer_obj = file_info.buffer_obj_;
                if (visit_cnt == 0) {
                    EXPECT_EQ(buffer_obj, nullptr);
                    SizeT file_size = rand() % avg_file_size + avg_file_size / 2;
                    auto file_name = MakeShared<String>(fmt::format("file_{}", file_id));
                    auto file_worker = MakeUnique<DataFileWorker>(data_dir_, file_name, file_size);
                    if (alloc_new) {
                        buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));
                    } else {
                        buffer_obj = buffer_mgr.GetBufferObject(std::move(file_worker));
                    }
                } else if (visit_cnt == loop_n) {
                    EXPECT_EQ(buffer_obj, nullptr);
                    continue;
                }
                {
                    BufferHandle buffer_handle = buffer_obj->Load();
                    auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
                    for (SizeT i = 0; i < file_info.file_size_; ++i) {
                        data[i] = 'a' + (visit_cnt % 26);
                    }
                    ++visit_cnt;
                }
                if (visit_cnt == loop_n) {
                    buffer_obj->PickForCleanup();
                    buffer_obj = nullptr;
                    finished_n.fetch_add(1);
                }
            } else {
                std::shared_lock lck(file_info.mtx_);
                SizeT visit_cnt = file_info.visit_cnt_;
                BufferObj *buffer_obj = file_info.buffer_obj_;
                if (visit_cnt == 0) {
                    continue;
                } else if (visit_cnt == loop_n) {
                    continue;
                }
                {
                    BufferHandle buffer_handle = buffer_obj->Load();
                    const auto *data = reinterpret_cast<const char *>(buffer_handle.GetData());
                    for (SizeT i = 0; i < file_info.file_size_; ++i) {
                        EXPECT_EQ(data[i], char('a' + visit_cnt % 26));
                    }
                }
            }
        }
    };

    for (int j = 0; j < 1; j++) {
        bool alloc_new = j == 0;
        Vector<FileInfo> file_infos(file_n);
        for (SizeT i = 0; i < thread_n; ++i) {
            threads.emplace_back([&]() { test_routine(file_infos, alloc_new); });
        }
        for (auto &thread : threads) {
            thread.join();
        }
    }
}