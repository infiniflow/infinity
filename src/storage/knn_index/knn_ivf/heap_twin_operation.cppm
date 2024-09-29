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

module;

import stl;
export module heap_twin_operation;
namespace infinity {

// assume that distances have been initialized
// the 1st element may not be the largest / smallest when heap is not full
export template <typename Compare>
class heap_twin_multiple {
    using DistType = typename Compare::DistanceType;
    using ID = typename Compare::IDType;
    u64 nq{};
    u32 top_k{};
    DistType *distance_ptr = nullptr;
    ID *id_ptr = nullptr;
    Vector<u32> sizes;
    static void down(DistType *distance, ID *id, u32 size, u32 index) {
        if (index == 0 || (index << 1) > size) {
            return;
        }
        DistType tmp_d = distance[index];
        ID tmp_i = id[index];
        for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
            if (sub + 1 <= size && Compare::Compare(distance[sub + 1], distance[sub])) {
                ++sub;
            }
            if (!Compare::Compare(distance[sub], tmp_d)) {
                break;
            }
            distance[index] = distance[sub];
            id[index] = id[sub];
        }
        distance[index] = tmp_d;
        id[index] = tmp_i;
    }

public:
    explicit heap_twin_multiple(u64 nq, u32 top_k, DistType *distance_ptr, ID *id_ptr)
        : nq{nq}, top_k{top_k}, distance_ptr{distance_ptr}, id_ptr{id_ptr}, sizes(nq) {}
    ~heap_twin_multiple() = default;
    void initialize() { std::fill_n(distance_ptr, nq * top_k, Compare::InitialValue()); }
    void reinitialize() {
        std::fill(sizes.begin(), sizes.end(), 0);
        std::fill_n(distance_ptr, nq * top_k, Compare::InitialValue());
    }
    void add(u64 q_id, DistType d, ID i) {
        u32 &size = sizes[q_id];
        DistType *distance = distance_ptr + q_id * top_k - 1;
        ID *id = id_ptr + q_id * top_k - 1;
        if (size == top_k) {
            if (Compare::Compare(distance[1], d)) {
                distance[1] = d;
                id[1] = i;
                down(distance, id, size, 1);
            }
        } else {
            ++size;
            distance[size] = d;
            id[size] = i;
            if (size == top_k) {
                for (u32 index = size / 2; index > 0; --index) {
                    down(distance, id, size, index);
                }
            }
        }
    }
    void sort(u64 q_id) {
        u32 &size = sizes[q_id];
        DistType *distance = distance_ptr + q_id * top_k - 1;
        ID *id = id_ptr + q_id * top_k - 1;
        if (size < top_k && size > 1) {
            for (u32 index = size / 2; index > 0; --index) {
                down(distance, id, size, index);
            }
        }
        while (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            down(distance, id, size, 1);
        }
        size = 0;
    }
    void sort() {
        for (u64 q_id = 0; q_id < nq; ++q_id) {
            sort(q_id);
        }
    }
};

// assume that distances have been initialized to std::numeric_limits<DistType>::max() / lowest()
// index should start from 1
// the 1st element may not be the largest / smallest when heap is not full
export template <typename Compare, typename DistType, typename ID>
class heap_twin {
    DistType *distance = nullptr;
    ID *id = nullptr;
    u32 size{};
    u32 capacity{};
    void down(u32 index) {
        if (index == 0 || (index << 1) > size) {
            return;
        }
        DistType tmp_d = distance[index];
        ID tmp_i = id[index];
        for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
            if (sub + 1 <= size && Compare::Compare(distance[sub + 1], distance[sub])) {
                ++sub;
            }
            if (!Compare::Compare(distance[sub], tmp_d)) {
                break;
            }
            distance[index] = distance[sub];
            id[index] = id[sub];
        }
        distance[index] = tmp_d;
        id[index] = tmp_i;
    }
    void construct_heap() {
        for (u32 index = size / 2; index > 0; --index) {
            down(index);
        }
    }
    void pop() {
        if (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            down(1);
        } else if (size == 1) {
            size = 0;
        }
    }

public:
    heap_twin(u32 capacity, DistType *distance_ptr, ID *id_ptr) : capacity{capacity}, size{0}, distance{distance_ptr - 1}, id{id_ptr - 1} {}
    void initialize() {
        size = 0;
        std::fill_n(distance + 1, capacity, Compare::InitialValue());
    }
    void add(DistType d, ID i) {
        if (size == capacity) {
            if (Compare::Compare(distance[1], d)) {
                distance[1] = d;
                id[1] = i;
                down(1);
            }
        } else {
            ++size;
            distance[size] = d;
            id[size] = i;
            if (size == capacity) {
                construct_heap();
            }
        }
    }
    void sort() {
        if (size < capacity && size > 1) {
            construct_heap();
        }
        while (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            down(1);
        }
        size = 0;
    }
};

} // namespace infinity