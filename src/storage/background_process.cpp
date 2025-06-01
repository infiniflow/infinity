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

module background_process;

import stl;
import bg_task;
import update_segment_bloom_filter_task;
import logger;
import blocking_queue;
import infinity_exception;
import wal_manager;
import catalog;
import third_party;
import buffer_manager;
import periodic_trigger;
import infinity_context;
import status;
import new_txn;

namespace infinity {

void BGTaskProcessor::SetCleanupTrigger(SharedPtr<CleanupPeriodicTrigger> cleanup_trigger) { cleanup_trigger_ = cleanup_trigger; }

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

                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = bg_task->ToString();
                        }

                        auto task = static_cast<NewCleanupTask *>(bg_task.get());
                        TxnTimeStamp last_cleanup_ts = this->last_cleanup_ts();
                        TxnTimeStamp cur_cleanup_ts = 0;

                        LOG_DEBUG(fmt::format("NewCleanup task in background, last_cleanup_ts: {}, current_cleanup_ts: {}",
                                              last_cleanup_ts,
                                              cur_cleanup_ts));
                        Status status = task->Execute(last_cleanup_ts, cur_cleanup_ts);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        if (cur_cleanup_ts > last_cleanup_ts) {
                            std::unique_lock lock(last_time_mtx_);
                            last_cleanup_ts_ = cur_cleanup_ts;
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
