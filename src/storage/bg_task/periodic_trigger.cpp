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

module periodic_trigger;

import stl;
import logger;
import infinity_exception;
import background_process;
import bg_task;
import catalog;
import txn_manager;
import third_party;

namespace infinity {

SharedPtr<CleanupTask> CleanupPeriodicTrigger::CreateCleanupTask() {
    TxnTimeStamp visible_ts = txn_mgr_->GetCleanupScanTS();
    {
        std::lock_guard lck(mtx_);
        if (visible_ts == last_visible_ts_) {
            LOG_TRACE(fmt::format("Skip cleanup. visible timestamp: {}", visible_ts));
            return nullptr;
        }
        if (visible_ts < last_visible_ts_) {
            UnrecoverableError("The visible timestamp is not monotonic.");
            return nullptr;
        }
        last_visible_ts_ = visible_ts;
        LOG_DEBUG(fmt::format("Cleanup visible timestamp: {}", visible_ts));
    }

    auto buffer_mgr = txn_mgr_->GetBufferMgr();
    return MakeShared<CleanupTask>(catalog_, visible_ts, buffer_mgr);
}

void CleanupPeriodicTrigger::Trigger() {
    auto cleanup_task = CreateCleanupTask();
    if (cleanup_task.get() == nullptr) {
        return;
    }
    bg_processor_->Submit(std::move(cleanup_task));
}

void CheckpointPeriodicTrigger::Trigger() {
    auto checkpoint_task = MakeShared<CheckpointTask>(is_full_checkpoint_);
    // LOG_DEBUG(fmt::format("Trigger {} periodic checkpoint.", is_full_checkpoint_ ? "FULL" : "DELTA"));
    if (!wal_mgr_->TrySubmitCheckpointTask(std::move(checkpoint_task))) {
        LOG_TRACE(
            fmt::format("Skip {} checkpoint(time) because there is already a checkpoint task running.", is_full_checkpoint_ ? "FULL" : "DELTA"));
    }
}

void CompactSegmentPeriodicTrigger::Trigger() {
    auto compact_task = MakeShared<NotifyCompactTask>();
    compact_processor_->Submit(std::move(compact_task));
}

void OptimizeIndexPeriodicTrigger::Trigger() {
    auto optimize_task = MakeShared<NotifyOptimizeTask>();
    compact_processor_->Submit(std::move(optimize_task));
}

} // namespace infinity