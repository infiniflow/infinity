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

import stl;
import hnsw_common;
import file_system;

export module graph_store;

namespace infinity {

export template <typename LabelType>
class GraphStore {
public:
    const char *GetLayers(VertexType vertex_idx) const {
        return *reinterpret_cast<const char **>(graph_ + vertex_idx * level0_size_ + layers_offset_);
    }
    char **GetLayersMut(VertexType vertex_idx) { return reinterpret_cast<char **>(graph_ + vertex_idx * level0_size_ + layers_offset_); }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_idx, i32 layer_idx) const {
        char *vertex_p = graph_ + vertex_idx * level0_size_;
        if (layer_idx) {
            const char *layer = GetLayers(vertex_idx) + (layer_idx - 1) * layer_size_;
            return {reinterpret_cast<const VertexType *>(layer + neighbors1_offset_),
                    *reinterpret_cast<const VertexListSize *>(layer + neighbor_n1_offset_)};
        } else {
            return {reinterpret_cast<const VertexType *>(vertex_p + neighbors0_offset_),
                    *reinterpret_cast<const VertexListSize *>(vertex_p + neighbor_n0_offset_)};
        }
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_idx, i32 layer_idx) {
        char *vertex_p = graph_ + vertex_idx * level0_size_;
        if (layer_idx) {
            char *layer = *GetLayersMut(vertex_idx) + (layer_idx - 1) * layer_size_;
            return {reinterpret_cast<VertexType *>(layer + neighbors1_offset_), reinterpret_cast<VertexListSize *>(layer + neighbor_n1_offset_)};
        } else {
            return {reinterpret_cast<VertexType *>(vertex_p + neighbors0_offset_),
                    reinterpret_cast<VertexListSize *>(vertex_p + neighbor_n0_offset_)};
        }
    }

    LayerSize GetLayerN(VertexType vertex_idx) const {
        return *reinterpret_cast<const LayerSize *>(graph_ + vertex_idx * level0_size_ + layer_n_offset_);
    }
    LayerSize *GetLayerNMut(VertexType vertex_idx) { return reinterpret_cast<LayerSize *>(graph_ + vertex_idx * level0_size_ + layer_n_offset_); }

    LabelType GetLabel(VertexType vertex_idx) const {
        return *reinterpret_cast<const LabelType *>(graph_ + vertex_idx * level0_size_ + label_offset_);
    }
    LabelType *GetLabelMut(VertexType vertex_idx) { return reinterpret_cast<LabelType *>(graph_ + vertex_idx * level0_size_ + label_offset_); }

    i32 max_layer() const { return max_layer_; }

    VertexType enterpoint() const { return enterpoint_; }

private:
    GraphStore(SizeT max_vertex, SizeT Mmax, SizeT Mmax0, SizeT loaded_vertex_n, char *loaded_layers)
        : max_vertex_(max_vertex),                                                                       //
          neighbor_n0_offset_(0),                                                                        //
          neighbors0_offset_(AlignTo(neighbor_n0_offset_ + sizeof(VertexListSize), sizeof(VertexType))), //
          layer_n_offset_(AlignTo(neighbors0_offset_ + sizeof(VertexType) * Mmax0, sizeof(LayerSize))),  //
          layers_offset_(AlignTo(layer_n_offset_ + sizeof(LayerSize), sizeof(void *))),                  //
          label_offset_(AlignTo(layers_offset_ + sizeof(void *), sizeof(LabelType))),                    //
          level0_size_(AlignTo(label_offset_ + sizeof(LabelType), 8)),                                   //
          neighbor_n1_offset_(0),                                                                        //
          neighbors1_offset_(AlignTo(sizeof(VertexListSize), sizeof(VertexType))),                       //
          layer_size_(AlignTo(neighbors1_offset_ + sizeof(VertexType) * Mmax, sizeof(VertexListSize))),  //
          loaded_vertex_n_(loaded_vertex_n),                                                             //
          graph_(new char[max_vertex * level0_size_]),                                                   //
          loaded_layers_(loaded_layers)                                                                  //
    {
        max_layer_ = -1;
        enterpoint_ = -1;
        for (VertexType vertex_idx = 0; vertex_idx < max_vertex_; ++vertex_idx) {
            *GetNeighborsMut(vertex_idx, 0).second = 0;
            *GetLayersMut(vertex_idx) = nullptr;
        }
    }

public:
    static GraphStore<LabelType> Make(SizeT max_vertex, SizeT Mmax, SizeT Mmax0) {
        return GraphStore<LabelType>(max_vertex, Mmax, Mmax0, 0, nullptr);
    }

