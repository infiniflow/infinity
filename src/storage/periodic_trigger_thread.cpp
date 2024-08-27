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

#include <thread>
#include <vector>

module periodic_trigger_thread;

import stl;
import logger;

namespace infinity {

PeriodicTriggerThread::~PeriodicTriggerThread() {
    if (running_) {
        Stop();
    }
}

void PeriodicTriggerThread::Start() {
    LOG_INFO("Periodic trigger start ...");
    running_ = true;
    thread_ = Thread([this] { Run(); });
}

void PeriodicTriggerThread::Stop() {
    {
        std::lock_guard lock(mtx_);
        running_ = false;
        cv_.notify_all();
    }
    thread_.join();
    LOG_INFO("Periodic trigger stop ...");
}

void PeriodicTriggerThread::Run() {
    auto sleep_time = std::chrono::milliseconds(1000);
    while (true) {
        std::unique_lock lock(mtx_);
        cv_.wait_for(lock, sleep_time, [this] { return !running_; });
        if (!running_) {
            break;
        }
        if (cleanup_trigger_->Check()) {
            cleanup_trigger_->Trigger();
        }

        if (full_checkpoint_trigger_->Check()) {
            full_checkpoint_trigger_->Trigger();
        }

        if (delta_checkpoint_trigger_->Check()) {
            delta_checkpoint_trigger_->Trigger();
        }

        if (compact_segment_trigger_->Check()) {
            compact_segment_trigger_->Trigger();
        }

        if (optimize_index_trigger_->Check()) {
            optimize_index_trigger_->Trigger();
        }
    }
}

} // namespace infinity
