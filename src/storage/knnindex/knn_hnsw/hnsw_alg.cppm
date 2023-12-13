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
#include <iostream>
#include <random>

import stl;
import file_system;
import file_system_type;
import infinity_exception;
import bitmask;

import hnsw_common;
import plain_store;
import graph_store;
import lvq_store;

export module hnsw_alg;

// Fixme: some variable has implicit type conversion.
// Fixme: some variable has confusing name.
// Fixme: has no test for different `DataType`.

// Todo: make more embedding type.
// Todo: make module partition.
// Todo: make the constructor of `KnnHnsw` with less confusing template

namespace infinity {

export template <typename DataType, typename LabelType, typename DataStore, typename Distance>
    requires DataStoreConcept<DataStore, DataType> && DistanceConcept<Distance, DataType> && std::same_as<typename Distance::DataStore, DataStore>
class KnnHnsw {
public:
    using This = KnnHnsw<DataType, LabelType, DataStore, Distance>;
    using StoreType = typename DataStore::StoreType;

    using PDV = Pair<DataType, VertexType>;
    using CMP = CompareByFirst<DataType, VertexType>;
    using DistHeap = Heap<PDV, CMP>;

    constexpr static int prefetch_offset_ = 0;
    constexpr static int prefetch_step_ = 2;

private:
    const SizeT M_;
    const SizeT Mmax_;
    const SizeT Mmax0_;
    const SizeT ef_construction_;
    SizeT ef_;

    // 1 / log(1.0 * M_)
    const double mult_;
    std::default_random_engine level_rng_{};

    DataStore data_store_;
    GraphStore graph_store_;
    Distance distance_;
    const UniquePtr<LabelType[]> labels_;

private:
    KnnHnsw(SizeT M,
            SizeT Mmax,
            SizeT Mmax0,
            SizeT ef_construction,
            DataStore data_store,
            GraphStore graph_store,
            Distance distance,
            UniquePtr<LabelType[]> labels,
            SizeT ef,
            SizeT random_seed)
        : M_(M), Mmax_(Mmax), Mmax0_(Mmax0), ef_construction_(Max(M_, ef_construction)), //
          mult_(1 / std::log(1.0 * M_)),                                                 //
          data_store_(Move(data_store)),                                                 //
          graph_store_(Move(graph_store)),                                               //
          distance_(Move(distance)),                                                     //
          labels_(Move(labels)) {
        if (ef == 0) {
            ef = ef_construction_;
        }
        ef_ = ef;
        level_rng_.seed(random_seed);
    }

    static Pair<SizeT, SizeT> GetMmax(SizeT M) { return {M, 2 * M}; }

public:
    static UniquePtr<This> Make(SizeT max_vertex, SizeT dim, SizeT M, SizeT ef_construction, DataStore::InitArgs args) {
        auto [Mmax, Mmax0] = This::GetMmax(M);
        auto data_store = DataStore::Make(max_vertex, dim, Move(args));
        auto graph_store = GraphStore::Make(max_vertex, Mmax, Mmax0);
        Distance distance(data_store.dim());
        return UniquePtr<This>(new This(M,
                                        Mmax,
                                        Mmax0,
                                        ef_construction,
                                        Move(data_store),
                                        Move(graph_store),
                                        Move(distance),
                                        MakeUnique<LabelType[]>(max_vertex),
                                        0,
                                        0));
    }

    ~KnnHnsw() = default;

private:
    // >= 0
    i32 GenerateRandomLayer() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        auto r1 = distribution(level_rng_);
        double r = -std::log(r1) * mult_;
        return static_cast<i32>(r);
    }

