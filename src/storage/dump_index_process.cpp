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
import status;
import wal_manager;
import global_resource_usage;

import new_txn_manager;
import new_txn;
import column_vector;
import mem_index;
import memory_indexer;
import txn_state;

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

    NewTxn *new_txn = dump_task->new_txn_;
    const String &db_name = dump_task->mem_index_->db_name_;
    const String &table_name = dump_task->mem_index_->table_name_;
    const String &index_name = dump_task->mem_index_->index_name_;
    SegmentID segment_id = dump_task->mem_index_->segment_id_;

    Status commit_status = Status::OK();
    Status rollback_status = Status::OK();
    i64 retry_count = 0;
    do {
        if (!commit_status.ok()) {
            new_txn = new_txn_mgr->BeginTxn(MakeUnique<String>("dump mem index"), TransactionType::kNormal);
        }

        Status status = new_txn->DumpMemIndex(db_name, table_name, index_name, segment_id);
        if (status.ok()) {
            commit_status = new_txn_mgr->CommitTxn(new_txn);
            if (!commit_status.ok()) {
                LOG_ERROR(fmt::format("Commit dump mem index failed: {}", commit_status.message()));
            }
        } else {
            LOG_ERROR(fmt::format("Dump mem index failed: {}", status.message()));
            Status rollback_status = new_txn_mgr->RollBackTxn(new_txn);
            if (!rollback_status.ok()) {
                UnrecoverableError(rollback_status.message());
            }
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
