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

module background_process;

import stl;
import bg_task;
import compact_segments_task;
import update_segment_bloom_filter_task;
import logger;
import blocking_queue;
import infinity_exception;
import wal_manager;
import catalog;
import third_party;

namespace infinity {

BGTaskProcessor::BGTaskProcessor(WalManager *wal_manager, Catalog *catalog) : wal_manager_(wal_manager), catalog_(catalog) {}

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

void BGTaskProcessor::Submit(SharedPtr<BGTask> bg_task) { task_queue_.Enqueue(std::move(bg_task)); }

void BGTaskProcessor::Process() {
    bool running{true};
    while (running) {
        SharedPtr<BGTask> bg_task = task_queue_.DequeueReturn();

        LOG_INFO(fmt::format("Processing background task: {}.", BGTaskTypeToString(bg_task->type_)));
        switch (bg_task->type_) {
            case BGTaskType::kStopProcessor: {
                running = false;
                break;
            }
            case BGTaskType::kForceCheckpoint: {
                ForceCheckpointTask *force_ckp_task = static_cast<ForceCheckpointTask *>(bg_task.get());
                auto [max_commit_ts, wal_size] = catalog_->GetCheckpointState();
                wal_manager_->Checkpoint(force_ckp_task, max_commit_ts, wal_size);
                break;
            }
            case BGTaskType::kAddDeltaEntry: {
                auto *task = static_cast<AddDeltaEntryTask *>(bg_task.get());
                auto &delta_entry = task->delta_entry_;
                catalog_->AddDeltaEntry(std::move(delta_entry), task->wal_size_);
                break;
            }
            case BGTaskType::kCheckpoint: {
                auto *task = static_cast<CheckpointTask *>(bg_task.get());
                bool is_full_checkpoint = task->is_full_checkpoint_;
                auto [max_commit_ts, wal_size] = catalog_->GetCheckpointState();
                wal_manager_->Checkpoint(is_full_checkpoint, max_commit_ts, wal_size);
                break;
            }
            case BGTaskType::kCompactSegments: {
                auto *task = static_cast<CompactSegmentsTask *>(bg_task.get());
                task->BeginTxn();
                task->Execute();
                task->CommitTxn();
                break;
            }
            case BGTaskType::kCleanup: {
                auto task = static_cast<CleanupTask *>(bg_task.get());
                task->Execute();
                break;
            }
            case BGTaskType::kUpdateSegmentBloomFilterData: {
                auto *task = static_cast<UpdateSegmentBloomFilterTask *>(bg_task.get());
                task->Execute();
                break;
            }
            default: {
                UnrecoverableError("Invalid background task");
                break;
            }
        }

        bg_task->Complete();
        LOG_INFO(fmt::format("Processing background task DONE: {}.", BGTaskTypeToString(bg_task->type_)));
    }
}

} // namespace infinity
