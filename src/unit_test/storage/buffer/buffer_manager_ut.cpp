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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.buffer_manager;

import :ut.base_test;

import :buffer_manager;
import :buffer_obj;
import :data_file_worker;
import :var_file_worker;
import :var_buffer;
import third_party;
import :buffer_handle;
import :infinity_context;
import :virtual_store;
import :logger;
import :config;
import :infinity_exception;
import :persistence_manager;
import :default_values;
import :kv_store;

import compilation_config;

using namespace infinity;

class BufferManagerTest : public BaseTest {
private:
    std::vector<std::shared_ptr<std::filesystem::directory_entry>> ListAllFile(const std::string &path) {
        std::vector<std::shared_ptr<std::filesystem::directory_entry>> res;
        std::function<void(const std::string &)> f = [&](const std::string &path) {
            auto [entries, status] = VirtualStore::ListDirectory(path);
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
    std::vector<std::shared_ptr<std::filesystem::directory_entry>> ListAllData() { return ListAllFile(*data_dir_); }

    std::vector<std::shared_ptr<std::filesystem::directory_entry>> ListAllTemp() { return ListAllFile(*temp_dir_); }

    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;
    std::shared_ptr<std::string> persistence_dir_;

    void SetUp() override {
        Config config;
        config.Init(nullptr, nullptr);
        // config.SetLogToStdout(true);

        Logger::Initialize(&config);

        data_dir_ = std::make_shared<std::string>(std::string(tmp_data_path()) + "/buffer/data");
        temp_dir_ = std::make_shared<std::string>(std::string(tmp_data_path()) + "/buffer/temp");
        persistence_dir_ = std::make_shared<std::string>(std::string(tmp_data_path()) + "/buffer/persistence");
        ResetDir();
    }

    void ResetDir() {
        VirtualStore::RemoveDirectory(*data_dir_);
        VirtualStore::RemoveDirectory(*temp_dir_);
        VirtualStore::RemoveDirectory(*persistence_dir_);
        VirtualStore::MakeDirectory(*data_dir_);
        VirtualStore::MakeDirectory(*temp_dir_);
        VirtualStore::MakeDirectory(*persistence_dir_);
    }

    void TearDown() override {
        VirtualStore::RemoveDirectory(*data_dir_);
        VirtualStore::RemoveDirectory(*temp_dir_);
        VirtualStore::RemoveDirectory(*persistence_dir_);

        Logger::Shutdown();
    }
};

TEST_F(BufferManagerTest, cleanup_test) {
    const size_t k = 2;
    const size_t file_size = 100;
    const size_t buffer_size = k * file_size;
    const size_t file_num = 100;
    const size_t file_num1 = file_num / 2;
    EXPECT_GT(file_num, k + file_num1);

    auto CheckFileNum = [&](size_t data_num, size_t temp_num) {
        auto datas = ListAllData();
        auto temps = ListAllTemp();
        EXPECT_EQ(datas.size(), data_num);
        EXPECT_EQ(temps.size(), temp_num);
    };

    {
        BufferManager buffer_mgr(buffer_size, data_dir_, temp_dir_, nullptr);
        std::vector<BufferObj *> buffer_objs;

        for (size_t i = 0; i < file_num; ++i) {
            auto file_name = std::make_shared<std::string>(fmt::format("file_{}", i));
            auto file_worker = std::make_unique<DataFileWorker>(data_dir_,
                                                                temp_dir_,
                                                                std::make_shared<std::string>(""),
                                                                file_name,
                                                                file_size,
                                                                buffer_mgr.persistence_manager());
            BufferObj *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));
            buffer_obj->AddObjRc();
            buffer_objs.push_back(buffer_obj);
            {
                auto buffer_handle = buffer_obj->Load();
                auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
                for (size_t j = 0; j < file_size; ++j) {
                    data[j] = 'a' + (i + j) % 26;
                }
                buffer_obj->SetDataSize(file_size);
            }
        }
        CheckFileNum(0, file_num - k);
        {
            size_t write_n = 0;
            for (auto *buffer_obj : buffer_objs) {
                if (buffer_obj->Save()) {
                    ++write_n;
                }
            }
            EXPECT_EQ(write_n, k);
        }
        CheckFileNum(file_num, 0);
        for (size_t i = 0; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            auto buffer_handle = buffer_obj->Load();
            const auto *data = reinterpret_cast<const char *>(buffer_handle.GetData());
            for (size_t j = 0; j < file_size; ++j) {
                EXPECT_EQ(data[j], char('a' + (i + j) % 26));
            }
        }
        CheckFileNum(file_num, 0);
        for (size_t i = 0; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            auto buffer_handle = buffer_obj->Load();
            auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
            for (size_t j = 0; j < file_size; ++j) {
                data[j] = 'a' + (i + j) % 26;
            }
        }
        {
            size_t write_n = 0;
            for (size_t i = 0; i < file_num1; ++i) {
                auto *buffer_obj = buffer_objs[i];
                bool write = buffer_obj->Save();
                if (write) {
                    ++write_n;
                }
            }
            EXPECT_GE(write_n, 0ull);
            EXPECT_LE(write_n, k);

            buffer_mgr.RemoveClean(nullptr);
            CheckFileNum(file_num, file_num - k - file_num1);
        }
        for (size_t i = file_num1; i < file_num; ++i) {
            auto *buffer_obj = buffer_objs[i];
            {
                auto buffer_handle = buffer_obj->Load();
                auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
                for (size_t j = 0; j < file_size; ++j) {
                    data[j] = 'A' + (i + j) % 26;
                }
            }
            buffer_obj->Save();
        }
        CheckFileNum(file_num, file_num - file_num1);
        buffer_mgr.RemoveClean(nullptr);
        CheckFileNum(file_num, 0);

        for (auto *buffer_obj : buffer_objs) {
            buffer_obj->PickForCleanup();
        }
        buffer_mgr.RemoveClean(nullptr);
        CheckFileNum(0, 0);
    }
}

