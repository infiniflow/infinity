module;

#include "concurrentqueue.h"

import stl;
import buffer_obj;

module specific_concurrent_queue;

namespace infinity {

template <>
void SpecificConcurrentQueue<FloatHeap>::Enqueue(const FloatHeap &item) {
    queue_.enqueue(item);
}

template <>
void SpecificConcurrentQueue<FloatHeap>::Enqueue(FloatHeap &&item) {
    queue_.enqueue(Move(item));
}

template <>
bool SpecificConcurrentQueue<FloatHeap>::TryDequeue(FloatHeap &item) {
    return queue_.try_dequeue(item);
}

template <>
void SpecificConcurrentQueue<BufferObj *>::Enqueue(BufferObj *const &item) {
    queue_.enqueue(item);
}

template <>
void SpecificConcurrentQueue<BufferObj *>::Enqueue(BufferObj * &&item) {
    queue_.enqueue(Move(item));
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
    queue_.enqueue(Move(item));
}

template <>
bool SpecificConcurrentQueue<Vector<bool>>::TryDequeue(Vector<bool> &item) {
    return queue_.try_dequeue(item);
}
} // namespace infinitye