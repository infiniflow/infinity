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
import emvb_index_in_mem;
import bg_task;
import infinity_context;
import infinity_exception;
import logger;
import third_party;
import txn_state;
import dump_index_process;
import storage;

import kv_store;
import new_catalog;
import new_txn_manager;
import mem_index;
import new_txn;
import status;
import defer_op;

namespace infinity {

void MemIndexTracer::DecreaseMemUsed(SizeT mem_used) {
    SizeT old_index_memory = cur_index_memory_.fetch_sub(mem_used);
    if (old_index_memory < mem_used) {
        UnrecoverableException(fmt::format("Memindex memory {} is larger than current index memory {}", mem_used, old_index_memory));
    }
}

bool MemIndexTracer::TryTriggerDump() {
    auto dump_task = MakeDumpTask();
    if (!dump_task) {
        return false;
    }
    LOG_TRACE(fmt::format("Dump triggered!"));
    TriggerDump(std::move(dump_task));
    return true;
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

Vector<MemIndexTracerInfo> MemIndexTracer::GetMemIndexTracerInfo(NewTxn *txn) {
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

Vector<BaseMemIndex *> MemIndexTracer::GetUndumpedMemIndexes(NewTxn *new_txn) {
    Vector<BaseMemIndex *> results;
    Vector<BaseMemIndex *> mem_indexes = GetAllMemIndexes(new_txn);
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

    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<NewTxn> new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("Dump index"), TransactionType::kNormal);

    bool make_task = false;
    DeferFn defer_op([&] {
        if (new_txn_shared.get() && !make_task) {
            auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
            Status status = new_txn_mgr->RollBackTxn(new_txn_shared.get());
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
        }
    });

    UniquePtr<DumpIndexTask> dump_task{};
    Vector<BaseMemIndex *> mem_indexes = GetUndumpedMemIndexes(new_txn_shared.get());
    Vector<EMVBIndexInMem *> emvb_indexes = GetEMVBMemIndexes(new_txn_shared.get());
    if (!mem_indexes.empty()) {
        SizeT dump_idx = ChooseDump(mem_indexes);
        BaseMemIndex *mem_index = mem_indexes[dump_idx];
        MemIndexTracerInfo info = mem_index->GetInfo();
        dump_task = MakeUnique<DumpIndexTask>(mem_index, new_txn_shared);

        acc_proposed_dump_.fetch_add(info.mem_used_);
        proposed_dump_[mem_index] = info.mem_used_;
    } else if (!emvb_indexes.empty()) {
        // FIXME: We do not calculate the memory used for each EMVB index,
        // so we choose the first EMVB index to dump.
        EMVBIndexInMem *emvb_index = emvb_indexes[0];
        dump_task = MakeUnique<DumpIndexTask>(emvb_index, new_txn_shared);
    } else {
        LOG_WARN("Cannot find memindex to dump");
        return nullptr;
    }

    make_task = true;
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

BGMemIndexTracer::BGMemIndexTracer(SizeT index_memory_limit, NewTxnManager *txn_mgr) : MemIndexTracer(index_memory_limit), txn_mgr_(txn_mgr) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("BGMemIndexTracer");
#endif
}

BGMemIndexTracer::~BGMemIndexTracer() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("BGMemIndexTracer");
#endif
}

void BGMemIndexTracer::TriggerDump(UniquePtr<DumpIndexTask> dump_task) {
    auto *dump_index_processor = InfinityContext::instance().storage()->dump_index_processor();

    LOG_INFO(fmt::format("Submit dump task: {}", dump_task->ToString()));
    dump_index_processor->Submit(std::move(dump_task));
}

NewTxn *BGMemIndexTracer::GetTxn() {
    if (!txn_mgr_) {
        return nullptr;
    }
    NewTxn *txn = txn_mgr_->BeginTxn(MakeUnique<String>("Dump index"), TransactionType::kNormal);
    return txn;
}

Vector<BaseMemIndex *> BGMemIndexTracer::GetAllMemIndexes(Txn *scan_txn) {
    Vector<BaseMemIndex *> mem_indexes;
    //    TransactionID txn_id = scan_txn->TxnID();
    //    TxnTimeStamp begin_ts = scan_txn->BeginTS();
    //    Vector<DBEntry *> db_entries = catalog_->Databases(txn_id, begin_ts);
    //    for (auto *db_entry : db_entries) {
    //        Vector<TableEntry *> table_entries = db_entry->TableCollections(txn_id, begin_ts);
    //        for (auto *table_entry : table_entries) {
    //            Vector<TableIndexEntry *> table_index_entries = table_entry->TableIndexes(txn_id, begin_ts);
    //            for (auto *table_index_entry : table_index_entries) {
    //                Vector<BaseMemIndex *> memindex_list = table_index_entry->GetMemIndex();
    //                mem_indexes.insert(mem_indexes.end(), memindex_list.begin(), memindex_list.end());
    //            }
    //        }
    //    }
    return mem_indexes;
}

Vector<BaseMemIndex *> BGMemIndexTracer::GetAllMemIndexes(NewTxn *new_txn) {
    Vector<SharedPtr<MemIndex>> mem_indexes;
    Vector<MemIndexID> mem_index_ids;
    Status status = NewCatalog::GetAllMemIndexes(new_txn, mem_indexes, mem_index_ids);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    Vector<BaseMemIndex *> base_mem_indexes;
    // for (auto &mem_index : mem_indexes) {
    for (SizeT i = 0; i < mem_indexes.size(); ++i) {
        auto &mem_index = mem_indexes[i];
        auto &mem_index_id = mem_index_ids[i];
        BaseMemIndex *base_mem_index = mem_index->GetBaseMemIndex(mem_index_id);
        EMVBIndexInMem *emvb_index = mem_index->GetEMVBMemIndex(mem_index_id);
        if (base_mem_index != nullptr) {
            base_mem_indexes.emplace_back(base_mem_index);
        } else if (emvb_index != nullptr) {
        } else {
            LOG_WARN(fmt::format("Not implement base index of index {}.{}.{}.{}",
                                 mem_index_id.db_name_,
                                 mem_index_id.table_name_,
                                 mem_index_id.index_name_,
                                 mem_index_id.segment_id_));
        }
    }
    return base_mem_indexes;
}

Vector<EMVBIndexInMem *> BGMemIndexTracer::GetEMVBMemIndexes(NewTxn *new_txn) {
    Vector<SharedPtr<MemIndex>> mem_indexes;
    Vector<MemIndexID> mem_index_ids;
    Status status = NewCatalog::GetAllMemIndexes(new_txn, mem_indexes, mem_index_ids);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    Vector<EMVBIndexInMem *> emvb_indexes;
    for (SizeT i = 0; i < mem_indexes.size(); ++i) {
        auto &mem_index = mem_indexes[i];
        auto &mem_index_id = mem_index_ids[i];
        BaseMemIndex *base_mem_index = mem_index->GetBaseMemIndex(mem_index_id);
        EMVBIndexInMem *emvb_index = mem_index->GetEMVBMemIndex(mem_index_id);

        if (emvb_index != nullptr) {
            emvb_indexes.emplace_back(emvb_index);
        } else if (base_mem_index != nullptr) {
        } else {
            LOG_WARN(fmt::format("Not implement base index of index {}.{}.{}.{}",
                                 mem_index_id.db_name_,
                                 mem_index_id.table_name_,
                                 mem_index_id.index_name_,
                                 mem_index_id.segment_id_));
        }
    }
    return emvb_indexes;
}

} // namespace infinity
