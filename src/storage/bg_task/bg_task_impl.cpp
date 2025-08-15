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

module infinity_core:bg_task.impl;

import :bg_task;
import :base_memindex;
import :emvb_index_in_mem;
import :infinity_context;
import :storage;
import :new_txn;
import :new_txn_manager;
import :infinity_exception;
import :txn_state;
import :column_vector;
import :mem_index;
import :base_txn_store;

import std;

import row_id;

namespace infinity {

Status NewCheckpointTask::ExecuteWithinTxn() {
    TxnTimeStamp last_checkpoint_ts = InfinityContext::instance().storage()->wal_manager()->LastCheckpointTS();
    Status status = new_txn_->Checkpoint(last_checkpoint_ts);
    new_txn_->SetWalSize(wal_size_);
    return status;
}

Status NewCheckpointTask::ExecuteWithNewTxn() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto new_txn_shared = new_txn_mgr->BeginTxnShared(std::make_unique<std::string>("checkpoint"), TransactionType::kNewCheckpoint);
    if (new_txn_shared == nullptr) {
        // System is checkpointing
        return Status::OK();
    }
    new_txn_shared->SetWalSize(wal_size_);
    TxnTimeStamp last_checkpoint_ts = InfinityContext::instance().storage()->wal_manager()->LastCheckpointTS();
    Status status = new_txn_shared->Checkpoint(last_checkpoint_ts);
    if (status.ok()) {
        status = new_txn_mgr->CommitTxn(new_txn_shared.get());

        auto *ckp_idx_store = static_cast<CheckpointTxnStore *>(new_txn_shared->GetTxnStore());
        if (ckp_idx_store != nullptr) {
            std::shared_ptr<BGTaskInfo> bg_task_info = std::make_shared<BGTaskInfo>(BGTaskType::kNewCheckpoint);
            for (const std::shared_ptr<FlushDataEntry> &flush_data_entry : ckp_idx_store->entries_) {
                std::string task_text = fmt::format("Txn: {}, commit: {}, checkpoint data: {}.{}.{}.{} {}",
                                               new_txn_shared->TxnID(),
                                               new_txn_shared->CommitTS(),
                                               flush_data_entry->db_id_str_,
                                               flush_data_entry->table_id_str_,
                                               flush_data_entry->segment_id_,
                                               flush_data_entry->block_id_,
                                               flush_data_entry->to_flush_);
                bg_task_info->task_info_list_.emplace_back(task_text);
                bg_task_info->status_list_.emplace_back("OK");
            }
            new_txn_mgr->AddTaskInfo(bg_task_info);
        }
    }
    return status;
}

Status NewCleanupTask::Execute(TxnTimeStamp last_cleanup_ts, TxnTimeStamp &cur_cleanup_ts) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("cleanup"), TransactionType::kCleanup);
    Status status = txn->Cleanup();
    if (!status.ok()) {
        return status;
    }
    status = new_txn_mgr->CommitTxn(txn);
    return Status::OK();
}

NewCompactTask::NewCompactTask(NewTxn *new_txn, std::string db_name, std::string table_name)
    : BGTask(BGTaskType::kNewCompact, false), new_txn_(new_txn), db_name_(db_name), table_name_(table_name) {}

DumpMemIndexTask::DumpMemIndexTask(const std::string &db_name,
                                   const std::string &table_name,
                                   const std::string &index_name,
                                   SegmentID segment_id,
                                   RowID begin_row_id)
    : BGTask(BGTaskType::kDumpMemIndex, true), db_name_(db_name), table_name_(table_name), index_name_(index_name), segment_id_(segment_id),
      begin_row_id_(begin_row_id) {}

AppendMemIndexTask::AppendMemIndexTask(const std::shared_ptr<MemIndex> &mem_index,
                                       const std::shared_ptr<ColumnVector> &input_column,
                                       BlockOffset offset,
                                       BlockOffset row_cnt)
    : BGTask(BGTaskType::kAppendMemIndex, false), mem_index_(mem_index), input_column_(input_column), offset_(offset), row_cnt_(row_cnt) {}

void AppendMemIndexBatch::InsertTask(AppendMemIndexTask *task) {
    append_tasks_.emplace_back(task);
    std::unique_lock lock(mtx_);
    ++task_count_;
}

void AppendMemIndexBatch::WaitForCompletion() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return task_count_ == 0; });
}

TestCommandTask::TestCommandTask(std::string command_content) : BGTask(BGTaskType::kTestCommand, true), command_content_(std::move(command_content)) {}

BGTaskInfo::BGTaskInfo(BGTaskType type) : type_(type), task_time_(std::chrono::system_clock::now()) {}

} // namespace infinity