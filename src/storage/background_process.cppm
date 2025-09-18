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

export module infinity_core:background_process;

import :blocking_queue;

namespace infinity {

class BGTask;

export class BGTaskProcessor {
public:
    BGTaskProcessor();
    virtual ~BGTaskProcessor();

    void Start();
    void Stop();

public:
    void Submit(std::shared_ptr<BGTask> bg_task);
    u64 RunningTaskCount() const { return task_count_; }
    std::string RunningTaskText() const {
        std::unique_lock<std::mutex> locker(task_mutex_);
        return task_text_;
    }

private:
    void Process();

private:
    BlockingQueue<std::shared_ptr<BGTask>> task_queue_{"BGTaskProcessor"};

    std::thread processor_thread_{};

    std::atomic<u64> task_count_{};

    mutable std::mutex task_mutex_;
    std::string task_text_;

public:
    TxnTimeStamp last_cleanup_ts() {
        std::unique_lock lock(last_time_mtx_);
        return last_cleanup_ts_;
    }

    TxnTimeStamp last_checkpoint_ts() {
        std::unique_lock lock(last_time_mtx_);
        return last_checkpoint_ts_;
    }

private:
    std::mutex last_time_mtx_;
    TxnTimeStamp last_cleanup_ts_ = 0;
    TxnTimeStamp last_checkpoint_ts_ = 0;
};

} // namespace infinity
