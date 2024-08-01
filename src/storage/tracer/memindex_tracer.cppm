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

    void RegisterMemIndex(BaseMemIndex *memindex);

    void AddUsage(SizeT usage);

    Vector<MemIndexTracerInfo> GetMemIndexTracerInfo();

private:
    void TriggerDump();

private:
    const SizeT index_memory_limit_;
    Atomic<SizeT> cur_index_memory = 0;

    std::mutex mtx_;
    HashSet<BaseMemIndex *> memindexes_{};
};

inline void MemIndexTracer::RegisterMemIndex(BaseMemIndex *memindex) { memindexes_.insert(memindex); }

inline void MemIndexTracer::AddUsage(SizeT add) {
    SizeT old_index_memory = cur_index_memory.fetch_add(add);
    if (old_index_memory > index_memory_limit_) {
        TriggerDump();
    }
}

} // namespace infinity