TEST_F(BufferManagerTest, varfile_test) {
    size_t buffer_size = 100;
    size_t file_num = 10;

    std::shared_ptr<PersistenceManager> persistence_manager_ =
        std::make_shared<PersistenceManager>(*persistence_dir_, *data_dir_, DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT);
    BufferManager buffer_mgr(buffer_size, data_dir_, temp_dir_, persistence_manager_.get());
    std::vector<BufferObj *> buffer_objs;
    for (size_t i = 0; i < file_num; ++i) {
        auto file_name = std::make_shared<std::string>(fmt::format("file_{}", i));
        auto file_worker =
            std::make_unique<VarFileWorker>(data_dir_, temp_dir_, std::make_shared<std::string>(), file_name, 0, buffer_mgr.persistence_manager());
        auto *buffer_obj = buffer_mgr.AllocateBufferObject(std::move(file_worker));
        buffer_objs.push_back(buffer_obj);
    }

    size_t data_size = 25;
    auto data = std::make_unique<char[]>(data_size);
    for (size_t i = 0; i < data_size; ++i) {
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

        size_t cur_mem = buffer_mgr.memory_usage();
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
    size_t file_size_ = 0;
    size_t visit_cnt_ = 0;
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
    void SetUp() override { BufferManagerTest::SetUp(); }

    void TearDown() override { BufferManagerTest::TearDown(); }

    const size_t thread_n = 2;
    const size_t file_n = 100;
    const size_t avg_file_size = 100;
    const size_t max_file_size = avg_file_size + avg_file_size / 2;
    // *2 because BufferManager::RequestSpace may scan buffer obj that is loading/cleaning
    const size_t buffer_size = max_file_size * thread_n * 2;
    const size_t loop_n = 10;
    const size_t test_n_ = 2;
    const size_t var_file_step = max_file_size / loop_n / test_n_;

    void TestRoutine(std::vector<FileInfo> &file_infos, size_t test_i, size_t thread_i, TestObj *test_obj, std::atomic<size_t> &finished_n) {
        bool alloc_new = test_i == 0;
        bool clean = test_i == test_n_ - 1;
        while (finished_n.load() < file_n) {
            int op = rand() % 5;
            if (op > 2) {
                op = 2; // read
            }

            int file_id = rand() % file_n;
            auto &file_info = file_infos[file_id];
            if (op < 2) { // write
                std::unique_lock lck(file_info.mtx_);
                size_t &visit_cnt = file_info.visit_cnt_;
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
                    finished_n.fetch_add(1);
                }
            } else {
                std::shared_lock lck(file_info.mtx_);
                size_t visit_cnt = file_info.visit_cnt_;
                if (visit_cnt == test_i * loop_n) {
                    continue;
                } else if (visit_cnt == (test_i + 1) * loop_n) {
                    continue;
                }
                test_obj->Check(file_info);
            }
        }
        //        LOG_INFO(fmt::format("Test {} thread {} finished", test_i, thread_i));
    }
};

