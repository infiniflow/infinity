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

#include <ostream>
#include <random>

export module hnsw_alg;

import stl;
import local_file_handle;
import infinity_exception;
import knn_result_handler;
import multivector_result_handler;
import logical_type;
import hnsw_common;
import data_store;
import third_party;

// Fixme: some variable has implicit type conversion.
// Fixme: some variable has confusing name.
// Fixme: has no test for different `DataType`.

// Todo: make more embedding type.
// Todo: make module partition.

namespace infinity {

export struct KnnSearchOption {
    SizeT ef_ = 0;
    LogicalType column_logical_type_ = LogicalType::kEmbedding;
};

export template <typename VecStoreType, typename LabelType>
class KnnHnsw {
public:
    using This = KnnHnsw<VecStoreType, LabelType>;
    using DataType = typename VecStoreType::DataType;
    using QueryVecType = typename VecStoreType::QueryVecType;
    using StoreType = typename VecStoreType::StoreType;
    using QueryType = typename VecStoreType::QueryType;
    using DataStore = DataStore<VecStoreType, LabelType>;
    using Distance = typename VecStoreType::Distance;
    using DistanceType = typename Distance::DistanceType;

    using PDV = Pair<DistanceType, VertexType>;
    using CMP = CompareByFirst<DistanceType, VertexType>;
    using CMPReverse = CompareByFirstReverse<DistanceType, VertexType>;
    using DistHeap = Heap<PDV, CMP>;

    constexpr static int prefetch_offset_ = 0;
    constexpr static int prefetch_step_ = 2;

    using CompressVecStoreType = decltype(VecStoreType::template ToLVQ<i8>());

    // private:
    KnnHnsw(SizeT M, SizeT ef_construction, DataStore data_store, Distance distance, SizeT random_seed)
        : M_(M), ef_construction_(std::max(M_, ef_construction)), mult_(1 / std::log(1.0 * M_)), data_store_(std::move(data_store)),
          distance_(std::move(distance)) {
        level_rng_.seed(random_seed);
    }

    static Pair<SizeT, SizeT> GetMmax(SizeT M) { return {2 * M, M}; }

public:
    KnnHnsw() : M_(0), ef_construction_(0), mult_(0) {}
    KnnHnsw(This &&other)
        : M_(std::exchange(other.M_, 0)), ef_construction_(std::exchange(other.ef_construction_, 0)), mult_(std::exchange(other.mult_, 0.0)),
          level_rng_(std::move(other.level_rng_)), data_store_(std::move(other.data_store_)), distance_(std::move(other.distance_)) {}
    This &operator=(This &&other) {
        if (this != &other) {
            M_ = std::exchange(other.M_, 0);
            ef_construction_ = std::exchange(other.ef_construction_, 0);
            mult_ = std::exchange(other.mult_, 0.0);
            level_rng_ = std::move(other.level_rng_);
            data_store_ = std::move(other.data_store_);
            distance_ = std::move(other.distance_);
        }
        return *this;
    }
    ~KnnHnsw() = default;

    static UniquePtr<This> Make(SizeT chunk_size, SizeT max_chunk_n, SizeT dim, SizeT M, SizeT ef_construction) {
        auto [Mmax0, Mmax] = This::GetMmax(M);
        auto data_store = DataStore::Make(chunk_size, max_chunk_n, dim, Mmax0, Mmax);
        Distance distance(data_store.dim());
        return MakeUnique<This>(M, ef_construction, std::move(data_store), std::move(distance), 0);
    }

    SizeT GetSizeInBytes() const { return sizeof(M_) + sizeof(ef_construction_) + data_store_.GetSizeInBytes(); }

    void Save(LocalFileHandle &file_handle) {
        file_handle.Append(&M_, sizeof(M_));
        file_handle.Append(&ef_construction_, sizeof(ef_construction_));
        data_store_.Save(file_handle);
    }

