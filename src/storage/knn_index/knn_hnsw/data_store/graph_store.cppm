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

export module graph_store;

import stl;
import hnsw_common;
import file_system;

namespace infinity {

struct VertexL0 {
    LayerSize layer_n_;
    char *layers_p_;
    VertexListSize neighbor_n_;
    VertexType neighbors_[];
};
struct VertexLX {
    VertexListSize neighbor_n_;
    VertexType neighbors_[];
};

export class GraphStoreMeta {
private:
    GraphStoreMeta(SizeT Mmax0, SizeT Mmax)
        : Mmax0_(Mmax0), Mmax_(Mmax), level0_size_(sizeof(VertexL0) + sizeof(VertexType) * Mmax0),
          levelx_size_(sizeof(VertexLX) + sizeof(VertexType) * Mmax) {}

public:
    GraphStoreMeta() : Mmax0_(0), Mmax_(0), level0_size_(0), levelx_size_(0), max_layer_(-1), enterpoint_(-1) {}
    GraphStoreMeta(GraphStoreMeta &&other)
        : Mmax0_(std::exchange(other.Mmax0_, 0)), Mmax_(std::exchange(other.Mmax_, 0)), level0_size_(std::exchange(other.level0_size_, 0)),
          levelx_size_(std::exchange(other.levelx_size_, 0)), max_layer_(std::exchange(other.max_layer_, -1)),
          enterpoint_(std::exchange(other.enterpoint_, -1)) {}
    GraphStoreMeta &operator=(GraphStoreMeta &&other) {
        Mmax0_ = std::exchange(other.Mmax0_, 0);
        Mmax_ = std::exchange(other.Mmax_, 0);
        level0_size_ = std::exchange(other.level0_size_, 0);
        levelx_size_ = std::exchange(other.levelx_size_, 0);
        max_layer_ = std::exchange(other.max_layer_, -1);
        enterpoint_ = std::exchange(other.enterpoint_, -1);
        return *this;
    }
    ~GraphStoreMeta() = default;

    static GraphStoreMeta Make(SizeT Mmax0, SizeT Mmax) {
        GraphStoreMeta meta(Mmax0, Mmax);
        meta.max_layer_ = -1;
        meta.enterpoint_ = -1;
        return meta;
    }

    void Save(FileHandler &file_handler) const {
        file_handler.Write(&Mmax0_, sizeof(Mmax0_));
        file_handler.Write(&Mmax_, sizeof(Mmax_));

        file_handler.Write(&max_layer_, sizeof(max_layer_));
        file_handler.Write(&enterpoint_, sizeof(enterpoint_));
    }

    static GraphStoreMeta Load(FileHandler &file_handler) {
        SizeT Mmax0, Mmax;
        file_handler.Read(&Mmax0, sizeof(Mmax0));
        file_handler.Read(&Mmax, sizeof(Mmax));

        GraphStoreMeta meta(Mmax0, Mmax);
        i32 max_layer;
        VertexType enterpoint;
        file_handler.Read(&max_layer, sizeof(max_layer));
        file_handler.Read(&enterpoint, sizeof(enterpoint));
        meta.max_layer_ = max_layer;
        meta.enterpoint_ = enterpoint;
        return meta;
    }

    SizeT Mmax0() const { return Mmax0_; }
    SizeT Mmax() const { return Mmax_; }
    SizeT level0_size() const { return level0_size_; }
    SizeT levelx_size() const { return levelx_size_; }

    Pair<i32, VertexType> GetEnterPoint() const {
        std::unique_lock lck(mtx_);
        return {max_layer_, enterpoint_};
    }

    Pair<i32, VertexType> TryUpdateEnterPoint(i32 layer, VertexType vertex_i) {
        std::unique_lock lck(mtx_);
        if (layer > max_layer_) {
            i32 old_max_layer = max_layer_;
            VertexType old_enterpoint = enterpoint_;
            max_layer_ = layer;
            enterpoint_ = vertex_i;
            return {old_max_layer, old_enterpoint};
        } else {
            return {max_layer_, enterpoint_};
        }
    }

private:
    SizeT Mmax0_;
    SizeT Mmax_;
    SizeT level0_size_;
    SizeT levelx_size_;

