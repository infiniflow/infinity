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

export module infinity_core:memindex_tracer;

import :logger;

import third_party;

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
    MemIndexTracerInfo(std::shared_ptr<std::string> index_name,
                       std::shared_ptr<std::string> table_name,
                       std::shared_ptr<std::string> db_name,
                       size_t mem_used,
                       size_t row_count)
        : index_name_(std::move(index_name)), table_name_(std::move(table_name)), db_name_(std::move(db_name)), mem_used_(mem_used),
          row_count_(row_count) {}
    std::shared_ptr<std::string> index_name_;
    std::shared_ptr<std::string> table_name_;
    std::shared_ptr<std::string> db_name_;
    size_t mem_used_;
    size_t row_count_;
};

export struct MemIndexDetail {
    std::shared_ptr<MemIndex> mem_index_;
    std::string db_name_{};
    std::string table_name_{};
    std::string index_name_{};
    SegmentID segment_id_{};
    RowID begin_row_id_{};
    size_t mem_used_{};
    size_t row_count_{};
    bool is_emvb_index_{};
};

export class MemIndexTracer {
public:
    MemIndexTracer(size_t index_memory_limit) : index_memory_limit_(index_memory_limit) {}

    virtual ~MemIndexTracer() = default;

    // Need call only once after construction.
    void InitMemUsed();

    void DecreaseMemUsed(size_t mem_dec);

    void IncreaseMemoryUsage(size_t mem_inc);

private:
    bool TryTriggerDump();

public:
    void DumpDone(std::shared_ptr<MemIndex> mem_index);

    std::vector<MemIndexTracerInfo> GetMemIndexTracerInfo(NewTxn *txn);

    virtual void TriggerDump(std::shared_ptr<DumpMemIndexTask> task) = 0;

protected:
    virtual std::vector<std::shared_ptr<MemIndexDetail>> GetAllMemIndexes(NewTxn *new_txn) = 0;

    using MemIndexMapIter = std::unordered_set<BaseMemIndex *>::iterator;

    std::vector<std::shared_ptr<DumpMemIndexTask>> MakeDumpTask();

    static size_t ChooseDump(const std::vector<BaseMemIndex *> &mem_indexes);

protected:
    const size_t index_memory_limit_;

    mutable std::mutex mtx_; // protect cur_index_memory_, proposed_dump_ and proposed_dump_size_
    size_t cur_index_memory_ = 0;
    std::unordered_map<std::shared_ptr<MemIndex>, size_t> proposed_dump_{};
    size_t proposed_dump_size_ = 0;
};

inline void MemIndexTracer::IncreaseMemoryUsage(size_t mem_inc) {
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
    BGMemIndexTracer(size_t index_memory_limit, NewTxnManager *txn_mgr);

    ~BGMemIndexTracer() override;

    void TriggerDump(std::shared_ptr<DumpMemIndexTask> task) override;

protected:
    std::vector<std::shared_ptr<MemIndexDetail>> GetAllMemIndexes(NewTxn *new_txn) override;

private:
    NewTxnManager *txn_mgr_;
};

} // namespace infinity
