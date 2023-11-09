//
// Created by yzq on 11/9/23.
//
module;
import stl;
import std_lib;
export module heap_twin_operation;
namespace infinity {

// assume that distances have been initialized to max
// index should start from 1
// the 1st element may not be the largest when heap is not full
export template <typename ID>
class heap_twin_max {
    f32 *distance;
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
    heap_twin_max(u32 capacity, f32 *distance_ptr, ID *id_ptr) : capacity{capacity}, size{0}, distance{distance_ptr - 1}, id{id_ptr - 1} {}
    void initialize() { std::fill_n(distance + 1, capacity, std::numeric_limits<f32>::max()); }
    void add(f32 d, ID i) {
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

// assume that distances have been initialized to min
// index should start from 1
// the 1st element may not be the smallest when heap is not full
export template <typename ID>
class heap_twin_min {
    f32 *distance;
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
    heap_twin_min(u32 capacity, f32 *distance_ptr, ID *id_ptr) : capacity{capacity}, size{0}, distance{distance_ptr - 1}, id{id_ptr - 1} {}
    void initialize() { std::fill_n(distance + 1, capacity, std::numeric_limits<f32>::min()); }
    void add(f32 d, ID i) {
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