    static UniquePtr<This> Load(LocalFileHandle &file_handle) {
        SizeT M;
        file_handle.Read(&M, sizeof(M));
        SizeT ef_construction;
        file_handle.Read(&ef_construction, sizeof(ef_construction));

        auto data_store = DataStore::Load(file_handle);
        Distance distance(data_store.dim());

        return MakeUnique<This>(M, ef_construction, std::move(data_store), std::move(distance), 0);
    }

private:
    // >= 0
    i32 GenerateRandomLayer() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        auto r1 = distribution(level_rng_);
        double r = -std::log(r1) * mult_;
        return static_cast<i32>(r);
    }

    template <LogicalType ColumnLogicalType>
    using SearchLayerReturnParam3T = std::conditional_t<ColumnLogicalType == LogicalType::kEmbedding, VertexType, LabelType>;

    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    template <bool WithLock,
              FilterConcept<LabelType> Filter = NoneType,
              LogicalType ColumnLogicalType = LogicalType::kEmbedding,
              typename MultiVectorInnerTopnIndexType = void>
    Tuple<SizeT, UniquePtr<DistanceType[]>, UniquePtr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>
    SearchLayer(VertexType enter_point, const StoreType &query, i32 layer_idx, SizeT result_n, const Filter &filter) const {
        static_assert(ColumnLogicalType == LogicalType::kEmbedding || ColumnLogicalType == LogicalType::kMultiVector);
        auto d_ptr = MakeUniqueForOverwrite<DistanceType[]>(result_n);
        auto i_ptr = MakeUniqueForOverwrite<SearchLayerReturnParam3T<ColumnLogicalType>[]>(result_n);
        using ResultHandler = std::conditional_t<ColumnLogicalType == LogicalType::kEmbedding,
                                                 HeapResultHandler<CompareMax<DistanceType, VertexType>>,
                                                 MultiVectorResultHandler<DistanceType, LabelType, MultiVectorInnerTopnIndexType>>;
        ResultHandler result_handler(1, result_n, d_ptr.get(), i_ptr.get());
        result_handler.Begin();
        auto add_result = [&](DistanceType add_dist, VertexType add_v) {
            if constexpr (ColumnLogicalType == LogicalType::kEmbedding) {
                if constexpr (!std::is_same_v<Filter, NoneType>) {
                    if (filter(this->GetLabel(add_v))) {
                        result_handler.AddResult(0, add_dist, add_v);
                    }
                } else {
                    result_handler.AddResult(0, add_dist, add_v);
                }
            } else if constexpr (ColumnLogicalType == LogicalType::kMultiVector) {
                const auto l = this->GetLabel(add_v);
                if constexpr (!std::is_same_v<Filter, NoneType>) {
                    if (filter(l)) {
                        result_handler.AddResult(add_dist, l);
                    }
                } else {
                    result_handler.AddResult(add_dist, l);
                }
            } else {
                static_assert(false, "Unsupported column logical type");
            }
        };
        DistHeap candidate;

        data_store_.PrefetchVec(enter_point);
        // enter_point will not be added to result_handler, the distance is not used
        {
            auto dist = distance_(query, data_store_.GetVec(enter_point), data_store_.vec_store_meta());
            candidate.emplace(-dist, enter_point);
            add_result(dist, enter_point);
        }

        SizeT cur_vec_num = data_store_.cur_vec_num();
        Vector<bool> visited(cur_vec_num, false);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (result_handler.GetSize(0) == result_n && -minus_c_dist > result_handler.GetDistance0(0)) {
                break;
            }

            std::shared_lock<std::shared_mutex> lock;
            if constexpr (WithLock) {
                lock = data_store_.SharedLock(c_idx);
            }

            const auto [neighbors_p, neighbor_size] = data_store_.GetNeighbors(c_idx, layer_idx);
            int prefetch_start = neighbor_size - 1 - prefetch_offset_;
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                if (n_idx >= (VertexType)cur_vec_num || visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                if (prefetch_start >= 0) {
                    int lower = std::max(0, prefetch_start - prefetch_step_);
                    for (int j = prefetch_start; j >= lower; --j) {
                        VertexType prefetch_idx = neighbors_p[j];
                        data_store_.PrefetchVec(prefetch_idx);
                    }
                    prefetch_start -= prefetch_step_;
                }
                auto dist = distance_(query, data_store_.GetVec(n_idx), data_store_.vec_store_meta());
                if (result_handler.GetSize(0) < result_n || dist <= result_handler.GetDistance0(0)) {
                    candidate.emplace(-dist, n_idx);
                    add_result(dist, n_idx);
                }
            }
        }
        result_handler.EndWithoutSort();
        return {result_handler.GetSize(0), std::move(d_ptr), std::move(i_ptr)};
    }

    template <bool WithLock>
    VertexType SearchLayerNearest(VertexType enter_point, const StoreType &query, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        auto cur_dist = distance_(query, data_store_.GetVec(cur_p), data_store_.vec_store_meta());
        bool check = true;
        while (check) {
            check = false;

            std::shared_lock<std::shared_mutex> lock;
            if constexpr (WithLock) {
                lock = data_store_.SharedLock(cur_p);
            }

            const auto [neighbors_p, neighbor_size] = data_store_.GetNeighbors(cur_p, layer_idx);
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                auto n_dist = distance_(query, data_store_.GetVec(n_idx), data_store_.vec_store_meta());
                if (n_dist < cur_dist) {
                    cur_p = n_idx;
                    cur_dist = n_dist;
                    check = true;
                }
            }
        }
        return cur_p;
    }

    // the function does not need mutex because the lock of `result_p` is already acquired
    void SelectNeighborsHeuristic(Vector<PDV> candidates, SizeT M, VertexType *result_p, VertexListSize *result_size_p) const {
        VertexListSize result_size = 0;
        if (candidates.size() < M) {
            std::sort(candidates.begin(), candidates.end(), CMPReverse());
            for (const auto &[_, idx] : candidates) {
                result_p[result_size++] = idx;
            }
        } else {
            std::make_heap(candidates.begin(), candidates.end(), CMPReverse());
            while (!candidates.empty() && SizeT(result_size) < M) {
                std::pop_heap(candidates.begin(), candidates.end(), CMPReverse());
                const auto &[c_dist, c_idx] = candidates.back();
                StoreType c_data = data_store_.GetVec(c_idx);
                bool check = true;
                for (SizeT i = 0; i < SizeT(result_size); ++i) {
                    VertexType r_idx = result_p[i];
                    auto cr_dist = distance_(c_data, data_store_.GetVec(r_idx), data_store_.vec_store_meta());
                    if (cr_dist < c_dist) {
                        check = false;
                        break;
                    }
                }
                if (check) {
                    result_p[result_size++] = c_idx;
                }
                candidates.pop_back();
            }
            std::reverse(result_p, result_p + result_size);
        }
        *result_size_p = result_size;
    }

    void ConnectNeighbors(VertexType vertex_i, const VertexType *q_neighbors_p, VertexListSize q_neighbor_size, i32 layer_idx) {
        for (int i = 0; i < q_neighbor_size; ++i) {
            VertexType n_idx = q_neighbors_p[i];

            std::unique_lock<std::shared_mutex> lock = data_store_.UniqueLock(n_idx);

            auto [n_neighbors_p, n_neighbor_size_p] = data_store_.GetNeighborsMut(n_idx, layer_idx);
            VertexListSize n_neighbor_size = *n_neighbor_size_p;
            SizeT Mmax = layer_idx == 0 ? data_store_.Mmax0() : data_store_.Mmax();
            if (n_neighbor_size < VertexListSize(Mmax)) {
                *(n_neighbors_p + n_neighbor_size) = vertex_i;
                *n_neighbor_size_p = n_neighbor_size + 1;
                continue;
            }
            StoreType n_data = data_store_.GetVec(n_idx);
            auto n_dist = distance_(n_data, data_store_.GetVec(vertex_i), data_store_.vec_store_meta());

            Vector<PDV> candidates;
            candidates.reserve(n_neighbor_size + 1);
            candidates.emplace_back(n_dist, vertex_i);
            for (int j = 0; j < n_neighbor_size; ++j) {
                candidates.emplace_back(distance_(n_data, data_store_.GetVec(n_neighbors_p[j]), data_store_.vec_store_meta()), n_neighbors_p[j]);
            }

            SelectNeighborsHeuristic(std::move(candidates), Mmax, n_neighbors_p, n_neighbor_size_p); // write in memory
        }
    }

    LabelType GetLabel(VertexType vertex_i) const { return data_store_.GetLabel(vertex_i); }

    template <bool WithLock, FilterConcept<LabelType> Filter, LogicalType ColumnLogicalType>
    auto SearchLayerHelper(VertexType enter_point, const StoreType &query, i32 layer_idx, SizeT result_n, const Filter &filter) const {
        if constexpr (ColumnLogicalType == LogicalType::kEmbedding) {
            return SearchLayer<WithLock, Filter, ColumnLogicalType>(enter_point, query, layer_idx, result_n, filter);
        } else if constexpr (ColumnLogicalType == LogicalType::kMultiVector) {
            if (result_n <= std::numeric_limits<u8>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u8>(enter_point, query, layer_idx, result_n, filter);
            }
            if (result_n <= std::numeric_limits<u16>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u16>(enter_point, query, layer_idx, result_n, filter);
            }
            if (result_n <= std::numeric_limits<u32>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u32>(enter_point, query, layer_idx, result_n, filter);
            }
            UnrecoverableError(fmt::format("Unsupported result_n : {}, which is larger than u32::max()", result_n));
            return Tuple<SizeT, UniquePtr<DistanceType[]>, UniquePtr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>{};
        } else {
            static_assert(false, "Unsupported column logical type");
        }
    }

    template <bool WithLock, FilterConcept<LabelType> Filter = NoneType, LogicalType ColumnLogicalType = LogicalType::kEmbedding>
    Tuple<SizeT, UniquePtr<DistanceType[]>, UniquePtr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>
    KnnSearchInner(const QueryVecType &q, SizeT k, const Filter &filter, const KnnSearchOption &option) const {
        SizeT ef = option.ef_;
        if (ef == 0) {
            ef = k;
        }
        QueryType query = data_store_.MakeQuery(q);
        auto [max_layer, ep] = data_store_.GetEnterPoint();
        if (ep == -1) {
            return {0, nullptr, nullptr};
        }
        for (i32 cur_layer = max_layer; cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest<WithLock>(ep, query, cur_layer);
        }
        return SearchLayerHelper<WithLock, Filter, ColumnLogicalType>(ep, query, 0, ef, filter);
    }

