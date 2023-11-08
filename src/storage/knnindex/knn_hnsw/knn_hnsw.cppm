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

#include "storage/knnindex/header.h"
#include <cassert>
#include <random>
#include <algorithm>

import cmp;
import stl;
import dist_func;
import file_system;
import file_system_type;
import hnsw_mem_pool;

export module knn_hnsw;

namespace infinity {

export template <typename DataType>
class KnnHnsw {
public:
    static SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

    using LabelType = u32;

    using VertexType = i32;

    using PDV = Pair<DataType, VertexType>;
    using CMP = CompareByFirst<DataType, VertexType>;
    using DistHeap = Heap<PDV, CMP>;

    using VertexListSize = i32;
    using LayerSize = i32;

private:
    const SizeT max_vertex_;
    const SizeT dim_;
    const SizeT M_;
    const SizeT Mmax_;
    const SizeT Mmax0_;
    SizeT ef_;
    const SizeT ef_construction_;
    const DistFunc<DataType> dist_func_{};

    SizeT cur_vertex_n_{};
    i32 max_layer_{};
    VertexType enterpoint_{};

    // 1 / log(1.0 * M_)
    const double mult_;
    std::default_random_engine level_rng_{};

    // The offset of data structure. Init not in initialize list. const
    const SizeT data_offset_;
    const SizeT neighbor_n0_offset_;
    const SizeT neighbors0_offset_;
    const SizeT layer_n_offset_;
    const SizeT layers_offset_;
    const SizeT label_offset_;
    const SizeT level0_size_;

    const SizeT neighbor_n1_offset_;
    const SizeT neighbors1_offset_;
    const SizeT layer_size_;
    char *graph_{};

    const SizeT loaded_vertex_n_;
    const char *loaded_layers_;

    // VisitedMemPool visited_pool_;

    // using DistHeapPool = MaxHeapMemPool<PDV, CMP>;
    // using PooledDistHeap = DistHeapPool::PooledT;
    // DistHeapPool dist_heap_pool_;

private:
    const DataType *GetData(VertexType vertex_idx) const {
        return reinterpret_cast<const DataType *>(graph_ + vertex_idx * level0_size_ + data_offset_);
    }
    DataType *GetDataMut(VertexType vertex_idx) { return reinterpret_cast<DataType *>(graph_ + vertex_idx * level0_size_ + data_offset_); }

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

public:
    template <typename SpaceType>
        requires SpaceConcept<DataType, SpaceType>
    KnnHnsw(SizeT max_vertex, SizeT dim, SpaceType space, SizeT M, SizeT ef_construction, SizeT random_seed = 100)
        : max_vertex_(max_vertex), dim_(dim),                                                            //
          M_(M), Mmax_(M_), Mmax0_(2 * Mmax_), ef_construction_(Max(M_, ef_construction)),               //
          dist_func_(space.DistFuncPtr()),                                                               //
          mult_(1 / std::log(1.0 * M_)),                                                                 //
          data_offset_(0),                                                                               //
          neighbor_n0_offset_(AlignTo(sizeof(DataType) * dim_, sizeof(VertexListSize))),                 //
          neighbors0_offset_(AlignTo(neighbor_n0_offset_ + sizeof(VertexListSize), sizeof(VertexType))), //
          layer_n_offset_(AlignTo(neighbors0_offset_ + sizeof(VertexType) * Mmax0_, sizeof(LayerSize))), //
          layers_offset_(AlignTo(layer_n_offset_ + sizeof(LayerSize), sizeof(void *))),                  //
          label_offset_(AlignTo(layers_offset_ + sizeof(void *), sizeof(LabelType))),                    //
          level0_size_(AlignTo(label_offset_ + sizeof(LabelType), 8)),                                   //
          neighbor_n1_offset_(0),                                                                        //
          neighbors1_offset_(AlignTo(sizeof(VertexListSize), sizeof(VertexType))),                       //
          layer_size_(AlignTo(neighbors1_offset_ + sizeof(VertexType) * Mmax_, sizeof(VertexListSize))), //
          loaded_vertex_n_(0), loaded_layers_(nullptr)                                                   //
    {
        ef_ = 10;
        cur_vertex_n_ = 0;
        max_layer_ = -1;
        enterpoint_ = -1;
        level_rng_.seed(random_seed);
        graph_ = new char[max_vertex_ * level0_size_];
    }