class Test1Obj : public TestObj {
public:
    Test1Obj(size_t avg_file_size, BufferManager *buffer_mgr, std::shared_ptr<std::string> data_dir, std::shared_ptr<std::string> temp_dir)
        : avg_file_size(avg_file_size), buffer_mgr_(buffer_mgr), data_dir_(data_dir), temp_dir_(temp_dir) {}

private:
    const size_t avg_file_size;
    BufferManager *buffer_mgr_;
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;

public:
    void Init(bool alloc_new, FileInfo &file_info) override {
        auto file_name = std::make_shared<std::string>(fmt::format("file_{}", file_info.file_id_));
        if (alloc_new) {
            size_t file_size = rand() % avg_file_size + avg_file_size / 2;
            file_info.file_size_ = file_size;
            auto file_worker =
                std::make_unique<DataFileWorker>(data_dir_, temp_dir_, std::make_shared<std::string>(""), file_name, file_size, nullptr);
            file_info.buffer_obj_ = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
            file_info.buffer_obj_->AddObjRc();
        } else {
            auto file_worker =
                std::make_unique<DataFileWorker>(data_dir_, temp_dir_, std::make_shared<std::string>(""), file_name, file_info.file_size_, nullptr);
            file_info.buffer_obj_ = buffer_mgr_->GetBufferObject(std::move(file_worker));
        }
    }

    void Write(FileInfo &file_info) override {
        size_t visit_cnt = file_info.visit_cnt_;
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        auto *data = reinterpret_cast<char *>(buffer_handle.GetDataMut());
        for (size_t i = 0; i < file_info.file_size_; ++i) {
            data[i] = 'a' + (visit_cnt % 26);
        }
        file_info.buffer_obj_->SetDataSize(file_info.file_size_);
    }

    void Check(const FileInfo &file_info) override {
        size_t visit_cnt = file_info.visit_cnt_;
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        const auto *data = reinterpret_cast<const char *>(buffer_handle.GetData());
        for (size_t i = 0; i < file_info.file_size_; ++i) {
            EXPECT_EQ(data[i], char('a' + (visit_cnt - 1) % 26));
        }
    }
};

TEST_F(BufferManagerParallelTest, parallel_test1) {
    for (int i = 0; i < 1; ++i) {
        std::shared_ptr<PersistenceManager> persistence_manager_ =
            std::make_shared<PersistenceManager>(*persistence_dir_, *data_dir_, DEFAULT_PERSISTENCE_OBJECT_SIZE_LIMIT);
        auto buffer_mgr = std::make_unique<BufferManager>(buffer_size, data_dir_, temp_dir_, persistence_manager_.get());
        auto test1_obj = std::make_unique<Test1Obj>(avg_file_size, buffer_mgr.get(), data_dir_, temp_dir_);

        std::vector<FileInfo> file_infos;
        for (size_t i = 0; i < file_n; ++i) {
            file_infos.emplace_back(i);
        }
        //        LOG_INFO(fmt::format("Start parallel test1 {}", i));
        for (size_t test_i = 0; test_i < test_n_; test_i++) {
            std::atomic<size_t> finished_n = 0;
            for (auto &file_info : file_infos) {
                file_info.buffer_obj_ = nullptr;
            }

            std::vector<std::thread> threads;
            for (size_t thread_i = 0; thread_i < thread_n; ++thread_i) {
                threads.emplace_back([&, thread_i]() { TestRoutine(file_infos, test_i, thread_i, test1_obj.get(), finished_n); });
            }
            for (auto &thread : threads) {
                thread.join();
            }
        }
        EXPECT_EQ(buffer_mgr->memory_usage(), 0);
        buffer_mgr->RemoveClean(nullptr);

        //        LOG_INFO(fmt::format("Finished parallel test1 {}", i));
        ResetDir();
    }
}

