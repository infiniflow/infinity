module;

import stl;
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

using PFV = Pair<float, i32>;
using CMP = CompareByFirst<float, i32>;
using FloatDistHeap = Heap<PFV, CMP>;
template class SpecificConcurrentQueue<FloatDistHeap>;

template class SpecificConcurrentQueue<BufferObj *>;

template class SpecificConcurrentQueue<Vector<bool>>;
} // namespace infinity
