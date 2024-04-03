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

import stl;
import hnsw_common;
import file_system;
import graph_store_inner;

export module graph_store;

namespace infinity {

export class GraphStore {
private:
    GraphStore(GraphStoreInner inner) : inner_(std::move(inner)) {}

public:
    static GraphStore Make(SizeT max_vertex, SizeT Mmax, SizeT Mmax0) {
        GraphStore graph_store(GraphStoreInner::Make(max_vertex, Mmax, Mmax0));
        graph_store.max_layer_ = -1;
        graph_store.enterpoint_ = -1;
        return graph_store;
    }

    GraphStore(GraphStore &&) = default;
    ~GraphStore() = default;

    void Free(SizeT cur_vertex_n) { inner_.Free(cur_vertex_n); }

    void AddVertex(VertexType vertex_i, i32 layer_n) {
        inner_.AddVertex(vertex_i, layer_n);
        if (layer_n > max_layer_) {
            max_layer_ = layer_n;
            enterpoint_ = vertex_i;
        }
    }

    i32 max_layer() const { return max_layer_; }

    VertexType enterpoint() const { return enterpoint_; }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i) const { return inner_.GetNeighbors(vertex_i, layer_i); }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i) { return inner_.GetNeighborsMut(vertex_i, layer_i); }

    void SaveGraph(FileHandler &file_handler, SizeT cur_vertex_n) const {
        file_handler.Write(&max_layer_, sizeof(max_layer_));
        file_handler.Write(&enterpoint_, sizeof(enterpoint_));
        inner_.SaveGraph(file_handler, cur_vertex_n);
    }

    static GraphStore LoadGraph(FileHandler &file_handler, SizeT max_vertex, SizeT Mmax, SizeT Mmax0, SizeT cur_vertex_n) {
        i32 max_layer;
        file_handler.Read(&max_layer, sizeof(max_layer));
        VertexType enterpoint;
        file_handler.Read(&enterpoint, sizeof(enterpoint));

        GraphStore graph_store(GraphStoreInner::LoadGraph(file_handler, max_vertex, Mmax, Mmax0, cur_vertex_n));
        graph_store.max_layer_ = max_layer;
        graph_store.enterpoint_ = enterpoint;

        return graph_store;
    }

    void CheckGraph(SizeT Mmax0, SizeT Mmax, SizeT cur_vertex_n) const {
        i32 max_l = -1;
        inner_.CheckGraph(Mmax0, Mmax, cur_vertex_n, 0, cur_vertex_n - 1, max_l);
        assert(max_l == max_layer_);
    }

    void Dump(std::ostream &os, SizeT cur_vertex_n) {
        os << "max layer: " << max_layer_ << std::endl;
        os << "enterpoint: " << enterpoint_ << std::endl;
        os << std::endl;
        os << "inner: " << std::endl;
        inner_.Dump(os, cur_vertex_n);
    }

private:
    GraphStoreInner inner_;

    i32 max_layer_{};
    VertexType enterpoint_{};
};

} // namespace infinity