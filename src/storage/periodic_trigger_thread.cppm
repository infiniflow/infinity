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
    PeriodicTriggerThread() : running_(false) {}

    ~PeriodicTriggerThread();

    void Start();

    void Stop();

    void Run();

public:
    SharedPtr<CleanupPeriodicTrigger> cleanup_trigger_;
    SharedPtr<CheckpointPeriodicTrigger> full_checkpoint_trigger_;
    SharedPtr<CheckpointPeriodicTrigger> delta_checkpoint_trigger_;
    SharedPtr<CompactSegmentPeriodicTrigger> compact_segment_trigger_;
    SharedPtr<OptimizeIndexPeriodicTrigger> optimize_index_trigger_;

private:
    Thread thread_{};
    std::mutex mtx_{};
    std::condition_variable cv_{};
    bool running_{false};
};

} // namespace infinity