    GraphStore(const GraphStore &) = delete;
    GraphStore &operator=(const GraphStore &) = delete;

    GraphStore(GraphStore &&other)
        : max_vertex_(other.max_vertex_),                 //
          neighbor_n0_offset_(other.neighbor_n0_offset_), //
          neighbors0_offset_(other.neighbors0_offset_),   //
          layer_n_offset_(other.layer_n_offset_),         //
          layers_offset_(other.layers_offset_),           //
          label_offset_(other.label_offset_),             //
          level0_size_(other.level0_size_),               //
          neighbor_n1_offset_(other.neighbor_n1_offset_), //
          neighbors1_offset_(other.neighbors1_offset_),   //
          layer_size_(other.layer_size_),                 //
          loaded_vertex_n_(other.loaded_vertex_n_),       //
          graph_(other.graph_),                           //
          loaded_layers_(other.loaded_layers_)            //
    {
        max_layer_ = other.max_layer_;
        enterpoint_ = other.enterpoint_;
        const_cast<char *&>(other.graph_) = nullptr;
        const_cast<char *&>(other.loaded_layers_) = nullptr;
    }
    GraphStore &operator=(GraphStore &&) = delete;

    ~GraphStore() {
        if (graph_) {
            for (VertexType vertex_idx = loaded_vertex_n_; vertex_idx < max_vertex_; ++vertex_idx) {
                delete[] GetLayers(vertex_idx);
            }
            delete[] graph_;
        }
        if (loaded_layers_) {
            delete[] loaded_layers_;
        }
    }

private:
    const SizeT max_vertex_;

    const SizeT neighbor_n0_offset_;
    const SizeT neighbors0_offset_;
    const SizeT layer_n_offset_;
    const SizeT layers_offset_;
    const SizeT label_offset_;
    const SizeT level0_size_;

    const SizeT neighbor_n1_offset_;
    const SizeT neighbors1_offset_;
    const SizeT layer_size_;

    const SizeT loaded_vertex_n_;

    char *const graph_;
    char *const loaded_layers_;

    i32 max_layer_{};
    VertexType enterpoint_{};

public:
    void AddVertex(VertexType vertex_idx, i32 layer_n, LabelType label) {
        *GetNeighborsMut(vertex_idx, 0).second = 0;
        *GetLabelMut(vertex_idx) = label;
        *GetLayerNMut(vertex_idx) = layer_n;
        if (layer_n) {
            *GetLayersMut(vertex_idx) = new char[layer_size_ * layer_n];
            for (i32 layer_idx = 1; layer_idx <= layer_n; ++layer_idx) {
                *GetNeighborsMut(vertex_idx, layer_idx).second = 0;
            }
        }
        if (layer_n > max_layer_) {
            max_layer_ = layer_n;
            enterpoint_ = vertex_idx;
        }
    }

