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

module infinity_core:optimization_process.impl;

import :optimization_process;
import :bg_task;
import :logger;
import :infinity_exception;
import :blocking_queue;
import :infinity_context;
import :base_memindex;
import :status;
import :wal_manager;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :table_meta;
import :db_meta;
import :segment_meta;
import :bg_task;
import :base_txn_store;

import std;
import third_party;

import global_resource_usage;
import compilation_config;

namespace infinity {

OptimizationProcessor::OptimizationProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("OptimizationProcessor");
#endif
}

OptimizationProcessor::~OptimizationProcessor() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("OptimizationProcessor");
#endif
}

void OptimizationProcessor::Start() {
    LOG_INFO("Optimization processor is started.");
    processor_thread_ = std::thread([this] { Process(); });
}

void OptimizationProcessor::Stop() {
    LOG_INFO("Optimization processor is stopping.");
    std::shared_ptr<StopProcessorTask> stop_task = std::make_shared<StopProcessorTask>();
    this->Submit(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    // Ensure all pending tasks are processed before shutdown
    while (!task_queue_.Empty()) {
        std::deque<std::shared_ptr<BGTask>> tasks;
        task_queue_.DequeueBulk(tasks);
        for (const auto &bg_task : tasks) {
            if (bg_task->type_ != BGTaskType::kStopProcessor) {
                // Skip processing any remaining tasks during shutdown
                continue;
            }
        }
    }
    LOG_INFO("Optimization processor is stopped.");
}

void OptimizationProcessor::Submit(const std::shared_ptr<BGTask> &bg_task) {
    task_queue_.Enqueue(bg_task);
    ++task_count_;
}

Status OptimizationProcessor::NewManualOptimize(NewTxn *new_txn, const std::string &db_name, const std::string &table_name) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    new_txn->SetBeginTS(new_txn_mgr->CurrentTS() + 1);
    return new_txn->OptimizeTableIndexes(db_name, table_name);
}

void OptimizationProcessor::NewNotifyOptimize() {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto new_txn_shared = new_txn_mgr->BeginTxnShared(std::make_unique<std::string>("optimize index"), TransactionType::kOptimizeIndex);
    LOG_INFO(fmt::format("Optimize all indexes begin ts: {}", new_txn_shared->BeginTS()));

    std::shared_ptr<BGTaskInfo> bg_task_info = std::make_shared<BGTaskInfo>(BGTaskType::kNotifyOptimize);
    Status status = new_txn_shared->OptimizeAllIndexes();
    if (status.ok()) {
        Status commit_status = new_txn_mgr->CommitTxn(new_txn_shared.get());
        if (!commit_status.ok()) {
            LOG_ERROR(fmt::format("Commit optimize all indexes failed: {}", commit_status.message()));
        }

        OptimizeIndexTxnStore *optimize_idx_store = static_cast<OptimizeIndexTxnStore *>(new_txn_shared->GetTxnStore());
        if (optimize_idx_store != nullptr) {
            for (const OptimizeIndexStoreEntry &store_entry : optimize_idx_store->entries_) {
                size_t size = store_entry.new_chunk_infos_.size();
                std::vector<ChunkID> chunk_ids(size);
                for (size_t i = 0; i < size; ++i) {
                    chunk_ids[i] = store_entry.new_chunk_infos_[i].chunk_id_;
                }
                std::string task_text = fmt::format("Txn: {}, commit: {}, optimize index: {}.{}.{} segment: {} with chunks: {} into {}",
                                                    new_txn_shared->TxnID(),
                                                    new_txn_shared->CommitTS(),
                                                    store_entry.db_name_,
                                                    store_entry.table_name_,
                                                    store_entry.index_name_,
                                                    store_entry.segment_id_,
                                                    fmt::join(store_entry.deprecate_chunks_, ","),
                                                    fmt::join(chunk_ids, ","));

                LOG_DEBUG(task_text);
                bg_task_info->task_info_list_.emplace_back(task_text);
                if (commit_status.ok()) {
                    bg_task_info->status_list_.emplace_back("OK");
                } else {
                    bg_task_info->status_list_.emplace_back(commit_status.message());
                }
            }
        }
    } else {
        LOG_ERROR(fmt::format("optimize all indexes failed: {}", status.message()));
        Status rollback_status = new_txn_mgr->RollBackTxn(new_txn_shared.get());
        if (!rollback_status.ok()) {
            UnrecoverableError(rollback_status.message());
        }
        std::string task_text = "Optimize all indexes";
        bg_task_info->task_info_list_.emplace_back(task_text);
        bg_task_info->status_list_.emplace_back(status.message());
    }

    if (!bg_task_info->task_info_list_.empty()) {
        new_txn_mgr->AddTaskInfo(bg_task_info);
    }
}

void OptimizationProcessor::Process() {
    bool running = true;
    while (running) {
        std::deque<std::shared_ptr<BGTask>> tasks;
        task_queue_.DequeueBulk(tasks);

        for (const auto &bg_task : tasks) {
            switch (bg_task->type_) {
                case BGTaskType::kStopProcessor: {
                    running = false;
                    break;
                }
                case BGTaskType::kManualOptimize: {
                    // Triggered by optimize command
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Command optimize start.");
                        auto *optimize_task = static_cast<ManualOptimizeTask *>(bg_task.get());
                        optimize_task->result_status_ =
                            NewManualOptimize(optimize_task->new_txn_, optimize_task->db_name_, optimize_task->table_name_);
                        LOG_DEBUG("Command optimize end.");
                    }
                    break;
                }
                case BGTaskType::kNotifyOptimize: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Optimize start.");
                        NewNotifyOptimize();
                        LOG_DEBUG("Optimize done.");
                    }
                    break;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid background task: {}", (u8)bg_task->type_));
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
