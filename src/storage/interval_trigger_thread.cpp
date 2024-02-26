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

#include <vector>
#include <thread>

module interval_trigger_thread;

import stl;

namespace infinity {

void IntervalTriggerThread::Run() {
    while (running_.load()) {
        for (auto &trigger : triggers_) {
            if (trigger->Check()) {
                trigger->Do();
            }
        }
        // sleep for 1s
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace infinity
