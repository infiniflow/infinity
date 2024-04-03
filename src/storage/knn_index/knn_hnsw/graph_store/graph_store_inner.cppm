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

export module graph_store_inner;

namespace infinity {

export class GraphStoreInner {
private:
    constexpr static SizeT layer_n_offset_ = 0;
    constexpr static SizeT layers_p_offset_ = AlignTo(layer_n_offset_ + sizeof(LayerSize), sizeof(char *));
    constexpr static SizeT l0_neighbor_n_offset_ = AlignTo(layers_p_offset_ + sizeof(char *), sizeof(VertexListSize));
    constexpr static SizeT l0_neighbors_offset_ = AlignTo(l0_neighbor_n_offset_ + sizeof(VertexListSize), sizeof(VertexType));
    SizeT level0_size_;

    constexpr static SizeT lx_neighbor_n_offset_ = 0;
    constexpr static SizeT lx_neighbors_offset_ = AlignTo(lx_neighbor_n_offset_ + sizeof(VertexListSize), sizeof(VertexType));
    SizeT levelx_size_;

    UniquePtr<char[]> graph_;
    SizeT loaded_vertex_n_;
    UniquePtr<char[]> loaded_layers_;

private:
    class VertexL0Mut {
        char *const ptr_;

    public:
        VertexL0Mut(char *ptr) : ptr_(ptr) {}
        Pair<char **, LayerSize *> GetLayers() {
            return {reinterpret_cast<char **>(ptr_ + layers_p_offset_), reinterpret_cast<LayerSize *>(ptr_ + layer_n_offset_)};
        }
        Pair<VertexType *, VertexListSize *> GetNeighbors() {
            return {reinterpret_cast<VertexType *>(ptr_ + l0_neighbors_offset_), reinterpret_cast<VertexListSize *>(ptr_ + l0_neighbor_n_offset_)};
        }
    };
    class VertexLXMut {
        char *const ptr_;

    public:
        VertexLXMut(char *ptr) : ptr_(ptr) {}
        Pair<VertexType *, VertexListSize *> GetNeighbors() {
            return {reinterpret_cast<VertexType *>(ptr_ + lx_neighbors_offset_), reinterpret_cast<VertexListSize *>(ptr_ + lx_neighbor_n_offset_)};
        }
    };
    class VertexL0 {
        const char *const ptr_;

    public:
        VertexL0(const char *ptr) : ptr_(ptr) {}
        Pair<const char *, LayerSize> GetLayers() const {
            return {*reinterpret_cast<char *const *>(ptr_ + layers_p_offset_), *reinterpret_cast<const LayerSize *>(ptr_ + layer_n_offset_)};
        }
        Pair<const VertexType *, VertexListSize> GetNeighbors() const {
            return {reinterpret_cast<const VertexType *>(ptr_ + l0_neighbors_offset_),
                    *reinterpret_cast<const VertexListSize *>(ptr_ + l0_neighbor_n_offset_)};
        }
    };
    class VertexLX {
        const char *const ptr_;

    public:
        VertexLX(const char *ptr) : ptr_(ptr) {}
        Pair<const VertexType *, VertexListSize> GetNeighbors() const {
            return {reinterpret_cast<const VertexType *>(ptr_ + lx_neighbors_offset_),
                    *reinterpret_cast<const VertexListSize *>(ptr_ + lx_neighbor_n_offset_)};
        }
    };
    VertexL0Mut GetLevel0Mut(VertexType vertex_i) { return VertexL0Mut(graph_.get() + level0_size_ * vertex_i); }
    VertexLXMut GetLevelXMut(VertexL0Mut &level0, LayerSize layer_i) { return VertexLXMut(*level0.GetLayers().first + levelx_size_ * (layer_i - 1)); }
    VertexL0 GetLevel0(VertexType vertex_i) const { return VertexL0(graph_.get() + level0_size_ * vertex_i); }
    VertexLX GetLevelX(const VertexL0 &level0, LayerSize layer_i) const { return VertexLX(level0.GetLayers().first + levelx_size_ * (layer_i - 1)); }

private:
    GraphStoreInner(SizeT max_vertex, SizeT Mmax, SizeT Mmax0, SizeT loaded_vertex_n) {
        level0_size_ = AlignTo(l0_neighbors_offset_ + sizeof(VertexType) * Mmax0, 8);
        levelx_size_ = AlignTo(lx_neighbors_offset_ + sizeof(VertexType) * Mmax, 8);
        graph_ = MakeUnique<char[]>(max_vertex * level0_size_);
        loaded_vertex_n_ = loaded_vertex_n;
    }

public:
    GraphStoreInner(GraphStoreInner &&) = default;
    ~GraphStoreInner() = default;

    static GraphStoreInner Make(SizeT max_vertex, SizeT Mmax, SizeT Mmax0) {
        auto graph_store = GraphStoreInner(max_vertex, Mmax, Mmax0, 0);
        std::fill(graph_store.graph_.get(), graph_store.graph_.get() + max_vertex * graph_store.level0_size_, 0);
        return graph_store;
    }

    void Free(SizeT current_vertex_num) {
        for (VertexType vertex_i = loaded_vertex_n_; vertex_i < VertexType(current_vertex_num); ++vertex_i) {
            delete[] GetLevel0(vertex_i).GetLayers().first;
        }
    }

