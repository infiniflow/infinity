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
#include <random>

import stl;
import memindex_tracer;
import base_memindex;
import bg_task;
import blocking_queue;
import third_party;
import logger;
import txn;
import table_index_entry;

using namespace infinity;

class TestMemIndexTracer;

class TestMemIndex : public BaseMemIndex {
public:
    TestMemIndex(SharedPtr<String> index_name, TestMemIndexTracer *tracer, bool trace);

    virtual ~TestMemIndex();

    MemIndexTracerInfo GetInfo() const override {
        std::lock_guard lck(mtx_);
        return MemIndexTracerInfo(index_name_, table_name_, db_name_, mem_used_, row_count_);
    }

    TableIndexEntry *table_index_entry() const override { return nullptr; }

    void AddMemUsed(SizeT usage, SizeT row_cnt);

    void Dump(SizeT &usage, SizeT &row_cnt) && {
        std::lock_guard lck(mtx_);
        usage = mem_used_;
        row_cnt = row_count_;
        mem_used_ = 0;
        row_count_ = 0;
        trace_ = false;
    }

public:
    SharedPtr<String> db_name_;
    SharedPtr<String> table_name_;
    SharedPtr<String> index_name_;

private:
    mutable std::mutex mtx_;
    SizeT mem_used_ = 0;
    SizeT row_count_ = 0;

    TestMemIndexTracer *tracer_;
    bool trace_;
};

class TestCatalog {
public:
    void SetTracer(TestMemIndexTracer *tracer) { tracer_ = tracer; }

private:
    TestMemIndex *GetMemIndexInner(const String &index_name);

public:
    Vector<BaseMemIndex *> GetMemIndexes();

    TestMemIndex *GetMemIndex(const String &index_name);

    void AppendMemIndex(const String &index_name, SizeT mem_used, SizeT row_cnt);

    bool DumpMemIndex(const String &index_name, SizeT &mem_used, SizeT &row_cnt);

    bool CleanupIndex(const String &index_name);

    void reset() { memindexes_.clear(); }

private:
    TestMemIndexTracer *tracer_;

    std::mutex mtx_;
    HashMap<String, UniquePtr<TestMemIndex>> memindexes_;
};

Vector<BaseMemIndex *> TestCatalog::GetMemIndexes() {
    Vector<BaseMemIndex *> ret;
    for (auto &iter : memindexes_) {
        ret.push_back(iter.second.get());
    }
    return ret;
}

TestMemIndex *TestCatalog::GetMemIndexInner(const String &index_name) {
    auto iter = memindexes_.find(index_name);
    if (iter != memindexes_.end()) {
        return iter->second.get();
    }
    auto memindex = MakeUnique<TestMemIndex>(MakeShared<String>(index_name), tracer_, true);
    auto *ret = memindex.get();
    memindexes_.emplace(index_name, std::move(memindex));
    return ret;
}

TestMemIndex *TestCatalog::GetMemIndex(const String &index_name) {
    std::lock_guard lck(mtx_);
    return GetMemIndexInner(index_name);
}

void TestCatalog::AppendMemIndex(const String &index_name, SizeT mem_used, SizeT row_cnt) {
    std::lock_guard lck(mtx_);
    auto *memindex = GetMemIndexInner(index_name);
    memindex->AddMemUsed(mem_used, row_cnt);
}

bool TestCatalog::DumpMemIndex(const String &index_name, SizeT &mem_used, SizeT &row_cnt) {
    std::lock_guard lck(mtx_);
    if (auto iter = memindexes_.find(index_name); iter != memindexes_.end()) {
        auto memindex = std::move(iter->second);
        memindexes_.erase(iter);
        std::move(*memindex).Dump(mem_used, row_cnt);
        return true;
    }
    return false;
}

bool TestCatalog::CleanupIndex(const String &index_name) {
    std::lock_guard lck(mtx_);
    if (auto iter = memindexes_.find(index_name); iter != memindexes_.end()) {
        memindexes_.erase(iter);
        return true;
    }
    return false;
}

class TestMemIndexTracer : public MemIndexTracer {
public:
    TestMemIndexTracer(SizeT index_memory_limit, TestCatalog &catalog, bool may_fail)
        : MemIndexTracer(index_memory_limit), catalog_(catalog), may_fail_(may_fail) {
        dump_thread_ = Thread([this] { DumpRoutine(); });
    }

    virtual ~TestMemIndexTracer() {
        task_queue_.Enqueue(nullptr);
        dump_thread_.join();
        catalog_.reset();
        EXPECT_EQ(this->cur_index_memory(), 0ul);
    }

    void TriggerDump(UniquePtr<DumpIndexTask> task) override { task_queue_.Enqueue(std::move(task)); }

    Txn *GetTxn() override { return nullptr; }

    Vector<BaseMemIndex *> GetAllMemIndexes(Txn *txn) override { return catalog_.GetMemIndexes(); }

    void HandleDump(UniquePtr<DumpIndexTask> task);

private:
    void DumpRoutine();

private:
    TestCatalog &catalog_;
    bool may_fail_;

    BlockingQueue<UniquePtr<DumpIndexTask>> task_queue_;
    Thread dump_thread_;
};

TestMemIndex::TestMemIndex(SharedPtr<String> index_name, TestMemIndexTracer *tracer, bool trace) : tracer_(tracer), trace_(trace) {
    db_name_ = MakeShared<String>("test_db");
    table_name_ = MakeShared<String>("test_table");
    index_name_ = index_name;
}

TestMemIndex::~TestMemIndex() {
    if (trace_) {
        tracer_->DecreaseMemUsed(mem_used_);
    }
}

void TestMemIndex::AddMemUsed(SizeT usage, SizeT row_cnt) {
    {
        std::lock_guard lck(mtx_);
        mem_used_ += usage;
        row_count_ += row_cnt;
    }
    tracer_->AddMemUsed(usage);
}

void TestMemIndexTracer::HandleDump(UniquePtr<DumpIndexTask> task) {
    auto *mem_index = static_cast<TestMemIndex *>(task->mem_index_);
    static int dump_id = 0;
    if ((dump_id++) % 4 != 0 || !may_fail_) { // success dump
        SizeT dumped_size = 0;
        SizeT row_cnt = 0;
        bool res = catalog_.DumpMemIndex(*mem_index->index_name_, dumped_size, row_cnt);
        if (res) {
            this->DumpDone(dumped_size, mem_index);
        }
    } else { // fail dump
        this->DumpFail(mem_index);
    }
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
    SizeT memory_limit = 50;
    TestCatalog catalog;

    TestMemIndexTracer tracer(memory_limit, catalog, false);
    catalog.SetTracer(&tracer);

    catalog.AppendMemIndex("idx1", 10, 10);
    catalog.AppendMemIndex("idx2", 30, 30);
    catalog.AppendMemIndex("idx3", 20, 20);
}

TEST_F(MemIndexTracerTest, test2) {
    auto Test = [](bool may_fail) {
        int thread_n = 2;
        SizeT memory_limit = 50;
        TestCatalog catalog;

        TestMemIndexTracer tracer(memory_limit, catalog, may_fail);
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
                catalog.AppendMemIndex(idx_name, mem, row_cnt);
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
    Test(false);
    Test(true);
}
