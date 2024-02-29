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
import cleanup_task;
import catalog;
import txn_manager;
import third_party;
import txn;

namespace infinity {

void CleanupPeriodicTrigger::Trigger() {
    TxnTimeStamp visible_ts = txn_mgr_->GetMinUncommitTs();
    if (visible_ts == last_visible_ts_) {
        LOG_INFO(fmt::format("No need to cleanup visible timestamp: {}", visible_ts));
        return;
    } else if (visible_ts < last_visible_ts_) {
        UnrecoverableException("The visible timestamp is not monotonic.");
        return;
    }
    last_visible_ts_ = visible_ts;
    LOG_INFO(fmt::format("Cleanup visible timestamp: {}", visible_ts));
    bg_processor_->Submit(MakeShared<CleanupTask>(catalog_, visible_ts));
}

} // namespace infinity