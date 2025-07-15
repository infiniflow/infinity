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

module infinity_core;

import :stl;
import :base_memindex;
import :emvb_index_in_mem;
import :bg_task;
import :infinity_context;
import :infinity_exception;
import :logger;
import :third_party;
import :txn_state;
import :dump_index_process;
import :storage;

import :kv_store;
import :new_catalog;
import :new_txn_manager;
import :mem_index;
import :new_txn;
import :status;
import :defer_op;
import row_id;
import global_resource_usage;

namespace infinity {

void MemIndexTracer::InitMemUsed() {
    SizeT cur_index_memory = 0;
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<NewTxn> new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("Init mem index tracer"), TransactionType::kNormal);
    Vector<SharedPtr<MemIndexDetail>> mem_index_details = GetAllMemIndexes(new_txn_shared.get());
    for (auto &mem_index_detail : mem_index_details) {
        if (mem_index_detail->is_emvb_index_) {
            continue;
        }
        cur_index_memory += mem_index_detail->mem_used_;
    }
    Status status = new_txn_mgr->CommitTxn(new_txn_shared.get());
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    std::lock_guard lck(mtx_);
    cur_index_memory_ = cur_index_memory;
}

void MemIndexTracer::DecreaseMemUsed(SizeT mem_dec) {
    std::lock_guard lck(mtx_);
    LOG_TRACE(fmt::format("DecreaseMemUsed mem_dec {}, cur_index_memory_ {}", mem_dec, cur_index_memory_));
    if (cur_index_memory_ >= mem_dec) {
        cur_index_memory_ -= mem_dec;
    } else {
        LOG_ERROR(fmt::format("DecreaseMemUsed mem_dec {} is larger than cur_index_memory_ {}", mem_dec, cur_index_memory_));
        cur_index_memory_ = 0;
    }
}

bool MemIndexTracer::TryTriggerDump() {
    Vector<SharedPtr<DumpMemIndexTask>> dump_tasks = MakeDumpTask();
    if (dump_tasks.empty()) {
        return false;
    }
    LOG_TRACE(fmt::format("Dump triggered!"));
    for (auto &dump_task : dump_tasks) {
        TriggerDump(std::move(dump_task));
    }
    dump_tasks.clear();
    return true;
}

void MemIndexTracer::DumpDone(SharedPtr<MemIndex> mem_index) {
    std::lock_guard lck(mtx_);
    auto iter = proposed_dump_.find(mem_index);
    if (iter == proposed_dump_.end()) {
        return;
    }
    SizeT dump_size = iter->second;
    proposed_dump_.erase(iter);
    if (proposed_dump_size_ >= dump_size) {
        proposed_dump_size_ -= dump_size;
        if (proposed_dump_.empty()) {
            proposed_dump_size_ = 0;
        }
    } else {
        proposed_dump_size_ = 0;
    }
}

Vector<SharedPtr<DumpMemIndexTask>> MemIndexTracer::MakeDumpTask() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    SharedPtr<NewTxn> new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("Dump index"), TransactionType::kNormal);

    Vector<SharedPtr<MemIndexDetail>> mem_index_details = GetAllMemIndexes(new_txn_shared.get());
    // Generate dump task for all EMVB index and at most one non-EMVB index
    Vector<SharedPtr<DumpMemIndexTask>> dump_tasks;
    for (auto &mem_index_detail : mem_index_details) {
        {
            std::lock_guard lck(mtx_);
            // Skip index that is already in proposed dump
            if (proposed_dump_.find(mem_index_detail->mem_index_) != proposed_dump_.end()) {
                continue;
            }
            // Record index in proposed dump
            proposed_dump_[mem_index_detail->mem_index_] = mem_index_detail->mem_used_;
            proposed_dump_size_ += mem_index_detail->mem_used_;
        }

        auto dump_task = MakeShared<DumpMemIndexTask>(mem_index_detail->db_name_,
                                                      mem_index_detail->table_name_,
                                                      mem_index_detail->index_name_,
                                                      mem_index_detail->segment_id_,
                                                      mem_index_detail->begin_row_id_);
        dump_tasks.push_back(std::move(dump_task));
        if (!mem_index_detail->is_emvb_index_) {
            break;
        }
    }
    Status status = new_txn_mgr->CommitTxn(new_txn_shared.get());
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return dump_tasks;
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

void BGMemIndexTracer::TriggerDump(SharedPtr<DumpMemIndexTask> dump_task) {
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

Vector<SharedPtr<MemIndexDetail>> BGMemIndexTracer::GetAllMemIndexes(NewTxn *new_txn) {
    Vector<SharedPtr<MemIndexDetail>> mem_index_details;
    Vector<SharedPtr<MemIndex>> mem_indexes;
    Vector<MemIndexID> mem_index_ids;
    Status status = NewCatalog::GetAllMemIndexes(new_txn, mem_indexes, mem_index_ids);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    for (SizeT i = 0; i < mem_indexes.size(); ++i) {
        SharedPtr<MemIndexDetail> detail = MakeShared<MemIndexDetail>();
        auto &mem_index = mem_indexes[i];
        auto &mem_index_id = mem_index_ids[i];
        const BaseMemIndex *base_mem_index = mem_index->GetBaseMemIndex();
        detail->mem_index_ = mem_index;
        detail->is_emvb_index_ = base_mem_index == nullptr;
        detail->db_name_ = mem_index_id.db_name_;
        detail->table_name_ = mem_index_id.table_name_;
        detail->index_name_ = mem_index_id.index_name_;
        detail->segment_id_ = mem_index_id.segment_id_;
        if (!detail->is_emvb_index_) {
            MemIndexTracerInfo info = base_mem_index->GetInfo();
            detail->mem_used_ = info.mem_used_;
            detail->row_count_ = info.row_count_;
        }
        mem_index_details.push_back(detail);
    }
    std::sort(mem_index_details.begin(), mem_index_details.end(), [](const SharedPtr<MemIndexDetail> &lhs, const SharedPtr<MemIndexDetail> &rhs) {
        return lhs->is_emvb_index_ || lhs->mem_used_ > rhs->mem_used_;
    });
    return mem_index_details;
}

} // namespace infinity
