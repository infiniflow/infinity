//
// Created by yzq on 11/9/23.
//
module;
#include <algorithm>
import stl;
export module heap_twin_operation;
namespace infinity {

// assume that distances have been initialized to std::numeric_limits<DistType>::max()
// the 1st element may not be the largest when heap is not full
export template <typename DistType, typename ID>
class heap_twin_max_multiple {
    u64 nq;
    u32 top_k;
    DistType *distance_ptr;
    ID *id_ptr;
    Vector<u32> sizes;
    void down(DistType *distance, ID *id, u32 size, u32 index) {
        for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
            if (sub + 1 <= size && distance[sub + 1] > distance[sub]) {
                ++sub;
            }
            if (distance[sub] <= distance[index]) {
                break;
            }
            std::swap(distance[sub], distance[index]);
            std::swap(id[sub], id[index]);
        }
    }

public:
    heap_twin_max_multiple(u64 nq, u32 top_k, DistType *distance_ptr, ID *id_ptr)
        : nq{nq}, top_k{top_k}, distance_ptr{distance_ptr}, id_ptr{id_ptr}, sizes(nq) {}
    void initialize() { std::fill_n(distance_ptr, nq * top_k, std::numeric_limits<DistType>::max()); }
    void add(u64 q_id, DistType d, ID i) {
        u32 &size = sizes[q_id];
        DistType *distance = distance_ptr + q_id * top_k - 1;
        ID *id = id_ptr + q_id * top_k - 1;
        if (size == top_k) {
            if (d < distance[1]) {
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
        while (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            down(distance, id, size, 1);
        }
        size = 0;
    }
};

// assume that distances have been initialized to std::numeric_limits<DistType>::lowest()
// the 1st element may not be the smallest when heap is not full
export template <typename DistType, typename ID>
class heap_twin_min_multiple {
    u64 nq;
    u32 top_k;
    DistType *distance_ptr;
    ID *id_ptr;
    Vector<u32> sizes;
    void down(DistType *distance, ID *id, u32 size, u32 index) {
        for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
            if (sub + 1 <= size && distance[sub + 1] < distance[sub]) {
                ++sub;
            }
            if (distance[sub] >= distance[index]) {
                break;
            }
            std::swap(distance[sub], distance[index]);
            std::swap(id[sub], id[index]);
        }
    }

public:
    heap_twin_min_multiple(u64 nq, u32 top_k, DistType *distance_ptr, ID *id_ptr)
        : nq{nq}, top_k{top_k}, distance_ptr{distance_ptr}, id_ptr{id_ptr}, sizes(nq) {}
    void initialize() { std::fill_n(distance_ptr, nq * top_k, std::numeric_limits<DistType>::lowest()); }
    void add(u64 q_id, DistType d, ID i) {
        u32 &size = sizes[q_id];
        DistType *distance = distance_ptr + q_id * top_k - 1;
        ID *id = id_ptr + q_id * top_k - 1;
        if (size == top_k) {
            if (d > distance[1]) {
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
        while (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            down(distance, id, size, 1);
        }
        size = 0;
    }
};

// assume that distances have been initialized to std::numeric_limits<DistType>::max()
// index should start from 1
// the 1st element may not be the largest when heap is not full
export template <typename DistType, typename ID>
class heap_twin_max {
    DistType *distance;
    ID *id;
    u32 size;
    u32 capacity;
    void up(u32 index) {
        while (index > 1) {
            u32 parent = index >> 1;
            if (distance[parent] >= distance[index]) {
                break;
            }
            std::swap(distance[parent], distance[index]);
            std::swap(id[parent], id[index]);
            index = parent;
        }
    }
    void down(u32 index) {
        while ((index << 1) <= size) {
            u32 sub = index << 1;
            if (sub + 1 <= size && distance[sub + 1] > distance[sub]) {
                ++sub;
            }
            if (distance[sub] <= distance[index]) {
                break;
            }
            std::swap(distance[sub], distance[index]);
            std::swap(id[sub], id[index]);
            index = sub;
        }
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
        }
    }

public:
    heap_twin_max(u32 capacity, DistType *distance_ptr, ID *id_ptr) : capacity{capacity}, size{0}, distance{distance_ptr - 1}, id{id_ptr - 1} {}
    void initialize() { std::fill_n(distance + 1, capacity, std::numeric_limits<DistType>::max()); }
    void add(DistType d, ID i) {
        if (size == capacity) {
            if (d < distance[1]) {
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
        while (size > 1) {
            pop();
        }
    }
};

// assume that distances have been initialized to std::numeric_limits<DistType>::lowest()
// index should start from 1
// the 1st element may not be the smallest when heap is not full
export template <typename DistType, typename ID>
class heap_twin_min {
    DistType *distance;
    ID *id;
    u32 size;
    u32 capacity;
    void up(u32 index) {
        while (index > 1) {
            u32 parent = index >> 1;
            if (distance[parent] <= distance[index]) {
                break;
            }
            std::swap(distance[parent], distance[index]);
            std::swap(id[parent], id[index]);
            index = parent;
        }
    }
    void down(u32 index) {
        while ((index << 1) <= size) {
            u32 sub = index << 1;
            if (sub + 1 <= size && distance[sub + 1] < distance[sub]) {
                ++sub;
            }
            if (distance[sub] >= distance[index]) {
                break;
            }
            std::swap(distance[sub], distance[index]);
            std::swap(id[sub], id[index]);
            index = sub;
        }
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
        }
    }

public:
    heap_twin_min(u32 capacity, DistType *distance_ptr, ID *id_ptr) : capacity{capacity}, size{0}, distance{distance_ptr - 1}, id{id_ptr - 1} {}
    void initialize() { std::fill_n(distance + 1, capacity, std::numeric_limits<DistType>::lowest()); }
    void add(DistType d, ID i) {
        if (size == capacity) {
            if (d > distance[1]) {
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
        while (size > 1) {
            pop();
        }
    }
};

} // namespace infinity