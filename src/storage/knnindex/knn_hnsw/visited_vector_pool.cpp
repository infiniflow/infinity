module;

#include "concurrentqueue.h"

import stl;
import std;

module visited_vector_pool;

namespace infinity {
VisitedVectorPool::VisitedVectorPool(SizeT max_elements, SizeT init_vec)
    : max_elements_(max_elements)
// , stop_(false)
{
    for (SizeT i = 0; i < init_vec; ++i) {
        pool_.enqueue(Vector<bool>(max_elements_, false));
    }
    // clear_worker_ = std::thread([&]() {
    //     Pair<Vector<bool>, SizeT> to_clear;
    //     while (!stop_.load()) {
    //         if (to_clear_.try_dequeue(to_clear)) {
    //             std::fill(to_clear.first.begin(), to_clear.first.begin() + to_clear.second, false);
    //             pool_.enqueue(std::move(to_clear.first));
    //         }
    //     }
    // });
}

VisitedVectorPool::~VisitedVectorPool() {
    // stop_.store(true);
    // clear_worker_.join();
}

Vector<bool> VisitedVectorPool::Get() {
    Vector<bool> result;
    
    if (pool_.try_dequeue(result)) {
        return result;
    }
    return Vector<bool>(max_elements_, false);
}

void VisitedVectorPool::Release(Vector<bool> visited, SizeT used_size) {
    std::fill(visited.begin(), visited.begin() + used_size, false);
    pool_.enqueue(std::move(visited));
    // to_clear_.enqueue({std::move(visited), used_size});
}
} // namespace infinity