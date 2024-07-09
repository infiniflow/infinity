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

export module periodic_trigger_thread;

import stl;
import periodic_trigger;

namespace infinity {

export class PeriodicTriggerThread {
public:
    PeriodicTriggerThread() : running_(true) {}

    ~PeriodicTriggerThread() {
        if (running_.load()) {
            Stop();
        }
    }

    int AddTrigger(UniquePtr<PeriodicTrigger> trigger) {
        int id = triggers_.size();
        triggers_.push_back(std::move(trigger));
        return id;
    }

    void Start();

    void Stop();

    void Run();

    void Reset(int id) { triggers_[id]->Reset(); }

private:
    Vector<UniquePtr<PeriodicTrigger>> triggers_;

    Thread thread_{};
    atomic_bool running_{};
};

} // namespace infinity