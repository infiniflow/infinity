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

#include <vector>

module memindex_tracer;

import stl;
import base_memindex;
import bg_task;
import infinity_context;
import infinity_exception;
import logger;
import third_party;
import catalog;
import db_entry;
import table_entry;
import table_index_entry;
import txn_manager;

namespace infinity {

void MemIndexTracer::DecreaseMemUsed(SizeT mem_used) {
    SizeT old_index_memory = cur_index_memory_.fetch_sub(mem_used);
    if (old_index_memory < mem_used) {
        UnrecoverableException(fmt::format("Memindex memory {} is larger than current index memory {}", mem_used, old_index_memory));
    }
}

void MemIndexTracer::DumpDone(SizeT actual_dump_size, BaseMemIndex *mem_index) {
    std::lock_guard lck(mtx_);
    auto iter = proposed_dump_.find(mem_index);
    if (iter == proposed_dump_.end()) {
        UnrecoverableException(fmt::format("Dump task {} is not found", (u64)(mem_index)));
    }
    SizeT old_index_memory = cur_index_memory_.fetch_sub(actual_dump_size);
    if (old_index_memory < actual_dump_size) {
        UnrecoverableException(fmt::format("Dump size {} is larger than current index memory {}", actual_dump_size, old_index_memory));
    }
    SizeT proposed_dump_size = iter->second;
    if (proposed_dump_size > actual_dump_size) {
        UnrecoverableException(fmt::format("Dump size {} is larger than proposed dump size {}", actual_dump_size, proposed_dump_size));
    }
    acc_proposed_dump_.fetch_sub(proposed_dump_size);
    proposed_dump_.erase(iter);
}

void MemIndexTracer::DumpFail(BaseMemIndex *mem_index) {
    std::lock_guard lck(mtx_);
    auto iter = proposed_dump_.find(mem_index);
    if (iter == proposed_dump_.end()) {
        UnrecoverableException(fmt::format("Dump task {} is not found", (u64)(mem_index)));
    }
    SizeT proposed_dump_size = iter->second;
    acc_proposed_dump_.fetch_sub(proposed_dump_size);

    proposed_dump_.erase(iter);
}

Vector<MemIndexTracerInfo> MemIndexTracer::GetMemIndexTracerInfo(Txn *txn) {
    Vector<BaseMemIndex *> mem_indexes = GetUndumpedMemIndexes(txn);
    Vector<MemIndexTracerInfo> info_vec;
    for (auto *mem_index : mem_indexes) {
        info_vec.push_back(mem_index->GetInfo());
    }
    return info_vec;
}

Vector<BaseMemIndex *> MemIndexTracer::GetUndumpedMemIndexes(Txn *txn) {
    Vector<BaseMemIndex *> results;
    Vector<BaseMemIndex *> mem_indexes = GetAllMemIndexes(txn);
    for (auto *mem_index : mem_indexes) {
        if (auto iter = proposed_dump_.find(mem_index); iter == proposed_dump_.end()) {
            auto info = mem_index->GetInfo();
            proposed_dump_.emplace(mem_index, info.mem_used_);
            results.push_back(mem_index);
        }
    }
    return results;
}

UniquePtr<DumpIndexTask> MemIndexTracer::MakeDumpTask() {
    std::lock_guard lck(mtx_);

    Txn *txn = GetTxn();
    Vector<BaseMemIndex *> mem_indexes = GetUndumpedMemIndexes(txn);

    if (mem_indexes.empty()) {
        LOG_WARN("Cannot find memindex to dump");
        return nullptr;
    }
    SizeT dump_idx = ChooseDump(mem_indexes);
    auto *mem_index = mem_indexes[dump_idx];
    auto info = mem_index->GetInfo();
    auto dump_task = MakeUnique<DumpIndexTask>(mem_index, txn);

    acc_proposed_dump_.fetch_add(info.mem_used_);
    proposed_dump_[mem_index] = info.mem_used_;
    return dump_task;
}

SizeT MemIndexTracer::ChooseDump(const Vector<BaseMemIndex *> &mem_indexes) {
    Vector<MemIndexTracerInfo> info_vec;
    info_vec.reserve(mem_indexes.size());
    for (auto *mem_index : mem_indexes) {
        info_vec.push_back(mem_index->GetInfo());
    }
    auto max_iter = std::max_element(info_vec.begin(), info_vec.end(), [](const auto &a, const auto &b) { return a.mem_used_ < b.mem_used_; });
    return std::distance(info_vec.begin(), max_iter);
}

void BGMemIndexTracer::TriggerDump(UniquePtr<DumpIndexTask> dump_task) {
    auto *compaction_process = InfinityContext::instance().storage()->compaction_processor();

    LOG_INFO(fmt::format("Submit dump task: {}", dump_task->ToString()));
    compaction_process->Submit(std::move(dump_task));
}

Txn *BGMemIndexTracer::GetTxn() {
    Txn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Dump index"));
    return txn;
}

Vector<BaseMemIndex *> BGMemIndexTracer::GetAllMemIndexes(Txn *scan_txn) {
    Vector<BaseMemIndex *> mem_indexes;
    TransactionID txn_id = scan_txn->TxnID();
    TxnTimeStamp begin_ts = scan_txn->BeginTS();
    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    for (auto *db_entry : db_entries) {
        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
        for (auto *table_entry : table_entries) {
            Vector<TableIndexEntry *> table_index_entries = table_entry->TableIndexes(txn_id, begin_ts);
            for (auto *table_index_entry : table_index_entries) {
                Vector<BaseMemIndex *> memindex_list = table_index_entry->GetMemIndex();
                mem_indexes.insert(mem_indexes.end(), memindex_list.begin(), memindex_list.end());
            }
        }
    }
    return mem_indexes;
}

} // namespace infinity
