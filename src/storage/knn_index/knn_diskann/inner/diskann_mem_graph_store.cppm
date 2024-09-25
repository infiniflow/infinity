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

#include <cassert>
#include <ostream>

export module diskann_mem_graph_store;

import stl;
import file_system;

namespace infinity {
export class DiskAnnMemGraphStore {
public:
    using This = DiskAnnMemGraphStore;
    using GraphStruct = Vector<Vector<SizeT>>;
    using VectorStruct = Vector<SizeT>;

    DiskAnnMemGraphStore() : capacity_(0), reserve_graph_degree_(0), graph_(0), max_range_of_graph_(0), max_observecd_degree_(0) {}

    DiskAnnMemGraphStore(SizeT capacity, u32 reserve_graph_degree)
        : capacity_(capacity), reserve_graph_degree_(reserve_graph_degree), graph_(capacity), max_range_of_graph_(0), max_observecd_degree_(0) {
        graph_.resize(capacity);
        for (SizeT i = 0; i < capacity; i++) {
            graph_[i].reserve(reserve_graph_degree);
        }
    }

    ~DiskAnnMemGraphStore() = default;

    DiskAnnMemGraphStore(This &&other)
        : capacity_(std::exchange(other.capacity_, 0)), reserve_graph_degree_(std::exchange(other.reserve_graph_degree_, 0)),
          graph_(std::move(other.graph_)), max_range_of_graph_(std::exchange(other.max_range_of_graph_, 0)),
          max_observecd_degree_(std::exchange(other.max_observecd_degree_, 0)) 
          {}

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

    static This Make(SizeT capacity, SizeT reserve_graph_degree) {
        return This(capacity, reserve_graph_degree);
    }

    // return Tuple of (load_point_num, enterpoint, frozen_pts)
    Tuple<SizeT, SizeT, SizeT> Load(FileHandler &load_handler, SizeT expected_num_points){
        SizeT expected_file_size = 0;
        SizeT start = 0; // enterpoint
        SizeT file_frozen_pts = 0;
        load_handler.Read(&expected_file_size, sizeof(SizeT));
        load_handler.Read(&max_observecd_degree_, sizeof(u32));
        load_handler.Read(&start, sizeof(SizeT));
        load_handler.Read(&file_frozen_pts, sizeof(SizeT));

        if (this->capacity_ < expected_num_points) {
            this->ResizeGraph(expected_num_points);
        }

        SizeT bytes_read = 28; // meta data size
        SizeT nodes_read = 0; // number of nodes read
        while (bytes_read != expected_file_size) {
            u32 k; // number of neighbors
            load_handler.Read(&k, sizeof(u32));
            Vector<SizeT> tmp(k);
            tmp.reserve(k);
            load_handler.Read(tmp.data(), sizeof(SizeT) * k);
            graph_[nodes_read - 1].swap(tmp);
            bytes_read += sizeof(SizeT) * k + sizeof(u32);
            nodes_read++;
            if (k > max_range_of_graph_) {
                max_range_of_graph_ = k;
            }
        }
        
        return Tuple<SizeT, SizeT, SizeT>(nodes_read, start, file_frozen_pts);
    }

    void Save(FileHandler &save_handler, SizeT num_points, SizeT num_frozen_points, SizeT start) {
        SizeT index_size = 28; // init size of meta data :(sizeof(SizeT) * 3 + sizeof(u32)) bytes
        u32 max_degree = 0;
        for (SizeT i = 0; i < num_points; i++) {
            u32 gk = static_cast<u32>(graph_[i].size());
            index_size += static_cast<SizeT>(sizeof(SizeT) * gk + sizeof(u32));
            max_degree = std::max(max_degree, gk);
        }

        save_handler.Write(&index_size, sizeof(SizeT));
        save_handler.Write(&max_degree, sizeof(u32));
        save_handler.Write(&start, sizeof(SizeT));
        save_handler.Write(&num_frozen_points, sizeof(SizeT));
        for (SizeT i = 0; i < num_points; i++) {
            u32 gk = static_cast<u32>(graph_[i].size());
            save_handler.Write(&gk, sizeof(u32));
            save_handler.Write(graph_[i].data(), sizeof(SizeT) * gk);
        }
    }

    SizeT GetTotalPoints() const {
        return this->capacity_;
    }
    
    VectorStruct &GetNeighbours(const SizeT vertex_id) {
        return graph_[vertex_id];
    }

    void AddNeighbour(const SizeT vertex_id, const SizeT neighbour_id) {
        graph_[vertex_id].emplace_back(neighbour_id);
        if (graph_[vertex_id].size() > max_observecd_degree_) {
            max_observecd_degree_ = static_cast<u32>(graph_[vertex_id].size());
        }
    }

    void ClearNeighbours(const SizeT vertex_id) {
        graph_[vertex_id].clear();
    }

    void SwapNeighbours(const SizeT vertex_a, const SizeT vertex_b) {
        graph_[vertex_a].swap(graph_[vertex_b]);
    }

    void SetNeighbours(const SizeT vertex_id, const VectorStruct &neighbours) {
        graph_[vertex_id].assign(neighbours.begin(), neighbours.end());
        if (neighbours.size() > max_observecd_degree_){
            max_observecd_degree_ = static_cast<u32>(neighbours.size());
        }
    }

    SizeT ResizeGraph(const SizeT new_capacity) {
        graph_.resize(new_capacity);
        capacity_ = new_capacity;
        return graph_.size();
    }

    void ClearGraph() {
        graph_.clear();
    }

    SizeT GetMaxRangeOfGraph() const {
        return max_range_of_graph_;
    }

    SizeT GetMaxObservedDegree() const {
        return max_observecd_degree_;
    }

    void InitRandomKnn(SizeT num_points){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<SizeT> dis(0, num_points - 1);

        for (SizeT i = 0; i < num_points; i++) {
            SizeT k = dis(gen);
            for (SizeT j = 0; j < k; j++) {
                SizeT neighbor_id = dis(gen);
                while (neighbor_id == i) {
                    neighbor_id = dis(gen);
                }
                graph_[i].emplace_back(neighbor_id);
            }
        }

    }


private:
    SizeT capacity_; // point num in graph
    SizeT reserve_graph_degree_; // max degree to reserve

    GraphStruct graph_;
    u32 max_range_of_graph_;
    u32 max_observecd_degree_;
};

} // namespace infinity