    mutable std::mutex mtx_;
    i32 max_layer_;
    VertexType enterpoint_;

public:
    void Dump(std::ostream &os) const {
        auto [max_layer, enterpoint] = GetEnterPoint();
        os << "[CONST] Mmax0: " << Mmax0_ << ", Mmax: " << Mmax_ << ", level0_size: " << level0_size_ << ", levelx_size: " << levelx_size_
           << std::endl;
        os << "max_layer: " << max_layer << ", enterpoint: " << enterpoint << std::endl;
    }
};

export class GraphStoreInner {
private:
    GraphStoreInner(SizeT max_vertex, const GraphStoreMeta &meta, SizeT loaded_vertex_n)
        : graph_(MakeUnique<char[]>(max_vertex * meta.level0_size())), loaded_vertex_n_(loaded_vertex_n) {}

public:
    GraphStoreInner() = default;

    void Free(SizeT current_vertex_num, const GraphStoreMeta &meta) {
        for (VertexType vertex_i = loaded_vertex_n_; vertex_i < VertexType(current_vertex_num); ++vertex_i) {
            delete[] GetLevel0(vertex_i, meta)->layers_p_;
        }
    }

    static GraphStoreInner Make(SizeT max_vertex, const GraphStoreMeta &meta) {
        GraphStoreInner graph_store(max_vertex, meta, 0);
        std::fill(graph_store.graph_.get(), graph_store.graph_.get() + max_vertex * meta.level0_size(), 0);
        return graph_store;
    }

    void Save(FileHandler &file_handler, SizeT cur_vertex_n, const GraphStoreMeta &meta) const {
        SizeT layer_sum = 0;
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            layer_sum += GetLevel0(vertex_i, meta)->layer_n_;
        }
        file_handler.Write(&layer_sum, sizeof(layer_sum));
        file_handler.Write(graph_.get(), cur_vertex_n * meta.level0_size());
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            const VertexL0 *v = GetLevel0(vertex_i, meta);
            if (v->layer_n_) {
                file_handler.Write(v->layers_p_, meta.levelx_size() * v->layer_n_);
            }
        }
    }

    static GraphStoreInner Load(FileHandler &file_handler, SizeT cur_vertex_n, SizeT max_vertex, const GraphStoreMeta &meta) {
        assert(cur_vertex_n <= max_vertex);

        SizeT layer_sum;
        file_handler.Read(&layer_sum, sizeof(layer_sum));

        GraphStoreInner graph_store(max_vertex, meta, cur_vertex_n);
        file_handler.Read(graph_store.graph_.get(), cur_vertex_n * meta.level0_size());

        auto loaded_layers = MakeUnique<char[]>(meta.levelx_size() * layer_sum);
        char *loaded_layers_p = loaded_layers.get();
        for (VertexType vertex_i = 0; vertex_i < (VertexType)cur_vertex_n; ++vertex_i) {
            VertexL0 *v = graph_store.GetLevel0(vertex_i, meta);
            if (v->layer_n_) {
                file_handler.Read(loaded_layers_p, meta.levelx_size() * v->layer_n_);
                v->layers_p_ = loaded_layers_p;
                loaded_layers_p += meta.levelx_size() * v->layer_n_;
            } else {
                v->layers_p_ = nullptr;
            }
        }
        graph_store.loaded_layers_ = std::move(loaded_layers);
        return graph_store;
    }

    void AddVertex(VertexType vertex_i, i32 layer_n, const GraphStoreMeta &meta) {
        VertexL0 *v = GetLevel0(vertex_i, meta);
        v->neighbor_n_ = 0;
        v->layer_n_ = layer_n;
        if (layer_n) {
            v->layers_p_ = new char[meta.levelx_size() * layer_n];
            for (i32 layer_i = 1; layer_i <= layer_n; ++layer_i) {
                GetLevelX(v->layers_p_, layer_i, meta)->neighbor_n_ = 0;
            }
        } else {
            v->layers_p_ = nullptr;
        }
    }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i, const GraphStoreMeta &meta) const {
        const VertexL0 *v = GetLevel0(vertex_i, meta);
        if (layer_i == 0) {
            return {v->neighbors_, v->neighbor_n_};
        }
        const VertexLX *vx = GetLevelX(v->layers_p_, layer_i, meta);
        return {vx->neighbors_, vx->neighbor_n_};
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i, const GraphStoreMeta &meta) {
        VertexL0 *v = GetLevel0(vertex_i, meta);
        if (layer_i == 0) {
            return {v->neighbors_, &v->neighbor_n_};
        }
        VertexLX *vx = GetLevelX(v->layers_p_, layer_i, meta);
        return {vx->neighbors_, &vx->neighbor_n_};
    }

