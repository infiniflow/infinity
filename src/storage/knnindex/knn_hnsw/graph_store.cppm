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
#include <iostream>
#include <new>

import stl;
import hnsw_common;
import file_system;

export module graph_store;

namespace infinity {

export class GraphStore {
private:
    constexpr static SizeT layer_n_offset_ = 0;
    constexpr static SizeT layers_p_offset_ = AlignTo(layer_n_offset_ + sizeof(LayerSize), sizeof(char *));
    constexpr static SizeT l0_neighbor_n_offset_ = AlignTo(layers_p_offset_ + sizeof(char *), sizeof(VertexListSize));
    constexpr static SizeT l0_neighbors_offset_ = AlignTo(l0_neighbor_n_offset_ + sizeof(VertexListSize), sizeof(VertexType));
    const SizeT level0_size_;

    constexpr static SizeT lx_neighbor_n_offset_ = 0;
    constexpr static SizeT lx_neighbors_offset_ = AlignTo(lx_neighbor_n_offset_ + sizeof(VertexListSize), sizeof(VertexType));
    const SizeT levelx_size_;

    const SizeT max_vertex_num_;
    char *const graph_;
    const SizeT loaded_vertex_n_;
    char *const loaded_layers_;

    i32 max_layer_{};
    VertexType enterpoint_{};

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
    VertexL0Mut GetLevel0Mut(VertexType vertex_i) { return VertexL0Mut(graph_ + level0_size_ * vertex_i); }
    VertexLXMut GetLevelXMut(VertexL0Mut &level0, LayerSize layer_i) { return VertexLXMut(*level0.GetLayers().first + levelx_size_ * (layer_i - 1)); }
    VertexL0 GetLevel0(VertexType vertex_i) const { return VertexL0(graph_ + level0_size_ * vertex_i); }
    VertexLX GetLevelX(const VertexL0 &level0, LayerSize layer_i) const { return VertexLX(level0.GetLayers().first + levelx_size_ * (layer_i - 1)); }

private:
    GraphStore(SizeT max_vertex, SizeT Mmax, SizeT Mmax0, SizeT loaded_vertex_n, char *loaded_layers)
        : level0_size_(AlignTo(l0_neighbors_offset_ + sizeof(VertexType) * Mmax0, 8)),                 //
          levelx_size_(AlignTo(lx_neighbors_offset_ + sizeof(VertexType) * Mmax, 8)),                  //
          max_vertex_num_(max_vertex),                                                                 //
          graph_(static_cast<char *>(operator new[](max_vertex * level0_size_, std::align_val_t(8)))), //
          loaded_vertex_n_(loaded_vertex_n),                                                           //
          loaded_layers_(loaded_layers)                                                                //
                                                                                                       //
    {}

    void Init() {
        max_layer_ = -1;
        enterpoint_ = -1;
        Fill(graph_, graph_ + max_vertex_num_ * level0_size_, 0);
        // for (VertexType vertex_i = 0; vertex_i < max_vertex_num_; ++vertex_i) {
        //     VertexL0Mut vertex = GetLevel0Mut(vertex_i);
        //     *vertex.GetNeighbors().second = 0;
        //     *vertex.GetLayers().first = nullptr;
        // }
    }

public:
    static GraphStore Make(SizeT max_vertex, SizeT Mmax, SizeT Mmax0) {
        auto ret = GraphStore(max_vertex, Mmax, Mmax0, 0, nullptr);
        ret.Init();
        return ret;
    }

    GraphStore(const GraphStore &) = delete;
    GraphStore &operator=(const GraphStore &) = delete;
    GraphStore &operator=(GraphStore &&) = delete;

    GraphStore(GraphStore &&other)
        : level0_size_(other.level0_size_),         //
          levelx_size_(other.levelx_size_),         //
          max_vertex_num_(other.max_vertex_num_),   //
          graph_(other.graph_),                     //
          loaded_vertex_n_(other.loaded_vertex_n_), //
          loaded_layers_(other.loaded_layers_),     //
          max_layer_(other.max_layer_),             //
          enterpoint_(other.enterpoint_)            //
    {
        const_cast<char *&>(other.graph_) = nullptr;
        const_cast<char *&>(other.loaded_layers_) = nullptr;
    }

