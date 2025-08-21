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

export module infinity_core:diskann_mem_graph_store;

import :local_file_handle;

import std;

namespace infinity {
export class DiskAnnMemGraphStore {
public:
    using This = DiskAnnMemGraphStore;
    using GraphStruct = std::vector<std::vector<size_t>>;
    using VectorStruct = std::vector<size_t>;

    DiskAnnMemGraphStore() : capacity_(0), reserve_graph_degree_(0), graph_(0), max_range_of_graph_(0), max_observecd_degree_(0) {}

    DiskAnnMemGraphStore(size_t capacity, u32 reserve_graph_degree)
        : capacity_(capacity), reserve_graph_degree_(reserve_graph_degree), graph_(capacity), max_range_of_graph_(0), max_observecd_degree_(0) {
        graph_.resize(capacity);
        for (size_t i = 0; i < capacity; i++) {
            graph_[i].reserve(reserve_graph_degree);
        }
    }

    ~DiskAnnMemGraphStore() = default;

    DiskAnnMemGraphStore(This &&other)
        : capacity_(std::exchange(other.capacity_, 0)), reserve_graph_degree_(std::exchange(other.reserve_graph_degree_, 0)),
          graph_(std::move(other.graph_)), max_range_of_graph_(std::exchange(other.max_range_of_graph_, 0)),
          max_observecd_degree_(std::exchange(other.max_observecd_degree_, 0)) {}

    DiskAnnMemGraphStore &operator=(This &&other) {
        if (this != &other) {
            capacity_ = std::exchange(other.capacity_, 0);
            reserve_graph_degree_ = std::exchange(other.reserve_graph_degree_, 0);
            graph_ = std::move(other.graph_);
            max_range_of_graph_ = std::exchange(other.max_range_of_graph_, 0);
            max_observecd_degree_ = std::exchange(other.max_observecd_degree_, 0);
        }
        return *this;
    }

    static This Make(size_t capacity, size_t reserve_graph_degree) { return This(capacity, reserve_graph_degree); }

    // return std::tuple of (load_point_num, enterpoint, frozen_pts)
    std::tuple<size_t, size_t, size_t> Load(LocalFileHandle &load_handle, size_t expected_num_points) {
        size_t expected_file_size = 0;
        size_t start = 0; // enterpoint
        size_t file_frozen_pts = 0;
        load_handle.Read(&expected_file_size, sizeof(size_t));
        load_handle.Read(&max_observecd_degree_, sizeof(u32));
        load_handle.Read(&start, sizeof(size_t));
        load_handle.Read(&file_frozen_pts, sizeof(size_t));

        if (this->capacity_ < expected_num_points) {
            this->ResizeGraph(expected_num_points);
        }

        size_t bytes_read = 28; // meta data size
        size_t nodes_read = 0;  // number of nodes read
        while (bytes_read != expected_file_size) {
            u32 k; // number of neighbors
            load_handle.Read(&k, sizeof(u32));
            std::vector<size_t> tmp(k);
            tmp.reserve(k);
            load_handle.Read(tmp.data(), sizeof(size_t) * k);
            graph_[nodes_read - 1].swap(tmp);
            bytes_read += sizeof(size_t) * k + sizeof(u32);
            nodes_read++;
            if (k > max_range_of_graph_) {
                max_range_of_graph_ = k;
            }
        }

        return std::tuple<size_t, size_t, size_t>(nodes_read, start, file_frozen_pts);
    }

    void Save(LocalFileHandle &save_handle, size_t num_points, size_t num_frozen_points, size_t start) {
        size_t index_size = 28; // init size of meta data :(sizeof(size_t) * 3 + sizeof(u32)) bytes
        u32 max_degree = 0;
        for (size_t i = 0; i < num_points; i++) {
            u32 gk = static_cast<u32>(graph_[i].size());
            index_size += static_cast<size_t>(sizeof(size_t) * gk + sizeof(u32));
            max_degree = std::max(max_degree, gk);
        }

        save_handle.Append(&index_size, sizeof(size_t));
        save_handle.Append(&max_degree, sizeof(u32));
        save_handle.Append(&start, sizeof(size_t));
        save_handle.Append(&num_frozen_points, sizeof(size_t));
        for (size_t i = 0; i < num_points; i++) {
            u32 gk = static_cast<u32>(graph_[i].size());
            save_handle.Append(&gk, sizeof(u32));
            save_handle.Append(graph_[i].data(), sizeof(size_t) * gk);
        }
    }

    size_t GetTotalPoints() const { return this->capacity_; }

    VectorStruct &GetNeighbours(const size_t vertex_id) { return graph_[vertex_id]; }

    void AddNeighbour(const size_t vertex_id, const size_t neighbour_id) {
        graph_[vertex_id].emplace_back(neighbour_id);
        if (graph_[vertex_id].size() > max_observecd_degree_) {
            max_observecd_degree_ = static_cast<u32>(graph_[vertex_id].size());
        }
    }

    void ClearNeighbours(const size_t vertex_id) { graph_[vertex_id].clear(); }

    void SwapNeighbours(const size_t vertex_a, const size_t vertex_b) { graph_[vertex_a].swap(graph_[vertex_b]); }

    void SetNeighbours(const size_t vertex_id, const VectorStruct &neighbours) {
        graph_[vertex_id].assign(neighbours.begin(), neighbours.end());
        if (neighbours.size() > max_observecd_degree_) {
            max_observecd_degree_ = static_cast<u32>(neighbours.size());
        }
    }

    size_t ResizeGraph(const size_t new_capacity) {
        graph_.resize(new_capacity);
        capacity_ = new_capacity;
        return graph_.size();
    }

    void ClearGraph() { graph_.clear(); }

    size_t GetMaxRangeOfGraph() const { return max_range_of_graph_; }

    size_t GetMaxObservedDegree() const { return max_observecd_degree_; }

    void InitRandomKnn(size_t num_points) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, num_points - 1);

        for (size_t i = 0; i < num_points; i++) {
            size_t k = dis(gen);
            for (size_t j = 0; j < k; j++) {
                size_t neighbor_id = dis(gen);
                while (neighbor_id == i) {
                    neighbor_id = dis(gen);
                }
                graph_[i].emplace_back(neighbor_id);
            }
        }
    }

private:
    size_t capacity_;             // point num in graph
    size_t reserve_graph_degree_; // max degree to reserve

    GraphStruct graph_;
    u32 max_range_of_graph_;
    u32 max_observecd_degree_;
};

} // namespace infinity