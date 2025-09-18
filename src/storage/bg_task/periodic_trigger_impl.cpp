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

module infinity_core:periodic_trigger.impl;

import :periodic_trigger;
import :logger;
import :infinity_exception;
import :background_process;
import :compaction_process;
import :bg_task;
import :new_txn_manager;
import :infinity_context;

import std;
import third_party;

namespace infinity {

bool PeriodicTrigger::Check() {
    if (interval_ == 0) {
        return false;
    }
    const auto now = std::chrono::system_clock::now();
    const auto duration_s = std::chrono::duration_cast<seconds>(now - last_check_);
    const auto duration_tick_cnt = duration_s.count();
    duration_.store(duration_tick_cnt);
    if (duration_tick_cnt < static_cast<long>(interval_)) {
        return false;
    }
    last_check_ = now;
    return true;
}

std::shared_ptr<NewCleanupTask> NewCleanupPeriodicTrigger::CreateNewCleanupTask() {
    auto *bg_processor = InfinityContext::instance().storage()->bg_processor();
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();

    auto last_cleanup_ts = bg_processor->last_cleanup_ts();
    auto cur_cleanup_ts = new_txn_mgr->GetOldestAliveTS();
    if (cur_cleanup_ts <= last_cleanup_ts) {
        return nullptr;
    }

    return std::make_shared<NewCleanupTask>();
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
    auto last_ckp_ts = wal_manager->LastCheckpointTS();
    auto cur_ckp_ts = new_txn_mgr->CurrentTS() + 1;
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
    auto checkpoint_task = std::make_shared<NewCheckpointTask>(wal_size);
    bg_processor->Submit(std::move(checkpoint_task));
}

void CompactSegmentPeriodicTrigger::Trigger() {
    LOG_DEBUG(fmt::format("Trigger compact segment task, after {} seconds", duration_.load()));
    auto compact_task = std::make_shared<NotifyCompactTask>();
    compact_processor_->Submit(std::move(compact_task));
}

void OptimizeIndexPeriodicTrigger::Trigger() {
    LOG_DEBUG(fmt::format("Trigger optimize index task, after {} seconds", duration_.load()));
    auto optimize_task = std::make_shared<NotifyOptimizeTask>(true);
    compact_processor_->Submit(std::move(optimize_task));
}

} // namespace infinity