    ~GraphStore() {
        if (graph_) {
            for (VertexType vertex_i = loaded_vertex_n_; vertex_i < VertexType(max_vertex_num_); ++vertex_i) {
                delete[] GetLevel0(vertex_i).GetLayers().first;
            }
            operator delete[](graph_, std::align_val_t(8));
        }
        if (loaded_layers_) {
            delete[] loaded_layers_;
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
        if (layer_n > max_layer_) {
            max_layer_ = layer_n;
            enterpoint_ = vertex_i;
        }
    }

    i32 max_layer() const { return max_layer_; }

    VertexType enterpoint() const { return enterpoint_; }

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

    void SaveGraph(FileHandler &file_handler, VertexType cur_vertex_n) const {
        file_handler.Write(&max_layer_, sizeof(max_layer_));
        file_handler.Write(&enterpoint_, sizeof(enterpoint_));
        SizeT layer_sum = 0;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            layer_sum += GetLevel0(vertex_i).GetLayers().second;
        }
        file_handler.Write(&layer_sum, sizeof(layer_sum));
        file_handler.Write(graph_, cur_vertex_n * level0_size_);
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            VertexL0 vertex = GetLevel0(vertex_i);
            auto [layers, layer_n] = vertex.GetLayers();
            if (layer_n) {
                file_handler.Write(layers, levelx_size_ * layer_n);
            }
        }
    }

    static GraphStore LoadGraph(FileHandler &file_handler, SizeT max_vertex, SizeT Mmax, SizeT Mmax0, VertexType cur_vertex_n) {
        i32 max_layer;
        file_handler.Read(&max_layer, sizeof(max_layer));
        VertexType enterpoint;
        file_handler.Read(&enterpoint, sizeof(enterpoint));
        SizeT layer_sum;
        file_handler.Read(&layer_sum, sizeof(layer_sum));
        GraphStore graph_store(max_vertex, Mmax, Mmax0, cur_vertex_n, nullptr);
        const_cast<char *&>(graph_store.loaded_layers_) = new char[graph_store.levelx_size_ * layer_sum];

        graph_store.max_layer_ = max_layer;
        graph_store.enterpoint_ = enterpoint;
        file_handler.Read(graph_store.graph_, cur_vertex_n * graph_store.level0_size_);
        char *loaded_layers_p = graph_store.loaded_layers_;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
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
    void CheckGraph(VertexType cur_vertex_n, SizeT Mmax0, SizeT Mmax) const {
        assert(cur_vertex_n <= VertexType(max_vertex_num_));
        int max_layer = -1;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            VertexL0 vertex = GetLevel0(vertex_i);
            int cur_max_layer = vertex.GetLayers().second;
            max_layer = Max(cur_max_layer, max_layer);
            assert(cur_max_layer >= 0 && cur_max_layer <= max_layer_);
            auto [neighbors, neighbor_n] = vertex.GetNeighbors();
            assert(neighbor_n <= int(Mmax0));
            for (int i = 0; i < neighbor_n; ++i) {
                VertexType neighbor_idx = neighbors[i];
                assert(neighbor_idx < cur_vertex_n && neighbor_idx >= 0);
                assert(neighbor_idx != vertex_i);
            }
            for (int layer_i = 1; layer_i <= cur_max_layer; ++layer_i) {
                auto [neighbors, neighbor_n] = GetLevelX(vertex, layer_i).GetNeighbors();
                assert(neighbor_n <= int(Mmax));
                for (int i = 0; i < neighbor_n; ++i) {
                    VertexType neighbor_idx = neighbors[i];
                    assert(neighbor_idx < cur_vertex_n && neighbor_idx >= 0);
                    assert(neighbor_idx != vertex_i);

                    int n_layer = GetLevel0(neighbor_idx).GetLayers().second;
                    assert(n_layer >= layer_i);
                }
            }
        }
        assert(max_layer == max_layer_);
    }

    void Dump(std::ostream &os, VertexType cur_vertex_n) {

        os << "max layer: " << max_layer_ << std::endl;
        os << std::endl;

        Vector<Vector<VertexType>> layer2vertex(max_layer_ + 1);
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            int layer_n = GetLevel0(vertex_i).GetLayers().second;
            for (i32 layer_i = 0; layer_i <= layer_n; ++layer_i) {
                layer2vertex[layer_i].emplace_back(vertex_i);
            }
        }
        for (i32 layer = max_layer_; layer >= 0; --layer) {
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