class Test2Obj : public TestObj {
public:
    Test2Obj(size_t var_file_step, BufferManager *buffer_mgr, std::shared_ptr<std::string> data_dir, std::shared_ptr<std::string> temp_dir)
        : var_file_step(var_file_step), buffer_mgr_(buffer_mgr), data_dir_(data_dir), temp_dir_(temp_dir) {}

private:
    const size_t var_file_step;
    BufferManager *buffer_mgr_;
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;

public:
    void Init(bool alloc_new, FileInfo &file_info) override {
        auto file_name = std::make_shared<std::string>(fmt::format("file_{}", file_info.file_id_));
        if (alloc_new) {
            auto file_worker = std::make_unique<VarFileWorker>(data_dir_, temp_dir_, std::make_shared<std::string>(""), file_name, 0, nullptr);
            file_info.buffer_obj_ = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
            file_info.buffer_obj_->AddObjRc();
        } else {
            auto file_worker =
                std::make_unique<VarFileWorker>(data_dir_, temp_dir_, std::make_shared<std::string>(""), file_name, file_info.file_size_, nullptr);
            file_info.buffer_obj_ = buffer_mgr_->GetBufferObject(std::move(file_worker));
        }
    }

    void Write(FileInfo &file_info) override {
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        auto *buffer = reinterpret_cast<VarBuffer *>(buffer_handle.GetDataMut());
        auto data = std::make_unique<char[]>(var_file_step);
        for (size_t i = 0; i < var_file_step; ++i) {
            data[i] = 'a' + (i + file_info.file_size_) % 26;
        }
        buffer->Append(data.get(), var_file_step);
        file_info.file_size_ += var_file_step;

        ASSERT_EQ(file_info.buffer_obj_->GetBufferSize(), file_info.file_size_);
        LOG_DEBUG(fmt::format("Write file {} size {}", file_info.file_id_, file_info.file_size_));
    }

    void Check(const FileInfo &file_info) override {
        BufferHandle buffer_handle = file_info.buffer_obj_->Load();
        const auto *buffer = reinterpret_cast<const VarBuffer *>(buffer_handle.GetData());
        for (size_t i = 0; i < file_info.file_size_; i += var_file_step) {
            const char *data = buffer->Get(i, var_file_step);
            for (size_t j = 0; j < var_file_step; ++j) {
                EXPECT_EQ(data[j], char('a' + (i + j) % 26));
            }
        }
    }
};

TEST_F(BufferManagerParallelTest, parallel_test2) {
    for (int i = 0; i < 1; ++i) {
        auto buffer_mgr = std::make_unique<BufferManager>(buffer_size, data_dir_, temp_dir_, nullptr);
        auto test2_obj = std::make_unique<Test2Obj>(var_file_step, buffer_mgr.get(), data_dir_, temp_dir_);

        std::vector<FileInfo> file_infos;
        for (size_t i = 0; i < file_n; ++i) {
            file_infos.emplace_back(i);
        }
        //        LOG_INFO(fmt::format("Start parallel test2 {}", i));
        for (size_t test_i = 0; test_i < test_n_; test_i++) {
            std::atomic<size_t> finished_n = 0;
            for (auto &file_info : file_infos) {
                file_info.buffer_obj_ = nullptr;
            }

            std::vector<std::future<bool>> futures;
            for (size_t thread_i = 0; thread_i < thread_n; ++thread_i) {
                auto f = std::async(std::launch::async, [&, thread_i]() {
                    try {
                        TestRoutine(file_infos, test_i, thread_i, test2_obj.get(), finished_n);
                        return true;
                    } catch (const UnrecoverableException &e) {
                        return false;
                    }
                });
                futures.push_back(std::move(f));
            }
            bool success = true;
            for (auto &f : futures) {
                if (!f.get()) {
                    success = false;
                }
            }
            if (!success) {
                break;
            }
        }
        if (buffer_mgr->memory_usage() != 0) {
            auto infos = buffer_mgr->GetBufferObjectsInfo();
            for (const auto &info : infos) {
                LOG_INFO(fmt::format("BufferObjectInfo: path: {}, status: {}, type: {}, size: {}",
                                     info.object_path_,
                                     BufferStatusToString(info.buffered_status_),
                                     BufferTypeToString(info.buffered_type_),
                                     info.object_size_));
            }
        }

        ASSERT_EQ(buffer_mgr->memory_usage(), 0);
        buffer_mgr->RemoveClean(nullptr);

        //        LOG_INFO(fmt::format("Finished parallel test2 {}", i));
        ResetDir();
    }
}
