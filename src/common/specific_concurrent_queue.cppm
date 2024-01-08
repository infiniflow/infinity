module;

import stl;
import buffer_obj;
import third_party;

export module specific_concurrent_queue;

namespace infinity {

export template <typename T>
class SpecificConcurrentQueue {
    moodycamel::ConcurrentQueue<T> queue_;

public:
    void Enqueue(const T &item);

    void Enqueue(T &&item);

    bool TryDequeue(T &item);
};

export using PFV = Pair<float, i32>;
export using CMP = CompareByFirst<float, i32>;
export using FloatDistHeap = Heap<PFV, CMP>;

export template class SpecificConcurrentQueue<FloatDistHeap>;
export template class SpecificConcurrentQueue<BufferObj *>;
export template class SpecificConcurrentQueue<Vector<bool>>;
} // namespace infinity
