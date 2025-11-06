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

export module infinity_core:blocking_queue;

import :default_values;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

export template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(const std::string &name, size_t capacity = DEFAULT_BLOCKING_QUEUE_SIZE) : capacity_(capacity) {
        name_ = name;
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount(fmt::format("BlockingQueue: {}", name_));
#endif
    }
    ~BlockingQueue() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount(fmt::format("BlockingQueue: {}", name_));
#endif
    }

    void NotAllowEnqueue() { allow_enqueue_ = false; }

    bool Enqueue(const T &task) {
        {
            if (!allow_enqueue_) {
                return false;
            }

            std::unique_lock<std::mutex> lock(queue_mutex_);
            full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
            queue_.push_back(task);
        }
        empty_cv_.notify_one();
        return true;
    }

    bool Enqueue(T &&task) {
        {
            if (!allow_enqueue_) {
                return false;
            }

            std::unique_lock<std::mutex> lock(queue_mutex_);
            full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
            queue_.push_back(std::forward<T>(task));
        }
        empty_cv_.notify_one();
        return true;
    }

    bool TryEnqueue(T &&task) {
        {
            if (!allow_enqueue_) {
                return false;
            }

            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (queue_.size() >= capacity_) {
                return false;
            }
            queue_.push_back(std::forward<T>(task));
        }
        empty_cv_.notify_one();
        return true;
    }

    void EnqueueBulk(std::vector<T> &input_array) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            full_cv_.wait(lock, [&] { return queue_.size() + input_array.size() < capacity_; });
            queue_.insert(queue_.end(), input_array.begin(), input_array.end());
        }
        empty_cv_.notify_one();
    }

    void EnqueueBulk(std::list<T> &input_queue) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            full_cv_.wait(lock, [&] { return queue_.size() + input_queue.size() < capacity_; });
            queue_.splice(queue_.end(), input_queue);
        }
        empty_cv_.notify_one();
    }

    void Dequeue(T &task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            empty_cv_.wait(lock, [this] { return !queue_.empty(); });
            task = queue_.front();
            queue_.pop_front();
        }
        full_cv_.notify_one();
    }

    T DequeueReturn() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        T res = std::move(queue_.front());
        queue_.pop_front();
        full_cv_.notify_one();
        return res;
    }

    void DequeueBulk(std::list<T> &output_queue) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            empty_cv_.wait(lock, [this] { return !queue_.empty(); });
            output_queue.splice(output_queue.end(), queue_);
        }
        full_cv_.notify_one();
    }

    void DequeueBulk(std::vector<T> &output_array) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            empty_cv_.wait(lock, [this] { return !queue_.empty(); });
            output_array.insert(output_array.end(), queue_.begin(), queue_.end());
            queue_.clear();
        }
        full_cv_.notify_one();
    }

    void DequeueBulk(std::deque<T> &output_array) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            empty_cv_.wait(lock, [this] { return !queue_.empty(); });
            output_array.swap(queue_);
            //            output_array.insert(output_array.end(), queue_.begin(), queue_.end());
            queue_.clear();
        }
        full_cv_.notify_one();
    }

    bool TryDequeue(T &task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (queue_.empty()) {
                return false;
            }
            task = queue_.front();
            queue_.pop_front();
        }
        full_cv_.notify_one();
        return true;
    }

    bool TryDequeueBulk(std::vector<T> &output_array) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (queue_.empty()) {
                return false;
            }
            output_array.insert(output_array.end(), queue_.begin(), queue_.end());
            queue_.clear();
        }
        full_cv_.notify_one();
        return true;
    }

    bool TryDequeueBulkWait(std::vector<T> &output_array, size_t wait_ms) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            bool ok = empty_cv_.wait_for(lock, std::chrono::milliseconds(wait_ms), [this] { return !queue_.empty(); });
            if (!ok) {
                return false;
            }
            output_array.insert(output_array.end(), queue_.begin(), queue_.end());
            queue_.clear();
        }
        full_cv_.notify_one();
        return true;
    }

    [[nodiscard]] size_t Size() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.size();
    }

    [[nodiscard]] bool Empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    std::atomic_bool allow_enqueue_{true};
    mutable std::mutex queue_mutex_{};
    std::condition_variable full_cv_{};
    std::condition_variable empty_cv_{};
    std::deque<T> queue_{};
    size_t capacity_{DEFAULT_BLOCKING_QUEUE_SIZE};
    std::string name_{};
};

} // namespace infinity
