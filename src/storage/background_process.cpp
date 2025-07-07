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

#include <tuple>
#include <unistd.h>

module infinity_core;

import :stl;
import :bg_task;
import :update_segment_bloom_filter_task;
import :logger;
import :blocking_queue;
import :infinity_exception;
import :third_party;
import :infinity_context;
import :status;
import :new_txn;
import :bg_task_type;
import global_resource_usage;
import :wal_manager;
import :new_txn_manager;
import :base_txn_store;
import :txn_state;

namespace infinity {

BGTaskProcessor::BGTaskProcessor() = default;
BGTaskProcessor::~BGTaskProcessor() = default;

void BGTaskProcessor::Start() {
    processor_thread_ = Thread([this] { Process(); });
    LOG_INFO("Background processor is started.");
}

void BGTaskProcessor::Stop() {
    LOG_INFO("Background processor is stopping.");
    SharedPtr<StopProcessorTask> stop_task = MakeShared<StopProcessorTask>();
    task_queue_.Enqueue(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Background processor is stopped.");
}

void BGTaskProcessor::Submit(SharedPtr<BGTask> bg_task) {
    ++task_count_;
    task_queue_.Enqueue(std::move(bg_task));
}

void BGTaskProcessor::Process() {
    bool running{true};
    Deque<SharedPtr<BGTask>> tasks;
    while (running) {
        task_queue_.DequeueBulk(tasks);

        for (const auto &bg_task : tasks) {
            switch (bg_task->type_) {
                case BGTaskType::kStopProcessor: {
                    LOG_INFO("Stop the background processor");
                    {
                        std::unique_lock<std::mutex> locker(task_mutex_);
                        task_text_ = bg_task->ToString();
                    }
                    running = false;
                    break;
                }
                case BGTaskType::kNewCheckpoint: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Checkpoint in background");

                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = bg_task->ToString();
                        }

                        Status status = Status::OK();
                        auto *checkpoint_task = static_cast<NewCheckpointTask *>(bg_task.get());
                        if (checkpoint_task->new_txn_ == nullptr) {
                            status = checkpoint_task->ExecuteWithNewTxn();
                        } else {
                            status = checkpoint_task->ExecuteWithinTxn();
                        }
                        if (!status.ok()) {
                            UnrecoverableError(fmt::format("Fail to checkpoint: {}", status.message()));
                        }
                        LOG_DEBUG("Checkpoint in background done");
                    }
                    break;
                }
                case BGTaskType::kNewCleanup: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable or storage_mode == StorageMode::kReadable) {
                        auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
                        auto new_txn_shared = new_txn_mgr->BeginTxnShared(MakeUnique<String>("clean up"), TransactionType::kCleanup);
                        Status status = new_txn_shared->Cleanup();
                        if (!status.ok()) {
                            UnrecoverableError(status.message());
                        }
                        status = new_txn_mgr->CommitTxn(new_txn_shared.get());
                        if (!status.ok()) {
                            UnrecoverableError(status.message());
                        }

                        CleanupTxnStore *cleanup_txn_store = static_cast<CleanupTxnStore *>(new_txn_shared->GetTxnStore());
                        if(cleanup_txn_store != nullptr) {
                            TxnTimeStamp clean_ts = cleanup_txn_store->timestamp_;
                            SharedPtr<BGTaskInfo> bg_task_info = MakeShared<BGTaskInfo>(BGTaskType::kNewCleanup);
                            String task_text = fmt::format("NewCleanup task, cleanup timestamp: {}", clean_ts);
                            bg_task_info->task_info_list_.emplace_back(task_text);
                            if (status.ok()) {
                                bg_task_info->status_list_.emplace_back("OK");
                            } else {
                                RecoverableError(status);
                                bg_task_info->status_list_.emplace_back(status.message());
                            }
                            new_txn_mgr->AddTaskInfo(bg_task_info);
                        }
                        LOG_DEBUG("NewCleanup task in background done");
                    }
                    break;
                }
                case BGTaskType::kUpdateSegmentBloomFilterData: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable or storage_mode == StorageMode::kReadable) {
                        LOG_DEBUG("Update segment bloom filter");
                        auto *task = static_cast<UpdateSegmentBloomFilterTask *>(bg_task.get());
                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = task->ToString();
                        }
                        task->Execute();
                        LOG_DEBUG("Update segment bloom filter done");
                    }
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid background task: {}", (u8)bg_task->type_);
                    UnrecoverableError(error_message);
                    break;
                }
            }
            task_text_.clear();
            bg_task->Complete();
        }
        task_count_ -= tasks.size();
        tasks.clear();
    }
}

} // namespace infinity
