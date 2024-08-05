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

module memindex_tracer;

import stl;
import base_memindex;
import bg_task;
import infinity_context;
import infinity_exception;
import logger;
import third_party;

namespace infinity {

MemIndexTracer::MemIndexTracer(SizeT index_memory_limit) : index_memory_limit_(index_memory_limit) {}

void MemIndexTracer::RegisterMemIndex(BaseMemIndex *memindex) {
    std::lock_guard lck(mtx_);
    memindexes_.insert(memindex);
}

void MemIndexTracer::UnregisterMemIndex(BaseMemIndex *memindex, SizeT mem_used) {
    std::lock_guard lck(mtx_);
    memindexes_.erase(memindex);
    SizeT old_index_memory = cur_index_memory_.fetch_sub(mem_used);
    if (old_index_memory < mem_used) {
        UnrecoverableException(fmt::format("Memindex memory {} is larger than current index memory {}", mem_used, old_index_memory));
    }
}

void MemIndexTracer::DumpDone(SizeT actual_dump_size, int dump_task_id) {
    std::lock_guard lck(mtx_);
    auto iter = proposed_dump_.find(dump_task_id);
    if (iter == proposed_dump_.end()) {
        UnrecoverableException(fmt::format("Dump task {} is not found", dump_task_id));
    }
    SizeT old_index_memory = cur_index_memory_.fetch_sub(actual_dump_size);
    if (old_index_memory < actual_dump_size) {
        UnrecoverableException(fmt::format("Dump size {} is larger than current index memory {}", actual_dump_size, old_index_memory));
    }
    const auto &[proposed_dump_size, dumped_base_memindex] = iter->second;
    if (proposed_dump_size > actual_dump_size) {
        UnrecoverableException(fmt::format("Dump size {} is larger than proposed dump size {}", actual_dump_size, proposed_dump_size));
    }
    acc_proposed_dump_.fetch_sub(proposed_dump_size);
    proposed_dump_.erase(iter);
}

void MemIndexTracer::DumpFail(int dump_task_id) {
    std::lock_guard lck(mtx_);
    auto iter = proposed_dump_.find(dump_task_id);
    if (iter == proposed_dump_.end()) {
        UnrecoverableException(fmt::format("Dump task {} is not found", dump_task_id));
    }
    const auto &[proposed_dump_size, dumped_base_memindex] = iter->second;
    acc_proposed_dump_.fetch_sub(proposed_dump_size);
    memindexes_.insert(dumped_base_memindex);

    proposed_dump_.erase(iter);
}

Vector<MemIndexTracerInfo> MemIndexTracer::GetMemIndexTracerInfo() {
    Vector<MemIndexTracerInfo> info_vec;
    std::lock_guard lck(mtx_);
    info_vec.reserve(memindexes_.size());
    for (auto *memindex : memindexes_) {
        info_vec.push_back(memindex->GetInfo());
    }
    return info_vec;
}

UniquePtr<DumpIndexTask> MemIndexTracer::MakeDumpTask() {
    std::lock_guard lck(mtx_);

    Vector<Pair<MemIndexTracerInfo, MemIndexMapIter>> info_vec;
    info_vec.reserve(memindexes_.size());
    for (auto iter = memindexes_.begin(); iter != memindexes_.end(); ++iter) {
        info_vec.emplace_back((*iter)->GetInfo(), iter);
    }
    if (info_vec.empty()) {
        return nullptr;
    }
    SizeT dump_idx = ChooseDump(info_vec);
    const auto &[info, max_iter] = info_vec[dump_idx];

    i64 id = dump_task_id_++;
    auto dump_task = MakeUnique<DumpIndexTask>(id, info.db_name_, info.table_name_, info.index_name_);

    auto *dumped_base_memindex = *max_iter;
    memindexes_.erase(max_iter);

    acc_proposed_dump_.fetch_add(info.mem_used_);
    proposed_dump_[id] = {info.mem_used_, dumped_base_memindex};
    return dump_task;
}

SizeT MemIndexTracer::ChooseDump(const Vector<Pair<MemIndexTracerInfo, MemIndexMapIter>> &info_vec) {
    auto max_iter =
        std::max_element(info_vec.begin(), info_vec.end(), [](const auto &a, const auto &b) { return a.first.mem_used_ < b.first.mem_used_; });
    return std::distance(info_vec.begin(), max_iter);
}

void BGMemIndexTracer::TriggerDump(UniquePtr<DumpIndexTask> dump_task) {
    auto *compaction_process = InfinityContext::instance().storage()->compaction_processor();

    LOG_INFO(fmt::format("Submit dump task: {}", dump_task->ToString()));
    compaction_process->Submit(std::move(dump_task));
}

} // namespace infinity
