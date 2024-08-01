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

namespace infinity {

class BaseMemIndex;

export struct MemIndexTracerInfo {
    SharedPtr<String> index_name_;
    SharedPtr<String> table_name_;
    SharedPtr<String> db_name_;
    SizeT mem_used_;
    SizeT row_count_;
};

export class MemIndexTracer {
public:
    MemIndexTracer(SizeT index_memory_limit);

    virtual ~MemIndexTracer() = default;

    void RegisterMemIndex(BaseMemIndex *memindex);

    void UnregisterMemIndex(BaseMemIndex *memindex, SizeT mem_used);

    void AddMemUsed(SizeT usage);

    void DumpDone(SizeT actual_dump_size, int dump_task_id);

    void DumpFail(int dump_task_id);

    Vector<MemIndexTracerInfo> GetMemIndexTracerInfo();

    virtual void TriggerDump(UniquePtr<DumpIndexTask> task) = 0;

    SizeT cur_index_memory() const { return cur_index_memory_.load(); }

private:
    UniquePtr<DumpIndexTask> MakeDumpTask();

    using MemIndexMapIter = HashSet<BaseMemIndex *>::iterator;

    static SizeT ChooseDump(const Vector<Pair<MemIndexTracerInfo, MemIndexMapIter>> &info_vec);

private:
    const SizeT index_memory_limit_;
    Atomic<SizeT> cur_index_memory_ = 0;

    std::mutex mtx_;
    HashSet<BaseMemIndex *> memindexes_{};

    i64 dump_task_id_ = 0;
    HashMap<int, Pair<SizeT, BaseMemIndex *>> proposed_dump_{};
    Atomic<SizeT> accumulate_dump_size_ = 0;
};

inline void MemIndexTracer::AddMemUsed(SizeT add) {
    if (add == 0) {
        return;
    }
    SizeT old_index_memory = cur_index_memory_.fetch_add(add);
    if (old_index_memory + add > index_memory_limit_) {
        if (old_index_memory + add > index_memory_limit_ - accumulate_dump_size_.load()) {
            auto dump_task = MakeDumpTask();
            TriggerDump(std::move(dump_task));
        }
    }
}

export class BGMemIndexTracer : public MemIndexTracer {
public:
    BGMemIndexTracer(SizeT index_memory_limit) : MemIndexTracer(index_memory_limit) {}

    void TriggerDump(UniquePtr<DumpIndexTask> task) override;
};

} // namespace infinity