public:
    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    Pair<SizeT, SizeT> InsertVecs(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        auto [start_i, end_i] = StoreData(std::move(iter), config);
        for (VertexType vertex_i = start_i; vertex_i < end_i; ++vertex_i) {
            Build(vertex_i);
        }
        return {start_i, end_i};
    }

    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    Pair<VertexType, VertexType> StoreData(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        if (config.optimize_) {
            return data_store_.OptAddVec(std::move(iter));
        }
        return data_store_.AddVec(std::move(iter));
    }

    void Optimize() { data_store_.Optimize(); }

    void Build(VertexType vertex_i) {
        std::unique_lock<std::shared_mutex> lock = data_store_.UniqueLock(vertex_i);

        i32 q_layer = GenerateRandomLayer();
        auto [max_layer, ep] = data_store_.TryUpdateEnterPoint(q_layer, vertex_i);

        StoreType query = data_store_.GetVec(vertex_i);

        data_store_.AddVertex(vertex_i, q_layer);

        for (i32 cur_layer = max_layer; cur_layer > q_layer; --cur_layer) {
            ep = SearchLayerNearest<true>(ep, query, cur_layer);
        }
        for (i32 cur_layer = std::min(q_layer, max_layer); cur_layer >= 0; --cur_layer) {
            auto [result_n, d_ptr, v_ptr] = SearchLayer<true>(ep, query, cur_layer, ef_construction_, None);
            auto search_result = Vector<PDV>(result_n);
            for (SizeT i = 0; i < result_n; ++i) {
                search_result[i] = {d_ptr[i], v_ptr[i]};
            }

            const auto [q_neighbors_p, q_neighbor_size_p] = data_store_.GetNeighborsMut(vertex_i, cur_layer);
            SelectNeighborsHeuristic(std::move(search_result), M_, q_neighbors_p, q_neighbor_size_p);
            ep = q_neighbors_p[0];
            ConnectNeighbors(vertex_i, q_neighbors_p, *q_neighbor_size_p, cur_layer);
        }
    }

    UniquePtr<KnnHnsw<CompressVecStoreType, LabelType>> CompressToLVQ() && {
        if constexpr (std::is_same_v<VecStoreType, CompressVecStoreType>) {
            return MakeUnique<This>(std::move(*this));
        } else {
            using CompressedDistance = typename CompressVecStoreType::Distance;
            CompressedDistance distance = std::move(distance_).ToLVQDistance(data_store_.dim());
            auto compressed_datastore = std::move(data_store_).template CompressToLVQ<CompressVecStoreType>();
            return MakeUnique<KnnHnsw<CompressVecStoreType, LabelType>>(M_,
                                                                        ef_construction_,
                                                                        std::move(compressed_datastore),
                                                                        std::move(distance),
                                                                        0);
        }
    }

    template <FilterConcept<LabelType> Filter = NoneType, bool WithLock = true>
    Tuple<SizeT, UniquePtr<DistanceType[]>, UniquePtr<LabelType[]>>
    KnnSearch(const QueryVecType &q, SizeT k, const Filter &filter, const KnnSearchOption &option = {}) const {
        switch (option.column_logical_type_) {
            case LogicalType::kEmbedding: {
                auto [result_n, d_ptr, v_ptr] = KnnSearchInner<WithLock, Filter>(q, k, filter, option);
                auto labels = MakeUniqueForOverwrite<LabelType[]>(result_n);
                for (SizeT i = 0; i < result_n; ++i) {
                    labels[i] = GetLabel(v_ptr[i]);
                }
                return {result_n, std::move(d_ptr), std::move(labels)};
            }
            case LogicalType::kMultiVector: {
                return KnnSearchInner<WithLock, Filter, LogicalType::kMultiVector>(q, k, filter, option);
            }
            default: {
                UnrecoverableError(fmt::format("Unsupported column logical type: {}", LogicalType2Str(option.column_logical_type_)));
            }
        }
        return {};
    }

    template <bool WithLock = true>
    Tuple<SizeT, UniquePtr<DistanceType[]>, UniquePtr<LabelType[]>>
    KnnSearch(const QueryVecType &q, SizeT k, const KnnSearchOption &option = {}) const {
        return KnnSearch<NoneType, WithLock>(q, k, None, option);
    }

    // function for test, add sort for convenience
    template <FilterConcept<LabelType> Filter = NoneType, bool WithLock = true>
    Vector<Pair<DistanceType, LabelType>>
    KnnSearchSorted(const QueryVecType &q, SizeT k, const Filter &filter, const KnnSearchOption &option = {}) const {
        auto [result_n, d_ptr, v_ptr] = KnnSearchInner<WithLock, Filter>(q, k, filter, option);
        Vector<Pair<DistanceType, LabelType>> result(result_n);
        for (SizeT i = 0; i < result_n; ++i) {
            result[i] = {d_ptr[i], GetLabel(v_ptr[i])};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    }

    // function for test
    Vector<Pair<DistanceType, LabelType>> KnnSearchSorted(const QueryVecType &q, SizeT k, const KnnSearchOption &option = {}) const {
        return KnnSearchSorted<NoneType>(q, k, None, option);
    }

    SizeT GetVecNum() const { return data_store_.cur_vec_num(); }

    SizeT mem_usage() const { return data_store_.mem_usage(); }

private:
    SizeT M_;
    SizeT ef_construction_;

    // 1 / log(1.0 * M_)
    double mult_;
    std::default_random_engine level_rng_{};

    DataStore data_store_;
    Distance distance_;

    // //---------------------------------------------- Following is the tmp debug function. ----------------------------------------------
public:
    void Check() const { data_store_.Check(); }

    void Dump(std::ostream &os) const {
        os << std::endl << "---------------------------------------------" << std::endl;
        os << "[CONST] M: " << M_ << ", ef_construction: " << ef_construction_ << ", mult: " << mult_ << std::endl;
        data_store_.Dump(os);
        os << "---------------------------------------------" << std::endl;
    }
};

} // namespace infinity