    void AddVertex(VertexType vertex_i, i32 layer_n) {
        VertexL0Mut vertex = GetLevel0Mut(vertex_i);
        *vertex.GetNeighbors().second = 0;
        auto [layers, layer_n_p] = vertex.GetLayers();
        *layer_n_p = layer_n;
        *layers = nullptr;
        if (layer_n) {
            *layers = new char[levelx_size_ * layer_n]{0};
            for (i32 layer_i = 1; layer_i <= layer_n; ++layer_i) {
                *GetLevelXMut(vertex, layer_i).GetNeighbors().second = 0;
            }
        }
    }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i) const {
        VertexL0 vertex = GetLevel0(vertex_i);
        if (layer_i == 0) {
            return vertex.GetNeighbors();
        }
        return GetLevelX(vertex, layer_i).GetNeighbors();
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i) {
        VertexL0Mut vertex = GetLevel0Mut(vertex_i);
        if (layer_i == 0) {
            return vertex.GetNeighbors();
        }
        return GetLevelXMut(vertex, layer_i).GetNeighbors();
    }

    void SaveGraph(FileHandler &file_handler, SizeT cur_vertex_n) const {
        SizeT layer_sum = 0;
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            layer_sum += GetLevel0(vertex_i).GetLayers().second;
        }
        file_handler.Write(&layer_sum, sizeof(layer_sum));
        file_handler.Write(graph_.get(), cur_vertex_n * level0_size_);
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            VertexL0 vertex = GetLevel0(vertex_i);
            auto [layers, layer_n] = vertex.GetLayers();
            if (layer_n) {
                file_handler.Write(layers, levelx_size_ * layer_n);
            }
        }
    }

    static GraphStoreInner LoadGraph(FileHandler &file_handler, SizeT max_vertex, SizeT Mmax, SizeT Mmax0, SizeT cur_vertex_n) {
        SizeT layer_sum;
        file_handler.Read(&layer_sum, sizeof(layer_sum));

        GraphStoreInner graph_store(max_vertex, Mmax, Mmax0, cur_vertex_n);
        graph_store.loaded_layers_ = MakeUnique<char[]>(graph_store.levelx_size_ * layer_sum);

        file_handler.Read(graph_store.graph_.get(), cur_vertex_n * graph_store.level0_size_);
        char *loaded_layers_p = graph_store.loaded_layers_.get();
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            VertexL0Mut vertex = graph_store.GetLevel0Mut(vertex_i);
            LayerSize layer_n = *vertex.GetLayers().second;
            if (layer_n) {
                file_handler.Read(loaded_layers_p, graph_store.levelx_size_ * layer_n);
                *vertex.GetLayers().first = loaded_layers_p;
                loaded_layers_p += graph_store.levelx_size_ * layer_n;
            } else {
                *vertex.GetLayers().first = nullptr;
            }
        }
        return graph_store;
    }

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------

    // check invariant of graph
    void CheckGraph(SizeT Mmax0, SizeT Mmax, VertexType cur_vertex_n, VertexType vertex_i_offset, VertexType max_vertex_i, i32 &max_l) const {
        int max_layer = -1;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            VertexL0 vertex = GetLevel0(vertex_i);
            VertexType out_vertex_i = vertex_i + vertex_i_offset;
            int cur_max_layer = vertex.GetLayers().second;
            max_layer = std::max(cur_max_layer, max_layer);
            assert(cur_max_layer >= 0 && cur_max_layer <= max_layer);
            auto [neighbors, neighbor_n] = vertex.GetNeighbors();
            assert(neighbor_n <= int(Mmax0));
            for (int i = 0; i < neighbor_n; ++i) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
                VertexType neighbor_idx = neighbors[i];
                assert(neighbor_idx <= max_vertex_i && neighbor_idx >= 0);
                assert(neighbor_idx != out_vertex_i);
#pragma clang diagnostic pop
            }
            for (int layer_i = 1; layer_i <= cur_max_layer; ++layer_i) {
                auto [neighbors, neighbor_n] = GetLevelX(vertex, layer_i).GetNeighbors();
                assert(neighbor_n <= int(Mmax));
                for (int i = 0; i < neighbor_n; ++i) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
                    VertexType neighbor_idx = neighbors[i];
                    assert(neighbor_idx <= max_vertex_i && neighbor_idx >= 0);
                    assert(neighbor_idx != out_vertex_i);

                    // int n_layer = GetLevel0(neighbor_idx).GetLayers().second;
                    // assert(n_layer >= layer_i);
#pragma clang diagnostic pop
                }
            }
        }
        max_l = max_layer;
    }

    void Dump(std::ostream &os, VertexType cur_vertex_n) {
        if (cur_vertex_n == 0) {
            return;
        }
        i32 max_layer = 0;
        Vector<Vector<VertexType>> layer2vertex;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            int layer_n = GetLevel0(vertex_i).GetLayers().second;
            max_layer = std::max(max_layer, layer_n);
            if (max_layer >= i32(layer2vertex.size())) {
                layer2vertex.resize(max_layer + 1);
            }
            for (i32 layer_i = 0; layer_i <= layer_n; ++layer_i) {
                layer2vertex[layer_i].emplace_back(vertex_i);
            }
        }
        for (i32 layer = max_layer; layer >= 0; --layer) {
            os << "layer " << layer << std::endl;
            for (VertexType vertex_i : layer2vertex[layer]) {
                os << vertex_i << ": ";
                const int *neighbors = nullptr;
                int neighbor_n = 0;
                if (layer == 0) {
                    std::tie(neighbors, neighbor_n) = GetLevel0(vertex_i).GetNeighbors();
                } else {
                    std::tie(neighbors, neighbor_n) = GetLevelX(GetLevel0(vertex_i), layer).GetNeighbors();
                }
                for (int i = 0; i < neighbor_n; ++i) {
                    os << neighbors[i] << ", ";
                }
                os << std::endl;
            }
        }
    }
};

} // namespace infinity