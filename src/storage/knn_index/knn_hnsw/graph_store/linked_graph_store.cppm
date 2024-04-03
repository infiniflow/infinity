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
#include <fstream>

export module linked_graph_store;

import stl;
import hnsw_common;
import graph_store_inner;
import file_system;

namespace infinity {

export class LinkedGraphStore {
private:
    LinkedGraphStore(SizeT chunk_size, SizeT Mmax, SizeT Mmax0) : chunk_size_(chunk_size), Mmax_(Mmax), Mmax0_(Mmax0) {}

public:
    static LinkedGraphStore Make(SizeT chunk_size, SizeT Mmax, SizeT Mmax0) {
        LinkedGraphStore graph_store(chunk_size, Mmax, Mmax0);
        graph_store.max_layer_ = -1;
        graph_store.enterpoint_ = -1;
        return graph_store;
    }

    LinkedGraphStore(LinkedGraphStore &&) = default;
    ~LinkedGraphStore() = default;

    void Free(SizeT cur_vertex_n) {
        SizeT chunk_n = inners_.size();
        if (chunk_n == 0) {
            return;
        }
        for (SizeT i = 0; i < chunk_n - 1; ++i) {
            inners_[i].Free(chunk_size_);
        }
        inners_.back().Free(LastChunkSize(cur_vertex_n));
    }

    void AddVertex(VertexType vertex_i, i32 layer_n) {
        SizeT chunk_id = vertex_i / chunk_size_;
        SizeT old_chunk_n = inners_.size();
        for (SizeT i = old_chunk_n; i <= chunk_id; ++i) {
            inners_.push_back(GraphStoreInner::Make(chunk_size_, Mmax_, Mmax0_));
        }
        auto &graph_store = inners_[chunk_id];
        graph_store.AddVertex(vertex_i % chunk_size_, layer_n);
        if (layer_n > max_layer_) {
            max_layer_ = layer_n;
            enterpoint_ = vertex_i;
        }
    }

    i32 max_layer() const { return max_layer_; }

    VertexType enterpoint() const { return enterpoint_; }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i) const {
        SizeT chunk_id = vertex_i / chunk_size_;
        assert(chunk_id < inners_.size());
        return inners_[chunk_id].GetNeighbors(vertex_i % chunk_size_, layer_i);
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i) {
        SizeT chunk_id = vertex_i / chunk_size_;
        assert(chunk_id < inners_.size());
        return inners_[chunk_id].GetNeighborsMut(vertex_i % chunk_size_, layer_i);
    }

    void SaveGraph(FileHandler &file_handler, SizeT cur_vertex_n) const {
        file_handler.Write(&max_layer_, sizeof(max_layer_));
        file_handler.Write(&enterpoint_, sizeof(enterpoint_));
        SizeT chunk_n = inners_.size();
        if (chunk_n == 0) {
            return;
        }
        for (SizeT i = 0; i < chunk_n - 1; ++i) {
            inners_[i].SaveGraph(file_handler, chunk_size_);
        }
        inners_.back().SaveGraph(file_handler, LastChunkSize(cur_vertex_n));
    }

    static LinkedGraphStore LoadGraph(FileHandler &file_handler, SizeT chunk_size, SizeT Mmax, SizeT Mmax0, SizeT cur_vertex_n) {
        LinkedGraphStore graph_store(chunk_size, Mmax, Mmax0);

        file_handler.Read(&graph_store.max_layer_, sizeof(graph_store.max_layer_));
        file_handler.Read(&graph_store.enterpoint_, sizeof(graph_store.enterpoint_));
        SizeT chunk_n = cur_vertex_n / chunk_size;
        if (chunk_n == 0) {
            return graph_store;
        }
        for (SizeT i = 0; i < chunk_n - 1; ++i) {
            graph_store.inners_.push_back(GraphStoreInner::LoadGraph(file_handler, chunk_size, Mmax, Mmax0, chunk_size));
        }
        graph_store.inners_.push_back(GraphStoreInner::LoadGraph(file_handler, chunk_size, Mmax, Mmax0, cur_vertex_n % chunk_size));
        return graph_store;
    }

    void CheckGraph(SizeT Mmax0, SizeT Mmax, SizeT cur_vertex_n) const {
        assert(Mmax0 == Mmax0_);
        assert(Mmax == Mmax_);
        SizeT chunk_n = inners_.size();
        if (chunk_n == 0) {
            return;
        }
        i32 max_max_l = -1;
        for (SizeT i = 0; i < chunk_n; ++i) {
            SizeT cur_n = (i == chunk_n - 1) ? LastChunkSize(cur_vertex_n) : chunk_size_;
            SizeT cur_offset = i * chunk_size_;
            i32 max_l = -1;
            inners_[i].CheckGraph(Mmax0, Mmax, cur_n, cur_offset, cur_vertex_n - 1, max_l);
            assert(max_l <= max_layer_);
            max_max_l = std::max(max_max_l, max_l);
        }
        assert(max_max_l == max_layer_);
    }

    void Dump(std::ostream &os, SizeT cur_vertex_n) {
        os << "max layer: " << max_layer_ << std::endl;
        os << "enterpoint: " << enterpoint_ << std::endl;
        os << std::endl;
        SizeT chunk_n = inners_.size();
        if (chunk_n == 0) {
            return;
        }
        os << "inner:" << std::endl;
        for (SizeT i = 0; i < chunk_n - 1; ++i) {
            os << "chunk " << i << ":" << std::endl;
            inners_[i].Dump(os, chunk_size_);
        }
    }

private:
    SizeT LastChunkSize(SizeT cur_vertex_n) const {
        assert(cur_vertex_n > 0);
        return (cur_vertex_n - 1) % chunk_size_ + 1;
    }

private:
    Vector<GraphStoreInner> inners_;

    SizeT chunk_size_;
    SizeT Mmax_;
    SizeT Mmax0_;

    i32 max_layer_{};
    VertexType enterpoint_{};
};

} // namespace infinity