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
import crash_handler;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.test_tracer;

import :ut.base_test;
import :ut.crash_handler;
import :stl;
import :base_memindex;
import :mem_index;
import :memindex_tracer;
import :bg_task;
import :blocking_queue;
import third_party;
import :logger;
import :new_txn;
import :infinity_context;
#endif

import compilation_config;

using namespace infinity;

class TestMemIndexTracer;

class TestCatalog {
public:
    void SetTracer(TestMemIndexTracer *tracer) { tracer_ = tracer; }

public:
    Vector<SharedPtr<MemIndexDetail>> GetMemIndexes();

    SharedPtr<MemIndex> GetMemIndex(const String &index_name);

    void AppendMemIndex(const String &index_name, SizeT row_cnt, SizeT mem_used);

    void DumpMemIndex(const String &index_name);

    void reset() { memindexes_.clear(); }

private:
    TestMemIndexTracer *tracer_;

    std::mutex mtx_;
    HashMap<String, SharedPtr<MemIndex>> memindexes_;
};

Vector<SharedPtr<MemIndexDetail>> TestCatalog::GetMemIndexes() {
    std::unique_lock<std::mutex> lock(mtx_);
    Vector<SharedPtr<MemIndexDetail>> ret;
    for (auto &iter : memindexes_) {
        SharedPtr<MemIndexDetail> detail = MakeShared<MemIndexDetail>();
        SharedPtr<DummyIndexInMem> memindex = iter.second->GetDummyIndex();
        detail->index_name_ = memindex->index_name_;
        detail->table_name_ = memindex->table_name_;
        detail->db_name_ = memindex->db_name_;
        detail->mem_used_ = memindex->mem_used_;
        detail->row_count_ = memindex->row_cnt_;
        ret.push_back(detail);
    }
    std::sort(ret.begin(), ret.end(), [](const SharedPtr<MemIndexDetail> &lhs, const SharedPtr<MemIndexDetail> &rhs) {
        return lhs->mem_used_ > rhs->mem_used_;
    });
    return ret;
}

SharedPtr<MemIndex> TestCatalog::GetMemIndex(const String &index_name) {
    std::lock_guard lck(mtx_);
    auto iter = memindexes_.find(index_name);
    if (iter != memindexes_.end()) {
        return iter->second;
    }
    return nullptr;
}

void TestCatalog::AppendMemIndex(const String &index_name, SizeT row_cnt, SizeT mem_used) {
    SharedPtr<MemIndex> mem_index = nullptr;
    {
        std::lock_guard lck(mtx_);
        auto iter = memindexes_.find(index_name);
        if (iter != memindexes_.end()) {
            mem_index = iter->second;
        } else {
            mem_index = MakeShared<MemIndex>();
            mem_index->SetDummyIndex(MakeShared<DummyIndexInMem>("db1", "tbl1", index_name, 0, tracer_));
            memindexes_.emplace(index_name, mem_index);
        }
    }
    mem_index->GetDummyIndex()->Append(row_cnt, mem_used);
}

void TestCatalog::DumpMemIndex(const String &index_name) {
    SharedPtr<MemIndex> memindex = nullptr;
    {
        std::lock_guard lck(mtx_);
        if (auto iter = memindexes_.find(index_name); iter != memindexes_.end()) {
            memindex = std::move(iter->second);
            memindexes_.erase(iter);
        }
    }
    if (memindex != nullptr) {
        memindex->GetDummyIndex()->Dump();
    }
}

class TestMemIndexTracer : public MemIndexTracer {
public:
    TestMemIndexTracer(SizeT index_memory_limit, TestCatalog &catalog) : MemIndexTracer(index_memory_limit), catalog_(catalog) {
        dump_thread_ = Thread([this] { DumpRoutine(); });
    }

    ~TestMemIndexTracer() override {
        task_queue_.Enqueue(nullptr);
        dump_thread_.join();
        catalog_.reset();
    }