    void SaveGraph(FileHandler &file_handler, VertexType cur_vertex_n) {
        file_handler.Write(&max_layer_, sizeof(max_layer_));
        file_handler.Write(&enterpoint_, sizeof(enterpoint_));
        file_handler.Write(graph_, cur_vertex_n * level0_size_);
        SizeT layer_sum = 0;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n; ++vertex_idx) {
            layer_sum += GetLayerN(vertex_idx);
        }
        file_handler.Write(&layer_sum, sizeof(layer_sum));
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n; ++vertex_idx) {
            LayerSize layer_n = GetLayerN(vertex_idx);
            if (layer_n) {
                file_handler.Write(GetLayers(vertex_idx), layer_size_ * layer_n);
            }
        }
    }

    static GraphStore<LabelType> LoadGraph(FileHandler &file_handler, SizeT max_vertex, SizeT Mmax, SizeT Mmax0, VertexType cur_vertex_n) {
        i32 max_layer;
        file_handler.Read(&max_layer, sizeof(max_layer));
        VertexType enterpoint;
        file_handler.Read(&enterpoint, sizeof(enterpoint));

        GraphStore<LabelType> graph_store(max_vertex, Mmax, Mmax0, cur_vertex_n, nullptr);
        graph_store.max_layer_ = max_layer;
        graph_store.enterpoint_ = enterpoint;

        file_handler.Read(graph_store.graph_, cur_vertex_n * graph_store.level0_size_);

        SizeT layer_sum;
        file_handler.Read(&layer_sum, sizeof(layer_sum));

        const_cast<char *&>(graph_store.loaded_layers_) = new char[layer_sum * graph_store.layer_size_];
        char *loaded_layers_p = graph_store.loaded_layers_;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n; ++vertex_idx) {
            LayerSize layer_n = graph_store.GetLayerN(vertex_idx);
            if (layer_n) {
                file_handler.Read(loaded_layers_p, layer_n * graph_store.layer_size_);
                *graph_store.GetLayersMut(vertex_idx) = loaded_layers_p;
                loaded_layers_p += layer_n * graph_store.layer_size_;
            } else {
                *graph_store.GetLayersMut(vertex_idx) = nullptr;
            }
        }
        return graph_store;
    }

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------

    // check invariant of graph
    void CheckGraph(VertexType cur_vertex_n, SizeT Mmax0, SizeT Mmax) const {
        assert(cur_vertex_n <= max_vertex_);
        int max_layer = -1;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n; ++vertex_idx) {
            int cur_max_layer = GetLayerN(vertex_idx);
            max_layer = Max(cur_max_layer, max_layer);
            assert(cur_max_layer >= 0 && cur_max_layer <= max_layer_);
            for (int layer_idx = 0; layer_idx <= cur_max_layer; ++layer_idx) {
                auto [neighbors, neighbor_n] = GetNeighbors(vertex_idx, layer_idx);
                VertexType Mmax = layer_idx == 0 ? Mmax0 : Mmax;
                assert(neighbor_n <= Mmax);
                for (int i = 0; i < neighbor_n; ++i) {
                    VertexType neighbor_idx = neighbors[i];
                    assert(neighbor_idx < cur_vertex_n && neighbor_idx >= 0);
                    assert(neighbor_idx != vertex_idx);
                    auto [n_neighbors, n_neighbor_n] = GetNeighbors(neighbor_idx, layer_idx);
                    assert(n_neighbor_n <= Mmax);
                }
            }
        }
        assert(max_layer == max_layer_);
    }

    void Dump(std::ostream &os, VertexType cur_vertex_n) {

        os << "max layer: " << max_layer_ << std::endl;
        os << std::endl;

        Vector<Vector<VertexType>> layer2vertex(max_layer_ + 1);
        for (VertexType v = 0; v < cur_vertex_n; ++v) {
            i32 layer_n = GetLayerN(v);
            for (i32 layer_idx = 0; layer_idx <= layer_n; ++layer_idx) {
                layer2vertex[layer_idx].emplace_back(v);
            }
        }
        for (i32 layer = max_layer_; layer >= 0; --layer) {
            os << "layer " << layer << std::endl;
            for (VertexType v : layer2vertex[layer]) {
                os << v << ": ";
                auto [neighbors, neighbor_n] = GetNeighbors(v, layer);
                for (int i = 0; i < neighbor_n; ++i) {
                    os << neighbors[i] << ", ";
                }
                os << std::endl;
            }
        }
    }
};

} // namespace infinity