// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include <thread>

module dump_index_process;

import stl;
import bg_task;
import logger;
import infinity_exception;
import third_party;
import blocking_queue;
import infinity_context;
import base_memindex;
import emvb_index_in_mem;
import status;
import wal_manager;
import global_resource_usage;

import new_txn_manager;
import new_txn;
import column_vector;
import mem_index;
import memory_indexer;
import txn_state;
import base_txn_store;

namespace infinity {

DumpIndexProcessor::DumpIndexProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("DumpIndexProcessor");
#endif
}

DumpIndexProcessor::~DumpIndexProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("DumpIndexProcessor");
#endif
}

void DumpIndexProcessor::Start() {
    LOG_INFO("Compaction processor is started.");
    processor_thread_ = Thread([this] { Process(); });
}

void DumpIndexProcessor::Stop() {
    LOG_INFO("Compaction processor is stopping.");
    SharedPtr<StopProcessorTask> stop_task = MakeShared<StopProcessorTask>();
    this->Submit(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Compaction processor is stopped.");
}

void DumpIndexProcessor::Submit(SharedPtr<BGTask> bg_task) {
    task_queue_.Enqueue(std::move(bg_task));
    ++task_count_;
}

void DumpIndexProcessor::DoDump(DumpIndexTask *dump_task) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    SharedPtr<NewTxn> new_txn_shared = dump_task->new_txn_shared_;
    String db_name{};
    String table_name{};
    String index_name{};
    SegmentID segment_id{};
    if (dump_task->mem_index_ != nullptr) {
        db_name = dump_task->mem_index_->db_name_;
        table_name = dump_task->mem_index_->table_name_;
        index_name = dump_task->mem_index_->index_name_;
        segment_id = dump_task->mem_index_->segment_id_;
    } else if (dump_task->emvb_mem_index_ != nullptr) {
        db_name = dump_task->emvb_mem_index_->db_name_;
        table_name = dump_task->emvb_mem_index_->table_name_;
        index_name = dump_task->emvb_mem_index_->index_name_;
        segment_id = dump_task->emvb_mem_index_->segment_id_;
    } else {
        UnrecoverableError("Invalid mem index");
    }

    Status commit_status = Status::OK();
    Status rollback_status = Status::OK();
    i64 retry_count = 0;
    do {
        SharedPtr<BGTaskInfo> bg_task_info = MakeShared<BGTaskInfo>(BGTaskType::kDumpIndex);

        if (!commit_status.ok()) {
            new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("Dump index"), TransactionType::kNormal);
        }

        Status status = new_txn_shared->DumpMemIndex(db_name, table_name, index_name, segment_id);
        if (status.ok()) {
            commit_status = new_txn_mgr->CommitTxn(new_txn_shared.get());
            if (!commit_status.ok()) {
                LOG_ERROR(fmt::format("Commit dump mem index {}.{}.{} in segment: failed: {}",
                                      db_name,
                                      table_name,
                                      index_name,
                                      segment_id,
                                      status.message()));
            }

            DumpMemIndexTxnStore *dump_index_txn_store = static_cast<DumpMemIndexTxnStore *>(new_txn_shared->GetTxnStore());
            if (dump_index_txn_store != nullptr) {
                String task_text = fmt::format("Txn: {}, commit: {}, dump mem index: {}.{}.{} in segment: {} into chunk:{}",
                                               new_txn_shared->TxnID(),
                                               new_txn_shared->CommitTS(),
                                               db_name,
                                               table_name,
                                               dump_index_txn_store->index_name_,
                                               dump_index_txn_store->segment_ids_[0],
                                               dump_index_txn_store->chunk_infos_in_segments_[dump_index_txn_store->segment_ids_[0]][0].chunk_id_);
                bg_task_info->task_info_list_.emplace_back(task_text);
                if (commit_status.ok()) {
                    bg_task_info->status_list_.emplace_back("OK");
                } else {
                    bg_task_info->status_list_.emplace_back(commit_status.message());
                }
            }
        } else {
            LOG_ERROR(fmt::format("Dump mem index {}.{}.{} in segment: failed: {}", db_name, table_name, index_name, segment_id, status.message()));
            Status rollback_status = new_txn_mgr->RollBackTxn(new_txn_shared.get());
            if (!rollback_status.ok()) {
                UnrecoverableError(rollback_status.message());
            }

            String task_text = fmt::format("Dump mem index: {}.{}.{} in segment: {}", db_name, table_name, index_name, segment_id);
            bg_task_info->task_info_list_.emplace_back(task_text);
            bg_task_info->status_list_.emplace_back(status.message());
        }

        if (!bg_task_info->task_info_list_.empty()) {
            new_txn_mgr->AddTaskInfo(bg_task_info);
        }
    } while (!commit_status.ok() && rollback_status.ok() && (commit_status.code_ == ErrorCode::kTxnConflict || ++retry_count <= 3));
}

void DumpIndexProcessor::Process() {
    bool running = true;
    while (running) {
        Deque<SharedPtr<BGTask>> tasks;
        task_queue_.DequeueBulk(tasks);

        for (const auto &bg_task : tasks) {
            switch (bg_task->type_) {
                case BGTaskType::kStopProcessor: {
                    running = false;
                    break;
                }
                case BGTaskType::kDumpIndex: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        auto dump_task = static_cast<DumpIndexTask *>(bg_task.get());
                        LOG_DEBUG(dump_task->ToString());
                        // Trigger transaction to save the mem index
                        DoDump(dump_task);
                        LOG_DEBUG("Dump index done.");
                    }
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid background task: {}", (u8)bg_task->type_);
                    UnrecoverableError(error_message);
                    break;
                }
            }
            bg_task->Complete();
        }
        task_count_ -= tasks.size();
        tasks.clear();
    }
}

} // namespace infinity
