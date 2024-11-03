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

import blocking_queue;
import stl;
import object_storage_task;

export module object_storage_process;

namespace infinity {

export class ObjectStorageProcess {
public:
    explicit ObjectStorageProcess() = default;

    void Start();
    void Stop();
    u64 RunningTaskCount() const { return task_count_; }

public:
    void Submit(SharedPtr<BaseObjectStorageTask> object_storage_task);

private:
    void Process();

private:
    BlockingQueue<SharedPtr<BaseObjectStorageTask>> task_queue_{"ObjectStorageProcess"};

    Thread processor_thread_{};

    Atomic<u64> task_count_{};

    mutable std::mutex task_mutex_;
    String task_text_;
};

} // namespace infinity
