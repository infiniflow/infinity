// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:dump_index_process;

import :bg_task_type;
import :blocking_queue;
import :status;

namespace infinity {

class NewTxn;
class BGTask;
class DumpMemIndexTask;

export class DumpIndexProcessor {
public:
    DumpIndexProcessor();
    ~DumpIndexProcessor();

    void Start();

    void Stop();

    void Submit(std::shared_ptr<BGTask> bg_task);

    u64 RunningTaskCount() const { return task_count_; }

private:
    void DoDump(DumpMemIndexTask *dump_task);

    void Process();

private:
    // Waits out a backoff delay in small slices, checking stopping_ between
    // slices: the stop task sits in the queue behind a sleeping DoDump and
    // cannot preempt it, so only the sleeper itself can keep Stop() prompt.
    void WaitBackoff(i64 delay_ms);

private:
    BlockingQueue<std::shared_ptr<BGTask>> task_queue_{"DumpIndexProcessor", 1024 * 1024};

    std::thread processor_thread_{};

    std::atomic<u64> task_count_{};

    std::atomic<bool> stopping_{false};
};

// Exponential backoff between dump attempts, in milliseconds: 0.5s doubling
// per failure, capped at 30s. failed_attempts is the number of attempts
// already made, so the delay before the first retry is 500ms; values <= 0
// yield 0. Extracted as a function so the schedule is unit-testable.
export i64 DumpBackoffDelayMs(i64 failed_attempts);

} // namespace infinity