    void TriggerDump(SharedPtr<DumpMemIndexTask> task) override {
        LOG_INFO(fmt::format("Submit dump task: {}", task->ToString()));
        task_queue_.Enqueue(std::move(task));
    }

    NewTxn *GetTxn() override { return nullptr; }

    Vector<SharedPtr<MemIndexDetail>> GetAllMemIndexes(NewTxn *new_txn) override { return catalog_.GetMemIndexes(); }

    void HandleDump(SharedPtr<DumpMemIndexTask> task);

private:
    void DumpRoutine();

private:
    TestCatalog &catalog_;

    BlockingQueue<SharedPtr<DumpMemIndexTask>> task_queue_{"TestMemIndexTracer"};
    Thread dump_thread_;
};

void TestMemIndexTracer::HandleDump(SharedPtr<DumpMemIndexTask> task) {
    SharedPtr<MemIndex> mem_index = catalog_.GetMemIndex(task->index_name_);
    catalog_.DumpMemIndex(task->index_name_);
    this->DumpDone(mem_index);
}

void TestMemIndexTracer::DumpRoutine() {
    while (true) {
        auto task = task_queue_.DequeueReturn();
        if (task.get() == nullptr) {
            break;
        }
        HandleDump(std::move(task));
    }
}

class MemIndexTracerTest : public BaseTest {};

TEST_F(MemIndexTracerTest, test1) {
    // Earlier cases may leave a dirty infinity instance. Destroy it first.
    infinity::InfinityContext::instance().UnInit();
    RemoveDbDirs();
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
    //    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    SizeT memory_limit = 50;
    TestCatalog catalog;

    TestMemIndexTracer tracer(memory_limit, catalog);
    catalog.SetTracer(&tracer);

    catalog.AppendMemIndex("idx1", 10, 10);
    catalog.AppendMemIndex("idx2", 30, 30);
    catalog.AppendMemIndex("idx3", 20, 20);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    // Check idx2 has been dumpped.
    Vector<SharedPtr<MemIndexDetail>> details = catalog.GetMemIndexes();
    EXPECT_EQ(details.size(), 2);
    for (const auto &detail : details) {
        EXPECT_NE(detail->index_name_, "idx2");
    }

    infinity::InfinityContext::instance().UnInit();
}

TEST_F(MemIndexTracerTest, test2) {
    // Install signal handlers for crash debugging
    infinity::CrashHandlerGuard crash_guard(__FUNCTION__);
    fprintf(stderr, "Starting MemIndexTracerTest.test2 with crash detection enabled\n");

    // Earlier cases may leave a dirty infinity instance. Destroy it first.
    infinity::InfinityContext::instance().UnInit();
    RemoveDbDirs();
    std::shared_ptr<std::string> config_path = std::make_shared<std::string>(std::string(test_data_path()) + "/config/test_buffer_obj.toml");
    //    RemoveDbDirs();
    infinity::InfinityContext::instance().InitPhase1(config_path);
    infinity::InfinityContext::instance().InitPhase2();

    auto Test = []() {
        int thread_n = 2;
        SizeT memory_limit = 50;
        TestCatalog catalog;

        TestMemIndexTracer tracer(memory_limit, catalog);
        catalog.SetTracer(&tracer);
        int index_n = 10;
        int max_append_mem = 10;
        int iterate_n = 1000;

        auto test_f = [&](int id) {
            for (int i = 0; i < iterate_n; ++i) {
                int idx_i = rand() % index_n;
                String idx_name = "idx" + std::to_string(idx_i);
                SizeT mem = rand() % max_append_mem + 1;
                SizeT row_cnt = mem;
                catalog.AppendMemIndex(idx_name, row_cnt, mem);
            }
        };
        Vector<Thread> threads;
        for (int i = 0; i < thread_n; ++i) {
            threads.emplace_back(test_f, i);
        }
        for (auto &th : threads) {
            th.join();
        }
    };
    Test();

    infinity::InfinityContext::instance().UnInit();
}
