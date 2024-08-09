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
import var_file_worker;
import var_buffer;
import compilation_config;
import third_party;
import buffer_handle;
import infinity_context;
import local_file_system;
import logger;
import config;

using namespace infinity;

class BufferManagerTest : public BaseTest {
private:
    LocalFileSystem fs;

    Vector<SharedPtr<DirEntry>> ListAllFile(const String &path) {
        Vector<SharedPtr<DirEntry>> res;
        std::function<void(const String &)> f = [&](const String &path) {
            auto entries = fs.ListDirectory(path);
            for (auto &entry : entries) {
                if (entry->is_directory()) {
                    f(entry->path());
                } else {
                    res.push_back(entry);
                }
            }
        };
        f(path);
        return res;
    }

protected:
    Vector<SharedPtr<DirEntry>> ListAllData() { return ListAllFile(*data_dir_); }

    Vector<SharedPtr<DirEntry>> ListAllTemp() { return ListAllFile(*temp_dir_); }

    SharedPtr<String> data_dir_;
    SharedPtr<String> temp_dir_;

    void SetUp() override {
        data_dir_ = MakeShared<String>(std::string(tmp_data_path()) + "/buffer/data");
        temp_dir_ = MakeShared<String>(std::string(tmp_data_path()) + "/buffer/temp");
        fs.DeleteDirectory(*data_dir_);
        fs.DeleteDirectory(*temp_dir_);
        fs.CreateDirectory(*data_dir_);
        fs.CreateDirectory(*temp_dir_);
    }

    void TearDown() override {
        fs.DeleteDirectory(*data_dir_);
        fs.DeleteDirectory(*temp_dir_);

        Logger::Shutdown();
    }
};

TEST_F(BufferManagerTest, cleanup_test) {
    const SizeT k = 2;
    const SizeT file_size = 100;
    const SizeT buffer_size = k * file_size;
    const SizeT file_num = 100;
    const SizeT file_num1 = file_num / 2;
    EXPECT_GT(file_num, k + file_num1);

    auto CheckFileNum = [&](SizeT data_num, SizeT temp_num) {
        auto datas = ListAllData();
        auto temps = ListAllTemp();
        EXPECT_EQ(datas.size(), data_num);
        EXPECT_EQ(temps.size(), temp_num);
    };

    {
        BufferManager buffer_mgr(buffer_size, data_dir_, temp_dir_);
        Vector<BufferObj *> buffer_objs;

        for (SizeT i = 0; i < file_num; ++i) {
            auto file_name = MakeShared<String>(fmt::format("file_{}", i));
            auto file_worker = MakeUnique<DataFileWorker>(data_dir_, file_name, file_size);
            auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));
            buffer_objs.push_back(buffer_obj);
            {
                auto buffer_handle = buffer_obj->Load();
                auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
                for (SizeT j = 0; j < file_size; ++j) {
                    data[j] = 'a' + (i + j) % 26;
                }
            }
        }
        CheckFileNum(0, file_num - k);
        {
            SizeT write_n = 0;
            for (auto *buffer_obj : buffer_objs) {
                if (buffer_obj->Save()) {
                    ++write_n;
                }
            }
            EXPECT_EQ(write_n, k);
        }
        CheckFileNum(file_num, 0);
        for (SizeT i = 0; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            auto buffer_handle = buffer_obj->Load();
            const auto *data = reinterpret_cast<const char *>(buffer_handle.GetData());
            for (SizeT j = 0; j < file_size; ++j) {
                EXPECT_EQ(data[j], char('a' + (i + j) % 26));
            }
        }
        CheckFileNum(file_num, 0);
        for (SizeT i = 0; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            auto buffer_handle = buffer_obj->Load();
            auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
            for (SizeT j = 0; j < file_size; ++j) {
                data[j] = 'a' + (i + j) % 26;
            }
        }
        {
            SizeT write_n = 0;
            for (SizeT i = 0; i < file_num1; ++i) {
                auto *buffer_obj = buffer_objs[i];
                bool write = buffer_obj->Save();
                if (write) {
                    ++write_n;
                }
            }
            EXPECT_GE(write_n, 0ull);
            EXPECT_LE(write_n, k);

            buffer_mgr.RemoveClean();
            CheckFileNum(file_num, file_num - k - file_num1);
        }
        for (SizeT i = file_num1; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            {
                auto buffer_handle = buffer_obj->Load();
                auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
                for (SizeT j = 0; j < file_size; ++j) {
                    data[j] = 'A' + (i + j) % 26;
                }
            }
            buffer_obj->Save();
        }
        CheckFileNum(file_num, file_num - file_num1);
        buffer_mgr.RemoveClean();
        CheckFileNum(file_num, 0);

        for (auto *buffer_obj : buffer_objs) {
            buffer_obj->PickForCleanup();
        }
        buffer_mgr.RemoveClean();
        CheckFileNum(0, 0);
    }
}

