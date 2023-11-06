module;

import stl;
import std;
import third_party;

export module visited_vector_pool;

namespace infinity {

export class VisitedVectorPool {
    const SizeT max_elements_;
    ConcurrentQueue<Vector<bool>> pool_;
    // ConcurrentQueue<Pair<Vector<bool>, SizeT>> to_clear_;

    // std::thread clear_worker_;

    // atomic_bool stop_;

public:
    VisitedVectorPool(SizeT max_elements, SizeT init_vec = 0);

    ~VisitedVectorPool();

    Vector<bool> Get();

    void Release(Vector<bool> visited, SizeT used_size);
};
} // namespace infinity