    ~KnnHnsw() {
        for (VertexType vertex_idx = loaded_vertex_n_; vertex_idx < cur_vertex_n_; ++vertex_idx) {
            delete[] GetLayers(vertex_idx);
        }
        delete[] graph_;
        if (loaded_layers_) {
            delete[] loaded_layers_;
        }
    }

private:
    void SetLoadLayer(SizeT loaded_vertex_n, char *loaded_layers) {
        const_cast<SizeT &>(loaded_vertex_n_) = loaded_vertex_n;
        const_cast<char *&>(loaded_layers_) = loaded_layers;
    }

    // >= 0
    i32 GenerateRandomLayer() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double r = -std::log(distribution(level_rng_)) * mult_;
        return static_cast<i32>(r);
    }

    VertexType GenerateNewVertexIdx() { return cur_vertex_n_++; }

    void InitVertex(VertexType vertex_idx, i32 layer_n, const DataType *data, LabelType label) {
        DataType *vertex_data = GetDataMut(vertex_idx);
        std::copy(data, data + dim_, vertex_data); // TODO:: memcpy
        *GetNeighborsMut(vertex_idx, 0).second = 0;
        *GetLabelMut(vertex_idx) = label;
        *GetLayerNMut(vertex_idx) = layer_n;
        if (layer_n) {
            *GetLayersMut(vertex_idx) = new char[layer_size_ * layer_n];
            for (i32 layer_idx = 1; layer_idx <= layer_n; ++layer_idx) {
                *GetNeighborsMut(vertex_idx, layer_idx).second = 0;
            }
        } else [[likely]] {
            *GetLayersMut(vertex_idx) = nullptr;
        }
    }

public:
    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    // return value is a max heap of distance
    DistHeap SearchLayer(VertexType enter_point, const DataType *query, i32 layer_idx, SizeT candidate_n) {
        DistHeap result;    // TODO:: use faiss heap
        DistHeap candidate; // TODO:: use pool
        // TODO:: reserve heap size

        DataType dist = dist_func_(query, GetData(enter_point), dim_);

        candidate.emplace(-dist, enter_point);
        result.emplace(dist, enter_point);
        // auto pooled_visited = visited_pool_.Get(max_vertex_, cur_vertex_n_);
        // Vector<bool> &visited = *pooled_visited;

        Vector<bool> visited(cur_vertex_n_, false);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (-minus_c_dist > result.top().first && result.size() == candidate_n) {
                break;
            }
            const auto [neighbors_p, neighbor_size] = GetNeighbors(c_idx, layer_idx);
            // TODO:: prefetch visited
#ifdef USE_SSE
            if (neighbor_size) [[likely]]
                _mm_prefetch(GetData(neighbors_p[neighbor_size - 1]), _MM_HINT_T0);
#endif
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
#ifdef USE_SSE
                if (i - 1 >= 0) [[likely]]
                    _mm_prefetch(GetData(neighbors_p[i - 1]), _MM_HINT_T0);
#endif
                if (visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                // TODO:: store result.top(), result.size() in variable
                dist = dist_func_(query, GetData(n_idx), dim_);
                if (dist < result.top().first || result.size() < candidate_n) {
                    candidate.emplace(-dist, n_idx);
                    result.emplace(dist, n_idx);
                    if (result.size() > candidate_n) {
                        result.pop();
                    }
                }
            }
        }
        return result;
    }

    VertexType SearchLayerNearest(VertexType enter_point, const DataType *query, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        DataType cur_dist = dist_func_(query, GetData(enter_point), dim_);
        bool check = true;
        while (check) {
            check = false;
            const auto [neighbors_p, neighbor_size] = GetNeighbors(cur_p, layer_idx);
#ifdef USE_SSE
            if (neighbor_size) [[likely]]
                _mm_prefetch(GetData(neighbors_p[neighbor_size - 1]), _MM_HINT_T0);
#endif
            for (int i = neighbor_size - 1; i >= 0; --i) {
#ifdef USE_SSE
                if (i - 1 >= 0) [[likely]]
                    _mm_prefetch(GetData(neighbors_p[i - 1]), _MM_HINT_T0);
#endif
                VertexType n_idx = neighbors_p[i];
                DataType n_dist = dist_func_(query, GetData(n_idx), dim_);
                if (n_dist < cur_dist) {
                    cur_p = n_idx;
                    cur_dist = n_dist;
                    check = true;
                }
            }
        }
        return cur_p;
    }

    // DistHeap is the min heap whose key is the minus distance to query
    // result distance is increasing
    // TODO:: write directly into query's neighbors

    void SelectNeighborsHeuristic(DistHeap &candidates, SizeT M, VertexType *result_p, VertexListSize *result_size_p) const {
        VertexListSize result_size = 0;
        if (SizeT c_size = candidates.size(); c_size < M) {
            while (!candidates.empty()) {
                result_p[result_size++] = candidates.top().second;
                candidates.pop();
            }
        } else {
            while (!candidates.empty() && result_size < M) { // TODO:: store result.size() into variable
                const auto &[minus_c_dist, c_idx] = candidates.top();
                const DataType *c_data = GetData(c_idx);
                bool check = true;
#ifdef USE_SSE
                if (!result_size) [[likely]]
                    _mm_prefetch(GetData(result_p[0]), _MM_HINT_T0);
#endif
                for (SizeT i = 0; i < result_size; ++i) {
#ifdef USE_SSE
                    if (i + 1 < result_size) [[likely]]
                        _mm_prefetch(GetData(result_p[i + 1]), _MM_HINT_T0);
#endif
                    VertexType r_idx = result_p[i];
                    DataType cr_dist = dist_func_(c_data, GetData(r_idx), dim_);
                    if (cr_dist < -minus_c_dist) {
                        check = false;
                        break;
                    }
                }
                if (check) {
                    result_p[result_size++] = c_idx;
                }
                candidates.pop();
            }
        }
        *result_size_p = result_size;
    }

    void ConnectNeighbors(VertexType vertex_idx, const VertexType *q_neighbors_p, VertexListSize q_neighbor_size, i32 layer_idx) {
        for (int i = 0; i < q_neighbor_size; ++i) {
            VertexType n_idx = q_neighbors_p[i];
            auto [n_neighbors_p, n_neighbor_size_p] = GetNeighborsMut(n_idx, layer_idx);
            VertexListSize n_neighbor_size = *n_neighbor_size_p;
            SizeT Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
            if (n_neighbor_size < Mmax) {
                *(n_neighbors_p + n_neighbor_size) = vertex_idx;
                *n_neighbor_size_p = n_neighbor_size + 1;
                continue;
            }
            const DataType *n_data = GetData(n_idx);
            DataType n_dist = dist_func_(n_data, GetData(vertex_idx), dim_);

            Vector<PDV> tmp;
            tmp.reserve(n_neighbor_size + 1);
            tmp.emplace_back(-n_dist, vertex_idx);
#ifdef USE_SSE
            if (n_neighbor_size) [[likely]]
                _mm_prefetch(GetData(n_neighbors_p[0]), _MM_HINT_T0);
#endif
            for (int i = 0; i < n_neighbor_size; ++i) {
#ifdef USE_SSE
                if (i + 1 < n_neighbor_size) [[likely]]
                    _mm_prefetch(GetData(n_neighbors_p[i + 1]), _MM_HINT_T0);
#endif
                tmp.emplace_back(-dist_func_(n_data, GetData(n_neighbors_p[i]), dim_), n_neighbors_p[i]);
            }

            DistHeap candidates(tmp.begin(), tmp.end());
            SelectNeighborsHeuristic(candidates, Mmax, n_neighbors_p, n_neighbor_size_p); // write in memory
        }
    }

    void Insert(const DataType *query, LabelType label) {
        VertexType q_vertex = GenerateNewVertexIdx();
        i32 q_layer = GenerateRandomLayer();
        InitVertex(q_vertex, q_layer, query, label);
        VertexType ep = enterpoint_;
        for (i32 cur_layer = max_layer_; cur_layer > q_layer; --cur_layer) {
            ep = SearchLayerNearest(ep, query, cur_layer);
        }
        for (i32 cur_layer = Min(q_layer, max_layer_); cur_layer >= 0; --cur_layer) {
            DistHeap search_result = SearchLayer(ep, query, cur_layer, ef_construction_); // TODO:: use pool
            DistHeap candidates;
            while (!search_result.empty()) {
                const auto &[dist, idx] = search_result.top();
                candidates.emplace(-dist, idx);
                search_result.pop();
            }
            const auto [q_neighbors_p, q_neighbor_size_p] = GetNeighborsMut(q_vertex, cur_layer);
            SelectNeighborsHeuristic(candidates, M_, q_neighbors_p, q_neighbor_size_p);
            ep = q_neighbors_p[0];
            ConnectNeighbors(q_vertex, q_neighbors_p, *q_neighbor_size_p, cur_layer);
        }
        if (q_layer > max_layer_) {
            max_layer_ = q_layer;
            enterpoint_ = q_vertex;
        }
    }

    MaxHeap<Pair<DataType, LabelType>> KnnSearch(const DataType *query, SizeT k) {
        VertexType ep = enterpoint_;
        for (i32 cur_layer = max_layer_; cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest(ep, query, cur_layer);
        }
        DistHeap search_result = SearchLayer(ep, query, 0, Max(k, ef_));
        while (search_result.size() > k) {
            search_result.pop();
        }
        MaxHeap<Pair<DataType, LabelType>> result; // TODO:: reserve
        while (!search_result.empty()) {
            const auto &[dist, idx] = search_result.top();
            result.emplace(dist, GetLabel(idx));
            search_result.pop();
        }
        return result;
    }

    void SetEf(SizeT ef) { ef_ = ef; }

    void SaveIndex(const String &file_path, UniquePtr<FileSystem> fs) {
        u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        UniquePtr<FileHandler> file_handler = fs->OpenFile(file_path, file_flags, FileLockType::kWriteLock);
        fs->Write(*file_handler, &max_vertex_, sizeof(max_vertex_));
        fs->Write(*file_handler, &dim_, sizeof(dim_));
        fs->Write(*file_handler, &M_, sizeof(M_));
        fs->Write(*file_handler, &ef_construction_, sizeof(ef_construction_));
        fs->Write(*file_handler, &cur_vertex_n_, sizeof(cur_vertex_n_));
        fs->Write(*file_handler, &max_layer_, sizeof(max_layer_));
        fs->Write(*file_handler, &enterpoint_, sizeof(enterpoint_));

        fs->Write(*file_handler, graph_, cur_vertex_n_ * level0_size_);

        SizeT layer_sum = 0;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n_; ++vertex_idx) {
            layer_sum += GetLayerN(vertex_idx);
        }
        fs->Write(*file_handler, &layer_sum, sizeof(layer_sum));

        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n_; ++vertex_idx) {
            LayerSize layer_n = GetLayerN(vertex_idx);
            if (layer_n) {
                const char *layer_p = GetLayers(vertex_idx);
                fs->Write(*file_handler, layer_p, layer_size_ * layer_n);
            }
        }
        file_handler->Close();
    }

    template <typename SpaceType>
        requires SpaceConcept<DataType, SpaceType>
    static UniquePtr<KnnHnsw<DataType>>
    LoadIndex(const String &file_path, UniquePtr<FileSystem> fs, SpaceType space, SizeT max_vertex_new = 0, SizeT random_seed = 100) {
        u8 file_flags = FileFlags::READ_FLAG;
        UniquePtr<FileHandler> file_handler = fs->OpenFile(file_path, file_flags, FileLockType::kReadLock);
        SizeT max_vertex;
        fs->Read(*file_handler, &max_vertex, sizeof(max_vertex));
        if (max_vertex_new > max_vertex) {
            max_vertex = max_vertex_new;
        }
        SizeT dim;
        fs->Read(*file_handler, &dim, sizeof(dim));
        SizeT M;
        fs->Read(*file_handler, &M, sizeof(M));
        SizeT ef_construction;
        fs->Read(*file_handler, &ef_construction, sizeof(ef_construction));
        SizeT cur_vertex_n;
        fs->Read(*file_handler, &cur_vertex_n, sizeof(cur_vertex_n));
        i32 max_layer;
        fs->Read(*file_handler, &max_layer, sizeof(max_layer));
        VertexType enterpoint;
        fs->Read(*file_handler, &enterpoint, sizeof(enterpoint));

        auto index = MakeUnique<KnnHnsw<DataType>>(max_vertex, dim, space, M, ef_construction, random_seed);
        index->cur_vertex_n_ = cur_vertex_n;
        index->max_layer_ = max_layer;
        index->enterpoint_ = enterpoint;

        fs->Read(*file_handler, index->graph_, cur_vertex_n * index->level0_size_);
        // check invariant of grap

        SizeT layer_sum;
        fs->Read(*file_handler, &layer_sum, sizeof(layer_sum));

        char *const loaded_layers = new char[layer_sum * index->layer_size_];
        char *loaded_layers_p = loaded_layers;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n; ++vertex_idx) {
            LayerSize layer_n = index->GetLayerN(vertex_idx);
            if (layer_n) {
                fs->Read(*file_handler, loaded_layers_p, layer_n * index->layer_size_);
                *index->GetLayersMut(vertex_idx) = loaded_layers_p;
                loaded_layers_p += layer_n * index->layer_size_;
            }
        }
        index->SetLoadLayer(cur_vertex_n, loaded_layers);
        return index;
    }

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------

    // check invariant of graph

    void CheckGraph() const {
        assert(cur_vertex_n_ <= max_vertex_);
        int max_layer = -1;
        for (VertexType vertex_idx = 0; vertex_idx < cur_vertex_n_; ++vertex_idx) {
            int cur_max_layer = GetLayerN(vertex_idx);
            max_layer = Max(cur_max_layer, max_layer);
            assert(cur_max_layer >= 0 && cur_max_layer <= max_layer_);
            for (int layer_idx = 0; layer_idx <= cur_max_layer; ++layer_idx) {
                auto [neighbors, neighbor_n] = GetNeighbors(vertex_idx, layer_idx);
                VertexType Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
                assert(neighbor_n <= Mmax);
                for (int i = 0; i < neighbor_n; ++i) {
                    VertexType neighbor_idx = neighbors[i];
                    assert(neighbor_idx < cur_vertex_n_ && neighbor_idx >= 0);
                    assert(neighbor_idx != vertex_idx);
                    auto [n_neighbors, n_neighbor_n] = GetNeighbors(neighbor_idx, layer_idx);
                    assert(n_neighbor_n <= Mmax);
                }
            }
        }
        assert(max_layer == max_layer_);
    }

    // void DumpGraph(const String filename) {
    //     std::fstream fo(filename, std::ios::out);
    //     fo << std::endl << "---------------------------------------------" << std::endl;
    //     fo << "M_: " << M_ << std::endl;
    //     fo << "Mmax_: " << Mmax_ << std::endl;
    //     fo << "Mmax0_: " << Mmax0_ << std::endl;
    //     fo << "ef_: " << ef_ << std::endl;
    //     fo << "ef_construction_: " << ef_construction_ << std::endl;
    //     fo << "dim_: " << dim_ << std::endl;
    //     fo << std::endl;

    //     fo << "current element number: " << cur_vertex_n_ << std::endl;
    //     fo << "max layer: " << max_layer_ << std::endl;
    //     fo << std::endl;

    //     Vector<Vector<VertexType>> layer2vertex(max_layer_ + 1);
    //     for (VertexType v = 0; v < cur_vertex_n_; ++v) {
    //         i32 layer_n = GetLayerN(v);
    //         for (i32 layer_idx = 0; layer_idx <= layer_n; ++layer_idx) {
    //             layer2vertex[layer_idx].emplace_back(v);
    //         }
    //     }
    //     for (i32 layer = 0; layer <= max_layer_; ++layer) {
    //         fo << "layer " << layer << std::endl;
    //         for (VertexType v : layer2vertex[layer]) {
    //             fo << v << ": ";
    //             auto [neighbors, neighbor_n] = GetNeighbors(v, layer);
    //             for (int i = 0; i < neighbor_n; ++i) {
    //                 fo << neighbors[i] << ", ";
    //             }
    //             fo << std::endl;
    //         }
    //     }
    //     fo << "---------------------------------------------" << std::endl;
    // }
};
} // namespace infinity