TEST_F(BufferManagerTest, varfile_test) {
    LocalFileSystem fs;
    SizeT buffer_size = 100;
    SizeT file_num = 10;

    BufferManager buffer_mgr(buffer_size, data_dir_, temp_dir_);
    Vector<BufferObj *> buffer_objs;
    for (SizeT i = 0; i < file_num; ++i) {
        auto file_name = MakeShared<String>(fmt::format("file_{}", i));
        auto file_worker = MakeUnique<VarFileWorker>(data_dir_, file_name, 0);
        auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));
        buffer_objs.push_back(buffer_obj);
    }

    SizeT data_size = 25;
    auto data = MakeUnique<char[]>(data_size);
    for (SizeT i = 0; i < data_size; ++i) {
        data[i] = 'a' + i % 26;
    }
    {
        auto handle1 = buffer_objs[0]->Load();
        auto *buffer1 = reinterpret_cast<VarBuffer *>(handle1.GetDataMut());
        buffer1->Append(data.get(), data_size);

        auto handle2 = buffer_objs[1]->Load();
        auto *buffer2 = reinterpret_cast<VarBuffer *>(handle2.GetDataMut());
        buffer2->Append(data.get(), data_size);

        auto handle3 = buffer_objs[2]->Load();
        auto *buffer3 = reinterpret_cast<VarBuffer *>(handle3.GetDataMut());
        bool free_success = true;
        buffer3->Append(data.get(), data_size, &free_success);
        EXPECT_TRUE(free_success);

        SizeT cur_mem = buffer_mgr.memory_usage();
        EXPECT_EQ(cur_mem, 3 * data_size);
    }
    {
        auto handle1 = buffer_objs[0]->Load();
        auto *buffer1 = reinterpret_cast<VarBuffer *>(handle1.GetDataMut());
        buffer1->Append(data.get(), data_size);

        auto handle2 = buffer_objs[1]->Load();

        auto handle3 = buffer_objs[2]->Load();
        auto *buffer3 = reinterpret_cast<VarBuffer *>(handle3.GetDataMut());
        bool free_success = true;
        buffer3->Append(data.get(), data_size, &free_success);
        EXPECT_FALSE(free_success);

        EXPECT_EQ(buffer_mgr.memory_usage(), 5 * data_size);
    }
    {
        auto handle2 = buffer_objs[1]->Load();
        auto *buffer2 = reinterpret_cast<VarBuffer *>(handle2.GetDataMut());
        buffer2->Append(data.get(), data_size);
    }

    for (int i = 0; i < 2; ++i) {
        auto handle1 = buffer_objs[i]->Load();
        const auto *buffer1 = reinterpret_cast<const VarBuffer *>(handle1.GetData());
        const char *res1 = buffer1->Get(0, data_size);
        EXPECT_EQ(std::string_view(res1, data_size), std::string_view(data.get(), data_size));

        const char *res2 = buffer1->Get(data_size, data_size);
        EXPECT_EQ(std::string_view(res2, data_size), std::string_view(data.get(), data_size));
    }
}

struct FileInfo {
    FileInfo(int file_id) : file_id_(file_id) {}

    FileInfo(FileInfo &&other) {
        file_id_ = std::exchange(other.file_id_, -1);
        buffer_obj_ = std::exchange(other.buffer_obj_, nullptr);
        file_size_ = other.file_size_;
        visit_cnt_ = other.visit_cnt_;
    }

    int file_id_;
    BufferObj *buffer_obj_ = nullptr;
    SizeT file_size_ = 0;
    SizeT visit_cnt_ = 0;
    std::shared_mutex mtx_{};
};

class TestObj {
public:
    virtual ~TestObj() = default;

