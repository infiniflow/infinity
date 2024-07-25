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

void PeriodicTriggerThread::Start() {
    LOG_INFO("Periodic trigger start ...");
    running_.store(true);
    thread_ = Thread([this] { Run(); });
}

void PeriodicTriggerThread::Stop() {
    running_.store(false);
    thread_.join();
    LOG_INFO("Periodic trigger stop ...");
}

void PeriodicTriggerThread::Run() {
    while (running_.load()) {
        if(cleanup_trigger_->Check()) {
            cleanup_trigger_->Trigger();
        }

        if(full_checkpoint_trigger_->Check()) {
            full_checkpoint_trigger_->Trigger();
        }

        if(delta_checkpoint_trigger_->Check()) {
            delta_checkpoint_trigger_->Trigger();
        }

        if(compact_segment_trigger_->Check()) {
            compact_segment_trigger_->Trigger();
        }

        if(optimize_index_trigger_->Check()) {
            optimize_index_trigger_->Trigger();
        }

        // sleep for 1s
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

} // namespace infinity
