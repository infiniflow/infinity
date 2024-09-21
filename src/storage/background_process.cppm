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

import wal_manager;
import blocking_queue;
import bg_task;
import stl;

export module background_process;

namespace infinity {

class Catalog;
class CleanupPeriodicTrigger;

export class BGTaskProcessor {
public:
    explicit BGTaskProcessor(WalManager *wal_manager, Catalog *catalog) : wal_manager_(wal_manager), catalog_(catalog) {}

    // cleanup is used before full checkpoint
    void SetCleanupTrigger(SharedPtr<CleanupPeriodicTrigger> cleanup_trigger);

    void Start();
    void Stop();

public:
    void Submit(SharedPtr<BGTask> bg_task);
    u64 RunningTaskCount() const { return task_count_; }
    String RunningTaskText() const {
        std::unique_lock<std::mutex> locker(task_mutex_);
        return task_text_;
    }

    CleanupPeriodicTrigger *cleanup_trigger() const { return cleanup_trigger_.get(); }

private:
    void Process();

private:
    BlockingQueue<SharedPtr<BGTask>> task_queue_;

    Thread processor_thread_{};

    WalManager *wal_manager_{};
    Catalog *catalog_{};
    SharedPtr<CleanupPeriodicTrigger> cleanup_trigger_;

    Atomic<u64> task_count_{};

    mutable std::mutex task_mutex_;
    String task_text_;
};

} // namespace infinity