    virtual void Init(bool alloc_new, FileInfo &file_info) = 0;

    virtual void Write(FileInfo &file_info) = 0;

    virtual void Check(const FileInfo &file_info) = 0;
};

class BufferManagerParallelTest : public BufferManagerTest {
protected:
    void SetUp() override {
        BufferManagerTest::SetUp();
        buffer_mgr_ = MakeUnique<BufferManager>(buffer_size, data_dir_, temp_dir_);
        finished_n_ = 0;
    }

    void TearDown() override {
        EXPECT_EQ(buffer_mgr_->memory_usage(), 0ul);
        buffer_mgr_.reset();
        BufferManagerTest::TearDown();
    }

    const SizeT thread_n = 2;
    const SizeT file_n = 100;
    const SizeT avg_file_size = 100;
    const SizeT max_file_size = avg_file_size + avg_file_size / 2;
    const SizeT buffer_size = max_file_size * thread_n;
    const SizeT loop_n = 10;
    const SizeT test_n_ = 2;
    const SizeT var_file_step = max_file_size / loop_n / test_n_;

    UniquePtr<BufferManager> buffer_mgr_;
    Atomic<SizeT> finished_n_ = 0;

    void TestRoutine(Vector<FileInfo> &file_infos, SizeT test_i, SizeT thread_i, TestObj *test_obj) {
        bool alloc_new = test_i == 0;
        bool clean = test_i == test_n_ - 1;
        while (finished_n_.load() < file_n) {
            int op = rand() % 5;
            if (op > 2) {
                op = 2; // read
            }

            int file_id = rand() % file_n;
            auto &file_info = file_infos[file_id];
            if (op < 2) { // write
                std::unique_lock lck(file_info.mtx_);
                SizeT &visit_cnt = file_info.visit_cnt_;
                BufferObj *&buffer_obj = file_info.buffer_obj_;
                if (visit_cnt == test_i * loop_n) {
                    EXPECT_EQ(buffer_obj, nullptr);
                    test_obj->Init(alloc_new, file_info);
                } else if (visit_cnt == (test_i + 1) * loop_n) {
                    if (clean) {
                        EXPECT_EQ(buffer_obj, nullptr);
                    }
                    continue;
                }
                test_obj->Write(file_info);
                if (op == 1) {
                    buffer_obj->Save();
                }
                ++visit_cnt;
                if (visit_cnt == (test_i + 1) * loop_n) {
                    if (clean) {
                        buffer_obj->PickForCleanup();
                        buffer_obj = nullptr;
                    }
                    finished_n_.fetch_add(1);
                }
            } else {
                std::shared_lock lck(file_info.mtx_);
                SizeT visit_cnt = file_info.visit_cnt_;
                if (visit_cnt == test_i * loop_n) {
                    continue;
                } else if (visit_cnt == (test_i + 1) * loop_n) {
                    continue;
                }
                test_obj->Check(file_info);
            }
        }
        LOG_INFO(fmt::format("Test {} thread {} finished", test_i, thread_i));
    }
};

class Test1Obj : public TestObj {
public:
    Test1Obj(SizeT avg_file_size, BufferManager *buffer_mgr, SharedPtr<String> data_dir)
        : avg_file_size(avg_file_size), buffer_mgr_(buffer_mgr), data_dir_(data_dir) {}

private:
    const SizeT avg_file_size;
    BufferManager *buffer_mgr_;
    SharedPtr<String> data_dir_;

public:
    void Init(bool alloc_new, FileInfo &file_info) override {
        auto file_name = MakeShared<String>(fmt::format("file_{}", file_info.file_id_));
        if (alloc_new) {
            SizeT file_size = rand() % avg_file_size + avg_file_size / 2;
            file_info.file_size_ = file_size;
            auto file_worker = MakeUnique<DataFileWorker>(data_dir_, file_name, file_size);
            file_info.buffer_obj_ = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
        } else {
            auto file_worker = MakeUnique<DataFileWorker>(data_dir_, file_name, file_info.file_size_);
            file_info.buffer_obj_ = buffer_mgr_->GetBufferObject(std::move(file_worker));
        }
    }

    void Write(FileInfo &file_info) override {
        SizeT visit_cnt = file_info.visit_cnt_;
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
        for (SizeT i = 0; i < file_info.file_size_; ++i) {
            data[i] = 'a' + (visit_cnt % 26);
        }
    }

