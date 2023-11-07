module;

import stl;
import std;
import buffer_obj;
import third_party;

export module specific_concurrent_queue;

namespace infinity {

export template <typename T>
class SpecificConcurrentQueue {
    ConcurrentQueue<T> queue_;

public:
    void Enqueue(const T &item);

    void Enqueue(T &&item);

    bool TryDequeue(T &item);
};

using FloatHeap = MaxHeap<Pair<float, i32>>;
template class SpecificConcurrentQueue<MaxHeap<Pair<float, i32>>>;

template class SpecificConcurrentQueue<BufferObj *>;

template class SpecificConcurrentQueue<Vector<bool>>;
} // namespace infinity
