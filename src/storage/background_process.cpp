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
                case BGTaskType::kForceCheckpoint: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Force checkpoint in background");
                        ForceCheckpointTask *force_ckp_task = static_cast<ForceCheckpointTask *>(bg_task.get());

                        if (force_ckp_task->txn_ == nullptr) {
                            TxnTimeStamp last_ckp_ts = this->last_checkpoint_ts();
                            TxnTimeStamp cur_ckp_ts = 0;
                            Status status = force_ckp_task->new_txn_->Checkpoint(last_ckp_ts, &cur_ckp_ts);
                            if (!status.ok()) {
                                RecoverableError(status);
                            }
                            if (cur_ckp_ts > last_ckp_ts) {
                                std::unique_lock lock(last_time_mtx_);
                                last_checkpoint_ts_ = cur_ckp_ts;
                            }
                            LOG_DEBUG("Force checkpoint in background done");
                            break;
                        }

                        if (cleanup_trigger_.get() != nullptr && force_ckp_task->is_full_checkpoint_) {
                            LOG_INFO("Do cleanup before force checkpoint");
                            auto cleanup_task = cleanup_trigger_->CreateCleanupTask(force_ckp_task->cleanup_ts_);
                            if (cleanup_task.get() != nullptr) {
                                cleanup_task->Execute();
                                LOG_DEBUG("Cleanup before force checkpoint done");
                            } else {
                                LOG_DEBUG("Skip cleanup before force checkpoint");
                            }
                        }
                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = force_ckp_task->ToString();
                        }
                        wal_manager_->Checkpoint(force_ckp_task);
                        LOG_DEBUG("Force checkpoint in background done");
                    }
                    break;
                }
                case BGTaskType::kAddDeltaEntry: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        auto *task = static_cast<AddDeltaEntryTask *>(bg_task.get());
                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = task->ToString();
                        }
                        catalog_->AddDeltaEntry(std::move(task->delta_entry_));
                    }
                    break;
                }
                case BGTaskType::kCheckpoint: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable) {
                        LOG_DEBUG("Checkpoint in background");
                        auto *task = static_cast<CheckpointTask *>(bg_task.get());
                        bool is_full_checkpoint = task->is_full_checkpoint_;
                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = task->ToString();
                        }

                        SizeT retry_count = 0;
                        while (true) {
                            bool success = true;
                            try {
                                wal_manager_->Checkpoint(is_full_checkpoint);
                            } catch (RecoverableException &e) {
                                LOG_WARN(fmt::format("Background checkpoint failed: {}", e.what()));
                                success = false;
                            }

                            if (is_full_checkpoint && !success) {
                                // Full checkpoint and meet checkpoint conflict, retry
                                ++retry_count;
                                LOG_WARN(fmt::format("Full checkpoint conflict, retry count: {}", retry_count));
                                sleep(retry_count * 1);
                                continue;
                            }
                            break;
                        }

                        LOG_DEBUG("Checkpoint in background done");
                    }
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
                        auto *checkpoint_task = static_cast<NewCheckpointTask *>(bg_task.get());
                        TxnTimeStamp last_ckp_ts = this->last_checkpoint_ts();
                        TxnTimeStamp cur_ckp_ts = 0;
                        Status status = checkpoint_task->Execute(last_ckp_ts, cur_ckp_ts);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        if (cur_ckp_ts > last_ckp_ts) {
                            std::unique_lock lock(last_time_mtx_);
                            last_checkpoint_ts_ = cur_ckp_ts;
                        }

                        LOG_DEBUG("Checkpoint in background done");
                    }
                    break;
                }
                case BGTaskType::kCleanup: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable or storage_mode == StorageMode::kReadable) {
                        LOG_DEBUG("Cleanup in background");
                        auto task = static_cast<CleanupTask *>(bg_task.get());
                        {
                            std::unique_lock<std::mutex> locker(task_mutex_);
                            task_text_ = task->ToString();
                        }
                        task->Execute();
                        LOG_DEBUG("Cleanup in background done");
                    }
                    break;
                }
                case BGTaskType::kNewCleanup: {
                    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
                    if (storage_mode == StorageMode::kUnInitialized) {
                        UnrecoverableError("Uninitialized storage mode");
                    }
                    if (storage_mode == StorageMode::kWritable or storage_mode == StorageMode::kReadable) {
                        LOG_DEBUG("NewCleanup in background");

                        auto task = static_cast<NewCleanupTask *>(bg_task.get());
                        TxnTimeStamp last_cleanup_ts = this->last_cleanup_ts();
                        TxnTimeStamp cur_cleanup_ts = 0;
                        Status status = task->Execute(last_cleanup_ts, cur_cleanup_ts);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        if (cur_cleanup_ts > last_cleanup_ts) {
                            std::unique_lock lock(last_time_mtx_);
                            last_cleanup_ts_ = cur_cleanup_ts;
                        }

                        LOG_DEBUG("NewCleanup in background done");
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
