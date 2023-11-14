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

import stl;
import fragment_task;
import third_party;

export module fragment_task_poller_queue;

namespace infinity {

export class FragmentTaskPollerQueue {
public:
    explicit inline FragmentTaskPollerQueue(SizeT capacity = 1024) : queue_(capacity) {}

    void Enqueue(FragmentTask *task_ptr);

    SizeT DequeueBulk(List<FragmentTask *>::iterator& output_queue_iter);

    FragmentTask* Dequeue();

    SizeT Size() const;

protected:
    BlockingConcurrentQueue<FragmentTask *> queue_;
};

/*
export class FragmentTaskPollerQueue {
public:
    explicit inline FragmentTaskPollerQueue(SizeT capacity = 1024) : capacity_(capacity) {}

    void Enqueue(FragmentTask *task_ptr) {
        UniqueLock<Mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(task_ptr);
        empty_cv_.notify_one();
    }

    void DequeueBulk(List<FragmentTask *> &output_queue) {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_queue.splice(output_queue.end(), queue_);
        full_cv_.notify_one();
    }

    SizeT Size() const {
        LockGuard<Mutex> lock(queue_mutex_);
        return queue_.size();
    }

    bool Empty() const {
        LockGuard<Mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    mutable Mutex queue_mutex_{};
    CondVar full_cv_{};
    CondVar empty_cv_{};
    List<FragmentTask *> queue_{};
    SizeT capacity_{32};
};
*/
} // namespace infinity
