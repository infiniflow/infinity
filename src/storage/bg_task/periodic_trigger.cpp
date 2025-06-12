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
import compaction_process;
import bg_task;
import txn_manager;
import third_party;

import new_txn_manager;
import infinity_context;

namespace infinity {

bool PeriodicTrigger::Check() {
    if (interval_ == 0) {
        return false;
    }
    const auto now = std::chrono::system_clock::now();
    i64 duration = (now - last_check_).count() / 1000000;
    duration_.store(duration);
    if (duration < interval_) {
        return false;
    }
    last_check_ = now;
    return true;
}

SharedPtr<NewCleanupTask> NewCleanupPeriodicTrigger::CreateNewCleanupTask() {
    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    TxnTimeStamp last_cleanup_ts = bg_processor->last_cleanup_ts();
    TxnTimeStamp cur_cleanup_ts = new_txn_mgr->GetCleanupScanTS1();
    if (cur_cleanup_ts <= last_cleanup_ts) {
        return nullptr;
    }

    return MakeShared<NewCleanupTask>();
}

void NewCleanupPeriodicTrigger::Trigger() {
    auto cleanup_task = CreateNewCleanupTask();
    if (!cleanup_task) {
        return;
    }
    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    bg_processor->Submit(std::move(cleanup_task));
}

void CheckpointPeriodicTrigger::Trigger() {
    LOG_INFO(fmt::format("Trigger periodic checkpoint, after {} seconds", duration_.load()));

    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    auto *wal_manager = InfinityContext::instance().storage()->wal_manager();
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    TxnTimeStamp last_ckp_ts = wal_manager->LastCheckpointTS();
    TxnTimeStamp cur_ckp_ts = new_txn_mgr->CurrentTS() + 1;
    if (cur_ckp_ts <= last_ckp_ts) {
        LOG_DEBUG("No write txn after last checkpoint");
        return;
    }
    if (wal_manager->IsCheckpointing()) {
        LOG_INFO("There is a running checkpoint task, skip this checkpoint triggered by periodic timer");
        return;
    }
    TxnTimeStamp max_commit_ts{};
    i64 wal_size{};
    std::tie(max_commit_ts, wal_size) = wal_manager->GetCommitState();
    auto checkpoint_task = MakeShared<NewCheckpointTask>(wal_size);
    bg_processor->Submit(std::move(checkpoint_task));
}

void CompactSegmentPeriodicTrigger::Trigger() {
    LOG_DEBUG(fmt::format("Trigger compact segment task, after {} seconds", duration_.load()));
    auto compact_task = MakeShared<NotifyCompactTask>();
    auto *compact_processor = InfinityContext::instance().storage()->compaction_processor();
    compact_processor->Submit(std::move(compact_task));
}

void OptimizeIndexPeriodicTrigger::Trigger() {
    LOG_DEBUG(fmt::format("Trigger optimize index task, after {} seconds", duration_.load()));
    if (!new_optimize_) {
        auto optimize_task = MakeShared<NotifyOptimizeTask>();
        compact_processor_->Submit(std::move(optimize_task));
    } else {
        auto optimize_task = MakeShared<NotifyOptimizeTask>(true);
        auto *compact_processor = InfinityContext::instance().storage()->compaction_processor();
        compact_processor->Submit(std::move(optimize_task));
    }
}

} // namespace infinity