    template <typename Iterator>
        requires DataIteratorConcept<Iterator, const DataType *>
    VertexType StoreData(Iterator iter, const LabelType *labels, SizeT insert_n) {
        auto ret = data_store_.AddVec(iter, insert_n);
        if (ret == DataStore::ERR_IDX) {
            Error<StorageException>("Data index is not enough.");
        }
        std::copy(labels, labels + insert_n, labels_.get() + ret);
        return ret;
    }

public:
    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    // return value is a max heap of distance
    DistHeap SearchLayer(VertexType enter_point, const StoreType &query, i32 layer_idx, SizeT candidate_n) const {
        DistHeap result;
        DistHeap candidate;

        data_store_.Prefetch(enter_point);
        DataType dist = distance_(query, data_store_.GetVec(enter_point), data_store_);

        candidate.emplace(-dist, enter_point);
        result.emplace(dist, enter_point);

        Vector<bool> visited(data_store_.cur_vec_num(), false);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (-minus_c_dist > result.top().first && result.size() == candidate_n) {
                break;
            }
            const auto [neighbors_p, neighbor_size] = graph_store_.GetNeighbors(c_idx, layer_idx);
            int prefetch_start = neighbor_size - 1 - prefetch_offset_;
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                if (visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                if (prefetch_start >= 0) {
                    int lower = Max(0, prefetch_start - prefetch_step_);
                    for (int i = prefetch_start; i >= lower; --i) {
                        data_store_.Prefetch(neighbors_p[i]);
                    }
                    prefetch_start -= prefetch_step_;
                }
                dist = distance_(query, data_store_.GetVec(n_idx), data_store_);
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

    DistHeap SearchLayer(VertexType enter_point, const StoreType &query, i32 layer_idx, SizeT candidate_n, const Bitmask &bitmask) const {
        if (bitmask.IsAllTrue()) {
            return SearchLayer(enter_point, query, layer_idx, candidate_n);
        }
        DistHeap result;
        DistHeap candidate;

        DataType dist{};
        if (bitmask.IsTrue(enter_point)) {
            data_store_.Prefetch(enter_point);
            dist = distance_(query, data_store_.GetVec(enter_point), data_store_);

            candidate.emplace(-dist, enter_point);
            result.emplace(dist, enter_point);
        } else {
            candidate.emplace(LimitMax<DataType>(), enter_point);
        }

        Vector<bool> visited(data_store_.cur_vec_num(), false);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (result.size() == candidate_n && -minus_c_dist > result.top().first) {
                break;
            }
            const auto [neighbors_p, neighbor_size] = graph_store_.GetNeighbors(c_idx, layer_idx);
            int prefetch_start = neighbor_size - 1 - prefetch_offset_;
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                if (visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                if (prefetch_start >= 0) {
                    int lower = Max(0, prefetch_start - prefetch_step_);
                    for (int i = prefetch_start; i >= lower; --i) {
                        data_store_.Prefetch(neighbors_p[i]);
                    }
                    prefetch_start -= prefetch_step_;
                }
                dist = distance_(query, data_store_.GetVec(n_idx), data_store_);
                if (result.size() < candidate_n || dist < result.top().first) {
                    candidate.emplace(-dist, n_idx);
                    if (bitmask.IsTrue(n_idx)) {
                        result.emplace(dist, n_idx);
                        if (result.size() > candidate_n) {
                            result.pop();
                        }
                    }
                }
            }
        }
        return result;
    }

    VertexType SearchLayerNearest(VertexType enter_point, const StoreType &query, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        DataType cur_dist = distance_(query, data_store_.GetVec(cur_p), data_store_);
        bool check = true;
        while (check) {
            check = false;
            const auto [neighbors_p, neighbor_size] = graph_store_.GetNeighbors(cur_p, layer_idx);
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                DataType n_dist = distance_(query, data_store_.GetVec(n_idx), data_store_);
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
    void SelectNeighborsHeuristic(DistHeap &candidates, SizeT M, VertexType *result_p, VertexListSize *result_size_p) const {
        VertexListSize result_size = 0;
        if (SizeT c_size = candidates.size(); c_size < M) {
            while (!candidates.empty()) {
                result_p[result_size++] = candidates.top().second;
                candidates.pop();
            }
        } else {
            while (!candidates.empty() && result_size < M) {
                const auto &[minus_c_dist, c_idx] = candidates.top();
                StoreType c_data = data_store_.GetVec(c_idx);
                bool check = true;
                for (SizeT i = 0; i < result_size; ++i) {
                    VertexType r_idx = result_p[i];
                    DataType cr_dist = distance_(c_data, data_store_.GetVec(r_idx), data_store_);
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

    void ConnectNeighbors(VertexType vertex_i, const VertexType *q_neighbors_p, VertexListSize q_neighbor_size, i32 layer_idx) {
        for (int i = 0; i < q_neighbor_size; ++i) {
            VertexType n_idx = q_neighbors_p[i];
            auto [n_neighbors_p, n_neighbor_size_p] = graph_store_.GetNeighborsMut(n_idx, layer_idx);
            VertexListSize n_neighbor_size = *n_neighbor_size_p;
            SizeT Mmax = layer_idx == 0 ? Mmax0_ : Mmax_;
            if (n_neighbor_size < Mmax) {
                *(n_neighbors_p + n_neighbor_size) = vertex_i;
                *n_neighbor_size_p = n_neighbor_size + 1;
                continue;
            }
            StoreType n_data = data_store_.GetVec(n_idx);
            DataType n_dist = distance_(n_data, data_store_.GetVec(vertex_i), data_store_);

            Vector<PDV> tmp;
            tmp.reserve(n_neighbor_size + 1);
            tmp.emplace_back(-n_dist, vertex_i);
            for (int i = 0; i < n_neighbor_size; ++i) {
                tmp.emplace_back(-distance_(n_data, data_store_.GetVec(n_neighbors_p[i]), data_store_), n_neighbors_p[i]);
            }

            DistHeap candidates(tmp.begin(), tmp.end());
            SelectNeighborsHeuristic(candidates, Mmax, n_neighbors_p, n_neighbor_size_p); // write in memory
        }
    }

    template <typename Iterator>
        requires DataIteratorConcept<Iterator, const DataType *>
    void InsertVecs(Iterator query_iter, const LabelType *labels, SizeT insert_n) {
        const VertexType vertex_i1 = StoreData(Move(query_iter), labels, insert_n);
        for (SizeT i = 0; i < insert_n; ++i) {
            StoreType query = data_store_.GetVec(vertex_i1 + i);
            i32 q_layer = GenerateRandomLayer();
            i32 max_layer = graph_store_.max_layer();
            VertexType ep = graph_store_.enterpoint();
            VertexType vertex_i = vertex_i1 + i;
            graph_store_.AddVertex(vertex_i, q_layer);

            for (i32 cur_layer = max_layer; cur_layer > q_layer; --cur_layer) {
                ep = SearchLayerNearest(ep, query, cur_layer);
            }
            for (i32 cur_layer = Min(q_layer, max_layer); cur_layer >= 0; --cur_layer) {
                DistHeap search_result = SearchLayer(ep, query, cur_layer, ef_construction_); // TODO:: use pool
                DistHeap candidates;
                while (!search_result.empty()) {
                    const auto &[dist, idx] = search_result.top();
                    candidates.emplace(-dist, idx);
                    search_result.pop();
                }
                const auto [q_neighbors_p, q_neighbor_size_p] = graph_store_.GetNeighborsMut(vertex_i, cur_layer);
                SelectNeighborsHeuristic(candidates, M_, q_neighbors_p, q_neighbor_size_p);
                ep = q_neighbors_p[0];
                ConnectNeighbors(vertex_i, q_neighbors_p, *q_neighbor_size_p, cur_layer);
            }
            if (i && i % 10000 == 0) {
                std::cout << "Inserted " << i << " / " << insert_n << std::endl;
            }
        }
    }

    // this two interface is for test and benchmark
    void Insert(const DataType *queries, const LabelType *labels, SizeT insert_n) {
        InsertVecs(DenseVectorIter(queries, data_store_.dim(), insert_n), labels, insert_n);
    }
    void Insert(const DataType *query, LabelType label) { Insert(query, &label, 1); }

    MaxHeap<Pair<DataType, LabelType>> KnnSearch(const DataType *q, SizeT k, const Bitmask *bitmask = nullptr) const {
        auto query = data_store_.MakeQuery(q);
        VertexType ep = graph_store_.enterpoint();
        for (i32 cur_layer = graph_store_.max_layer(); cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest(ep, query, cur_layer);
        }
        DistHeap search_result = SearchLayer(ep, query, 0, Max(k, ef_));
        while (search_result.size() > k) {
            search_result.pop();
        }
        MaxHeap<Pair<DataType, LabelType>> result; // TODO:: reserve
        while (!search_result.empty()) {
            const auto &[dist, idx] = search_result.top();
            result.emplace(dist, labels_[idx]);
            search_result.pop();
        }
        return result;
    }

    MaxHeap<Pair<DataType, LabelType>> KnnSearch(const DataType *q, SizeT k, const Bitmask &bitmask) const {
        auto query = data_store_.MakeQuery(q);
        VertexType ep = graph_store_.enterpoint();
        for (i32 cur_layer = graph_store_.max_layer(); cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest(ep, query, cur_layer);
        }
        DistHeap search_result = SearchLayer(ep, query, 0, Max(k, ef_), bitmask);
        while (search_result.size() > k) {
            search_result.pop();
        }
        MaxHeap<Pair<DataType, LabelType>> result; // TODO:: reserve
        while (!search_result.empty()) {
            const auto &[dist, idx] = search_result.top();
            result.emplace(dist, labels_[idx]);
            search_result.pop();
        }
        return result;
    }

public:
    void SetEf(SizeT ef) { ef_ = ef; }

    void Save(FileHandler &file_handler) {
        file_handler.Write(&M_, sizeof(M_));
        file_handler.Write(&ef_construction_, sizeof(ef_construction_));
        data_store_.Save(file_handler);
        graph_store_.SaveGraph(file_handler, data_store_.cur_vec_num());
        file_handler.Write(labels_.get(), sizeof(LabelType) * data_store_.cur_vec_num());
    }

    static UniquePtr<This> Load(FileHandler &file_handler, DataStore::InitArgs args) {
        SizeT M;
        file_handler.Read(&M, sizeof(M));
        SizeT ef_construction;
        file_handler.Read(&ef_construction, sizeof(ef_construction));
        auto [Mmax, Mmax0] = This::GetMmax(M);

        auto data_store = DataStore::Load(file_handler, 0, args);
        auto graph_store = GraphStore::LoadGraph(file_handler, data_store.max_vec_num(), Mmax, Mmax0, data_store.cur_vec_num());
        Distance distance(data_store.dim());

        auto labels = MakeUnique<LabelType[]>(data_store.cur_vec_num());
        file_handler.Read(labels.get(), sizeof(LabelType) * data_store.cur_vec_num());
        return UniquePtr<This>(new This(M, Mmax, Mmax0, ef_construction, Move(data_store), Move(graph_store), Move(distance), Move(labels), 0, 0));
    }

    //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------
    void Check() const { graph_store_.CheckGraph(data_store_.cur_vec_num(), Mmax0_, Mmax_); }

    void Dump(std::ostream &os) {
        os << std::endl << "---------------------------------------------" << std::endl;
        os << "max_vec_n: " << data_store_.max_vec_num() << std::endl;
        graph_store_.Dump(os, data_store_.cur_vec_num());
        os << "---------------------------------------------" << std::endl;
    }
};

} // namespace infinity