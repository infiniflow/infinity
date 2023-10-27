//
// Created by jinhai on 23-9-8.
//

module;

import stl;
import fragment_task;

export module fragment_task_poller_queue;

namespace infinity {

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

} // namespace infinity
