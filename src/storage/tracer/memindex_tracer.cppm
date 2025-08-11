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

export module infinity_core:memindex_tracer;

import :stl;
import third_party;
import :logger;
import row_id;

namespace infinity {

export class BaseMemIndex;
class NewTxn;
struct NewCatalog;
class NewTxnManager;
class DumpMemIndexTask;
class EMVBIndexInMem;
class MemIndex;

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

export struct MemIndexDetail {
    SharedPtr<MemIndex> mem_index_;
    String db_name_{};
    String table_name_{};
    String index_name_{};
    SegmentID segment_id_{};
    RowID begin_row_id_{};
    SizeT mem_used_{};
    SizeT row_count_{};
    bool is_emvb_index_{};
};

export class MemIndexTracer {
public:
    MemIndexTracer(SizeT index_memory_limit) : index_memory_limit_(index_memory_limit) {}

    virtual ~MemIndexTracer() = default;

    // Need call only once after construction.
    void InitMemUsed();

    void DecreaseMemUsed(SizeT mem_dec);

    void IncreaseMemoryUsage(SizeT mem_inc);

private:
    bool TryTriggerDump();

public:
    void DumpDone(SharedPtr<MemIndex> mem_index);

    Vector<MemIndexTracerInfo> GetMemIndexTracerInfo(NewTxn *txn);

    virtual void TriggerDump(SharedPtr<DumpMemIndexTask> task) = 0;

protected:
    virtual NewTxn *GetTxn() = 0;

    virtual Vector<SharedPtr<MemIndexDetail>> GetAllMemIndexes(NewTxn *new_txn) = 0;

    using MemIndexMapIter = HashSet<BaseMemIndex *>::iterator;

    Vector<SharedPtr<DumpMemIndexTask>> MakeDumpTask();

    static SizeT ChooseDump(const Vector<BaseMemIndex *> &mem_indexes);

protected:
    const SizeT index_memory_limit_;

    mutable std::mutex mtx_; // protect cur_index_memory_, proposed_dump_ and proposed_dump_size_
    SizeT cur_index_memory_ = 0;
    HashMap<SharedPtr<MemIndex>, SizeT> proposed_dump_{};
    SizeT proposed_dump_size_ = 0;
};

inline void MemIndexTracer::IncreaseMemoryUsage(SizeT mem_inc) {
    bool need_trigger_dump = false;
    {
        std::lock_guard lck(mtx_);
        LOG_TRACE(fmt::format("IncreaseMemoryUsage mem_inc {}, cur_index_memory_ {}, proposed_dump_size_ {}",
                              mem_inc,
                              cur_index_memory_,
                              proposed_dump_size_));
        if (mem_inc == 0 || index_memory_limit_ == 0) {
            return;
        }
        cur_index_memory_ += mem_inc;
        if (cur_index_memory_ > index_memory_limit_ + proposed_dump_size_) {
            need_trigger_dump = true;
            LOG_TRACE(fmt::format("mem index limit: {}, current index memory: {}, proposed dump size: {}, trigger dump index due to memory limit.",
                                  index_memory_limit_,
                                  cur_index_memory_,
                                  proposed_dump_size_));
        }
    }
    if (need_trigger_dump) {
        TryTriggerDump();
    }
}

export class BGMemIndexTracer : public MemIndexTracer {
public:
    BGMemIndexTracer(SizeT index_memory_limit, NewTxnManager *txn_mgr);

    ~BGMemIndexTracer() override;

    void TriggerDump(SharedPtr<DumpMemIndexTask> task) override;

protected:
    NewTxn *GetTxn() override;

    Vector<SharedPtr<MemIndexDetail>> GetAllMemIndexes(NewTxn *new_txn) override;

private:
    NewTxnManager *txn_mgr_;
};

} // namespace infinity
