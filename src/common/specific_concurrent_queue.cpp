module;

#include "concurrentqueue.h"

module specific_concurrent_queue;

import stl;
import buffer_obj;

namespace infinity {

template <>
void SpecificConcurrentQueue<FloatDistHeap>::Enqueue(const FloatDistHeap &item) {
    queue_.enqueue(item);
}

template <>
void SpecificConcurrentQueue<FloatDistHeap>::Enqueue(FloatDistHeap &&item) {
    queue_.enqueue(std::move(item));
}

template <>
bool SpecificConcurrentQueue<FloatDistHeap>::TryDequeue(FloatDistHeap &item) {
    return queue_.try_dequeue(item);
}

template <>
void SpecificConcurrentQueue<BufferObj *>::Enqueue(BufferObj *const &item) {
    queue_.enqueue(item);
}

template <>
void SpecificConcurrentQueue<BufferObj *>::Enqueue(BufferObj *&&item) {
    queue_.enqueue(std::move(item));
}

template <>
bool SpecificConcurrentQueue<BufferObj *>::TryDequeue(BufferObj *&item) {
    return queue_.try_dequeue(item);
}

template <>
void SpecificConcurrentQueue<Vector<bool>>::Enqueue(const Vector<bool> &item) {
    queue_.enqueue(item);
}

template <>
void SpecificConcurrentQueue<Vector<bool>>::Enqueue(Vector<bool> &&item) {
    queue_.enqueue(std::move(item));
}

template <>
bool SpecificConcurrentQueue<Vector<bool>>::TryDequeue(Vector<bool> &item) {
    return queue_.try_dequeue(item);
}
} // namespace infinity