    void Check(const FileInfo &file_info) override {
        SizeT visit_cnt = file_info.visit_cnt_;
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        const auto *data = reinterpret_cast<const char *>(buffer_handle.GetData());
        for (SizeT i = 0; i < file_info.file_size_; ++i) {
            EXPECT_EQ(data[i], char('a' + (visit_cnt - 1) % 26));
        }
    }
};

TEST_F(BufferManagerParallelTest, parallel_test1) {
    auto test1_obj = MakeUnique<Test1Obj>(avg_file_size, buffer_mgr_.get(), data_dir_);
    LocalFileSystem fs;

    Vector<FileInfo> file_infos;
    for (SizeT i = 0; i < file_n; ++i) {
        file_infos.emplace_back(i);
    }
    LOG_INFO("Start parallel test1");
    for (SizeT test_i = 0; test_i < test_n_; test_i++) {
        finished_n_.store(0);
        for (auto &file_info : file_infos) {
            file_info.buffer_obj_ = nullptr;
        }

        Vector<Thread> threads;
        for (SizeT thread_i = 0; thread_i < thread_n; ++thread_i) {
            threads.emplace_back([&, thread_i]() { TestRoutine(file_infos, test_i, thread_i, test1_obj.get()); });
        }
        for (auto &thread : threads) {
            thread.join();
        }
    }
    LOG_INFO("Finished parallel test1");
}

class Test2Obj : public TestObj {
public:
    Test2Obj(SizeT var_file_step, BufferManager *buffer_mgr, SharedPtr<String> data_dir)
        : var_file_step(var_file_step), buffer_mgr_(buffer_mgr), data_dir_(data_dir) {}

private:
    const SizeT var_file_step;
    BufferManager *buffer_mgr_;
    SharedPtr<String> data_dir_;

public:
    void Init(bool alloc_new, FileInfo &file_info) override {
        auto file_name = MakeShared<String>(fmt::format("file_{}", file_info.file_id_));
        if (alloc_new) {
            auto file_worker = MakeUnique<VarFileWorker>(data_dir_, file_name, 0);
            file_info.buffer_obj_ = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
        } else {
            auto file_worker = MakeUnique<VarFileWorker>(data_dir_, file_name, file_info.file_size_);
            file_info.buffer_obj_ = buffer_mgr_->GetBufferObject(std::move(file_worker));
        }
    }

    void Write(FileInfo &file_info) override {
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        auto *buffer = reinterpret_cast<VarBuffer *>(buffer_handle.GetDataMut());
        auto data = MakeUnique<char[]>(var_file_step);
        for (SizeT i = 0; i < var_file_step; ++i) {
            data[i] = 'a' + (i + file_info.file_size_) % 26;
        }
        buffer->Append(data.get(), var_file_step);
        file_info.file_size_ += var_file_step;
    }

    void Check(const FileInfo &file_info) override {
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        const auto *buffer = reinterpret_cast<const VarBuffer *>(buffer_handle.GetData());
        for (SizeT i = 0; i < file_info.file_size_; i += var_file_step) {
            const char *data = buffer->Get(i, var_file_step);
            for (SizeT j = 0; j < var_file_step; ++j) {
                EXPECT_EQ(data[j], char('a' + (i + j) % 26));
            }
        }
    }
};

TEST_F(BufferManagerParallelTest, parallel_test2) {
    auto test2_obj = MakeUnique<Test2Obj>(var_file_step, buffer_mgr_.get(), data_dir_);
    LocalFileSystem fs;

    Vector<FileInfo> file_infos;
    for (SizeT i = 0; i < file_n; ++i) {
        file_infos.emplace_back(i);
    }
    LOG_INFO("Start parallel test2");
    for (SizeT test_i = 0; test_i < test_n_; test_i++) {
        finished_n_.store(0);
        for (auto &file_info : file_infos) {
            file_info.buffer_obj_ = nullptr;
        }

        Vector<Thread> threads;
        for (SizeT thread_i = 0; thread_i < thread_n; ++thread_i) {
            threads.emplace_back([&, thread_i]() { TestRoutine(file_infos, test_i, thread_i, test2_obj.get()); });
        }
        for (auto &thread : threads) {
            thread.join();
        }
    }
}
