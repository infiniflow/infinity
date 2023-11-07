module;

import stl;
import std;
import specific_concurrent_queue;

export module hnsw_mem_pool;

namespace infinity {

export class HnswMemPool {
    const SizeT max_elements_;

    SpecificConcurrentQueue<Vector<bool>> visited_pool_;

public:
    HnswMemPool(SizeT max_elements, SizeT init_vec = 0) : max_elements_(max_elements) {
        for (SizeT i = 0; i < init_vec; ++i) {
            visited_pool_.Enqueue(Vector<bool>(max_elements_, false));
        }
    }

    Vector<bool> GetVisited() {
        Vector<bool> result;
        if (visited_pool_.TryDequeue(result)) {
            return result;
        }
        return Vector<bool>(max_elements_, false);
    }

    void ReleaseVisited(Vector<bool> visited, SizeT used_size) {
        std::fill(visited.begin(), visited.begin() + used_size, false); // TODO:: memset
        visited_pool_.Enqueue(std::move(visited));                      // check if lockfree
    }
};
} // namespace infinity