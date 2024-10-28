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

export module memindex_tracer;

import stl;
import bg_task;
import third_party;
import logger;

namespace infinity {

class BaseMemIndex;
class Txn;
struct Catalog;
class TxnManager;

export struct MemIndexTracerInfo {
public:
    MemIndexTracerInfo(SharedPtr<String> index_name, SharedPtr<String> table_name, SharedPtr<String> db_name, SizeT mem_used, SizeT row_count)
        : index_name_(std::move(index_name)), table_name_(std::move(table_name)), db_name_(std::move(db_name)), mem_used_(mem_used),
          row_count_(row_count) {}
    SharedPtr<String> index_name_;
    SharedPtr<String> table_name_;
    SharedPtr<String> db_name_;
    SizeT mem_used_;
    SizeT row_count_;
};

export class MemIndexTracer {
public:
    MemIndexTracer(SizeT index_memory_limit) : index_memory_limit_(index_memory_limit) {}

    virtual ~MemIndexTracer() = default;

    void DecreaseMemUsed(SizeT mem_used);

    void AddMemUsed(SizeT usage);

    void DumpDone(SizeT actual_dump_size, BaseMemIndex *mem_index);

    void DumpFail(BaseMemIndex *mem_index);

    Vector<MemIndexTracerInfo> GetMemIndexTracerInfo(Txn *txn);

    Vector<BaseMemIndex *> GetUndumpedMemIndexes(Txn *txn);

    virtual void TriggerDump(UniquePtr<DumpIndexTask> task) = 0;

    SizeT cur_index_memory() const { return cur_index_memory_.load(); }

protected:
    virtual Txn *GetTxn() = 0;

    virtual Vector<BaseMemIndex *> GetAllMemIndexes(Txn *txn) = 0;

    using MemIndexMapIter = HashSet<BaseMemIndex *>::iterator;

    UniquePtr<DumpIndexTask> MakeDumpTask();

    static SizeT ChooseDump(const Vector<BaseMemIndex *> &mem_indexes);

protected:
    std::mutex mtx_;

    const SizeT index_memory_limit_;
    Atomic<SizeT> cur_index_memory_ = 0;

    HashMap<BaseMemIndex *, SizeT> proposed_dump_{};
    Atomic<SizeT> acc_proposed_dump_ = 0;
};

inline void MemIndexTracer::AddMemUsed(SizeT add) {
    LOG_TRACE(fmt::format("Add mem used: {}, mem index limit: {}", add, index_memory_limit_));
    if (add == 0 || index_memory_limit_ == 0) {
        return;
    }
    SizeT old_index_memory = cur_index_memory_.fetch_add(add);
    if (SizeT new_index_memory = old_index_memory + add; new_index_memory > index_memory_limit_) {
        if (new_index_memory > index_memory_limit_ + acc_proposed_dump_.load()) {
            auto dump_task = MakeDumpTask();
            if (dump_task.get() != nullptr) {
                TriggerDump(std::move(dump_task));
            }
        }
    }
}

export class BGMemIndexTracer : public MemIndexTracer {
public:
    BGMemIndexTracer(SizeT index_memory_limit, Catalog *catalog, TxnManager *txn_mgr)
        : MemIndexTracer(index_memory_limit), catalog_(catalog), txn_mgr_(txn_mgr) {}

    void TriggerDump(UniquePtr<DumpIndexTask> task) override;

protected:
    Txn *GetTxn() override;

    Vector<BaseMemIndex *> GetAllMemIndexes(Txn *txn) override;

private:
    Catalog *catalog_;
    TxnManager *txn_mgr_;
};

} // namespace infinity
