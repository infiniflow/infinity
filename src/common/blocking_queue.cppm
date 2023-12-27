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
import default_values;

export module blocking_queue;

namespace infinity {

export template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(SizeT capacity = DEFAULT_BLOCKING_QUEUE_SIZE) : capacity_(capacity) {}

    void NotAllowEnqueue() {
        allow_enqueue_ = false;
    }

    bool Enqueue(T& task) {
        if (!allow_enqueue_) {
            return false;
        }

        UniqueLock<Mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(task);
        empty_cv_.notify_one();
        return true;
    }

    bool Enqueue(T&& task) {
        if (!allow_enqueue_) {
            return false;
        }

        UniqueLock<Mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(Forward<T>(task));
        empty_cv_.notify_one();
        return true;
    }

    void EnqueueBulk(List<T> &input_queue) {
        UniqueLock<Mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [&] { return queue_.size() + input_queue.size() < capacity_; });
        queue_.splice(queue_.end(), input_queue);
        empty_cv_.notify_one();
    }

    void Dequeue(T& task) {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        task = queue_.front();
        queue_.pop_front();
        full_cv_.notify_one();
    }

    T DequeueReturn() {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        T res = Move(queue_.front());
        queue_.pop_front();
        full_cv_.notify_one();
        return res;
    }

    void DequeueBulk(List<T> &output_queue) {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_queue.splice(output_queue.end(), queue_);
        full_cv_.notify_one();
    }

    void DequeueBulk(Vector<T> &output_array) {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_array.insert(output_array.end(), queue_.begin(), queue_.end());
        queue_.clear();
        full_cv_.notify_one();
    }

    void DequeueBulk(Deque<T> &output_array) {
        UniqueLock<Mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_array = queue_;
//        output_array.insert(output_array.end(), queue_.begin(), queue_.end());
        queue_.clear();
        full_cv_.notify_one();
    }

    [[nodiscard]] SizeT Size() const {
        LockGuard<Mutex> lock(queue_mutex_);
        return queue_.size();
    }

    [[nodiscard]] bool Empty() const {
        LockGuard<Mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    atomic_bool allow_enqueue_{true};
    mutable Mutex queue_mutex_{};
    CondVar full_cv_{};
    CondVar empty_cv_{};
    Deque<T> queue_{};
    SizeT capacity_{DEFAULT_BLOCKING_QUEUE_SIZE};
};

} // namespace infinity