private:
    const VertexL0 *GetLevel0(VertexType vertex_i, const GraphStoreMeta &meta) const {
        return reinterpret_cast<const VertexL0 *>(graph_.get() + vertex_i * meta.level0_size());
    }
    VertexL0 *GetLevel0(VertexType vertex_i, const GraphStoreMeta &meta) {
        return reinterpret_cast<VertexL0 *>(graph_.get() + vertex_i * meta.level0_size());
    }

    const VertexLX *GetLevelX(const char *layer_p, i32 layer_i, const GraphStoreMeta &meta) const {
        assert(layer_i > 0);
        return reinterpret_cast<const VertexLX *>(layer_p + (layer_i - 1) * meta.levelx_size());
    }
    VertexLX *GetLevelX(char *layer_p, i32 layer_i, const GraphStoreMeta &meta) {
        assert(layer_i > 0);
        return reinterpret_cast<VertexLX *>(layer_p + (layer_i - 1) * meta.levelx_size());
    }

private:
    UniquePtr<char[]> graph_;
    SizeT loaded_vertex_n_;
    UniquePtr<char[]> loaded_layers_;

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------

public:
// check invariant of graph
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    void Check(VertexType cur_vertex_n, const GraphStoreMeta &meta, VertexType vertex_i_offset, SizeT cur_vec_num, i32 &max_l) const {
        int max_layer = -1;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            const VertexL0 *v = GetLevel0(vertex_i, meta);
            VertexType out_vertex_i = vertex_i + vertex_i_offset;
            assert(v->layer_n_ >= 0);
            max_layer = std::max(v->layer_n_, max_layer);
            for (int i = 0; i < v->neighbor_n_; ++i) {
                VertexType neighbor_idx = v->neighbors_[i];
                assert(neighbor_idx < (VertexType)cur_vec_num && neighbor_idx >= 0);
                assert(neighbor_idx != out_vertex_i);
            }
            for (int layer_i = 1; layer_i <= v->layer_n_; ++layer_i) {
                const VertexLX *vx = GetLevelX(v->layers_p_, layer_i, meta);
                for (int i = 0; i < vx->neighbor_n_; ++i) {
                    VertexType neighbor_idx = vx->neighbors_[i];
                    assert(neighbor_idx < (VertexType)cur_vec_num && neighbor_idx >= 0);
                    assert(neighbor_idx != out_vertex_i);
                }
            }
        }
        max_l = max_layer;
    }
#pragma clang diagnostic pop

    void Dump(std::ostream &os, VertexType cur_vertex_n, const GraphStoreMeta &meta) const {
        if (cur_vertex_n == 0) {
            return;
        }
        i32 max_layer = 0;
        Vector<Vector<VertexType>> layer2vertex;
        for (VertexType vertex_i = 0; vertex_i < cur_vertex_n; ++vertex_i) {
            const VertexL0 *v = GetLevel0(vertex_i, meta);
            max_layer = std::max(max_layer, v->layer_n_);
            if (max_layer >= i32(layer2vertex.size())) {
                layer2vertex.resize(max_layer + 1);
            }
            for (i32 layer_i = 0; layer_i <= v->layer_n_; ++layer_i) {
                layer2vertex[layer_i].emplace_back(vertex_i);
            }
        }
        for (i32 layer = max_layer; layer >= 0; --layer) {
            os << "layer " << layer << std::endl;
            for (VertexType vertex_i : layer2vertex[layer]) {
                os << vertex_i << ": ";
                const int *neighbors = nullptr;
                int neighbor_n = 0;

                const VertexL0 *v = GetLevel0(vertex_i, meta);
                if (layer == 0) {
                    neighbors = v->neighbors_;
                    neighbor_n = v->neighbor_n_;
                } else {
                    const VertexLX *vx = GetLevelX(v->layers_p_, layer, meta);
                    neighbors = vx->neighbors_;
                    neighbor_n = vx->neighbor_n_;
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
