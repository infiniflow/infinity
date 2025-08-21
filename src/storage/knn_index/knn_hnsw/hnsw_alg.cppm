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

export module infinity_core:hnsw_alg;

import :local_file_handle;
import :infinity_exception;
import :knn_result_handler;
import :multivector_result_handler;
import :hnsw_common;
import :data_store;
import :dist_func_lsg_wrapper;
import :plain_vec_store;
import :default_values;
import :utility;
import :hnsw_lsg_builder;
import :index_hnsw;

import std;
import third_party;

import logical_type;
import serialize;
import column_def;

// Fixme: some variable has implicit type conversion.
// Fixme: some variable has confusing name.
// Fixme: has no test for different `DataType`.

// Todo: make more embedding type.
// Todo: make module partition.

namespace infinity {

export struct KnnSearchOption {
    size_t ef_ = 0;
    LogicalType column_logical_type_ = LogicalType::kEmbedding;
};

export template <typename VecStoreType, typename LabelType, bool OwnMem>
class KnnHnswBase {
public:
    using This = KnnHnswBase<VecStoreType, LabelType, OwnMem>;
    using DataType = typename VecStoreType::DataType;
    using QueryVecType = typename VecStoreType::QueryVecType;
    using StoreType = typename VecStoreType::StoreType;
    using QueryType = typename VecStoreType::QueryType;
    using DataStore = DataStore<VecStoreType, LabelType, OwnMem>;
    using Distance = typename VecStoreType::Distance;
    using DistanceType = typename Distance::DistanceType;

    using PDV = std::pair<DistanceType, VertexType>;
    using CMP = CompareByFirst<DistanceType, VertexType>;
    using CMPReverse = CompareByFirstReverse<DistanceType, VertexType>;
    using DistHeap = std::priority_queue<PDV, std::vector<PDV>, CMP>;

    constexpr static bool LSG = IsLSGDistance<Distance>;

    static std::pair<size_t, size_t> GetMmax(size_t M) { return {2 * M, M}; }

public:
    KnnHnswBase() : M_(0), ef_construction_(0), mult_(0), prefetch_step_(DEFAULT_PREFETCH_SIZE) {}
    KnnHnswBase(This &&other)
        : M_(std::exchange(other.M_, 0)), ef_construction_(std::exchange(other.ef_construction_, 0)), mult_(std::exchange(other.mult_, 0.0)),
          data_store_(std::move(other.data_store_)), distance_(std::move(other.distance_)),
          prefetch_step_(L1_CACHE_SIZE / data_store_.vec_store_meta().GetVecSizeInBytes()) {}
    This &operator=(This &&other) {
        if (this != &other) {
            M_ = std::exchange(other.M_, 0);
            ef_construction_ = std::exchange(other.ef_construction_, 0);
            mult_ = std::exchange(other.mult_, 0.0);
            data_store_ = std::move(other.data_store_);
            distance_ = std::move(other.distance_);
            prefetch_step_ = L1_CACHE_SIZE / data_store_.vec_store_meta().GetVecSizeInBytes();
        }
        return *this;
    }

    size_t GetSizeInBytes() const { return sizeof(M_) + sizeof(ef_construction_) + data_store_.GetSizeInBytes(); }

    LabelType GetLabel(VertexType vertex_i) const { return data_store_.GetLabel(vertex_i); }

    void Save(LocalFileHandle &file_handle) const {
        file_handle.Append(&M_, sizeof(M_));
        file_handle.Append(&ef_construction_, sizeof(ef_construction_));
        data_store_.Save(file_handle);
    }

    void SaveToPtr(LocalFileHandle &file_handle) const {
        file_handle.Append(&M_, sizeof(M_));
        file_handle.Append(&ef_construction_, sizeof(ef_construction_));
        data_store_.SaveToPtr(file_handle);
    }

protected:
    // >= 0
    i32 GenerateRandomLayer() {
        static thread_local std::mt19937 generator;
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        auto r1 = distribution(generator);
        double r = -std::log(r1) * mult_;
        return static_cast<i32>(r);
    }

    template <LogicalType ColumnLogicalType>
    using SearchLayerReturnParam3T = std::conditional_t<ColumnLogicalType == LogicalType::kEmbedding, VertexType, LabelType>;

    // return the nearest `ef_construction_` neighbors of `query` in layer `layer_idx`
    template <bool WithLock,
              FilterConcept<LabelType> Filter = std::nullopt_t,
              LogicalType ColumnLogicalType = LogicalType::kEmbedding,
              typename MultiVectorInnerTopnIndexType = void>
    std::tuple<size_t, std::unique_ptr<DistanceType[]>, std::unique_ptr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>
    SearchLayer(VertexType enter_point, const StoreType &query, VertexType query_i, i32 layer_idx, size_t result_n, const Filter &filter) const {
        static_assert(ColumnLogicalType == LogicalType::kEmbedding || ColumnLogicalType == LogicalType::kMultiVector);
        auto d_ptr = std::make_unique_for_overwrite<DistanceType[]>(result_n);
        auto i_ptr = std::make_unique_for_overwrite<SearchLayerReturnParam3T<ColumnLogicalType>[]>(result_n);
        using ResultHandler = std::conditional_t<ColumnLogicalType == LogicalType::kEmbedding,
                                                 HeapResultHandler<CompareMax<DistanceType, VertexType>>,
                                                 MultiVectorResultHandler<DistanceType, LabelType, MultiVectorInnerTopnIndexType>>;
        ResultHandler result_handler(1, result_n, d_ptr.get(), i_ptr.get());
        result_handler.Begin();
        auto add_result = [&](DistanceType add_dist, VertexType add_v) {
            if constexpr (ColumnLogicalType == LogicalType::kEmbedding) {
                if constexpr (!std::is_same_v<Filter, std::nullopt_t>) {
                    if (filter(this->GetLabel(add_v))) {
                        result_handler.AddResult(0, add_dist, add_v);
                    }
                } else {
                    result_handler.AddResult(0, add_dist, add_v);
                }
            } else if constexpr (ColumnLogicalType == LogicalType::kMultiVector) {
                const auto l = this->GetLabel(add_v);
                if constexpr (!std::is_same_v<Filter, std::nullopt_t>) {
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
            auto dist = distance_(query, enter_point, data_store_, query_i);
            candidate.emplace(-dist, enter_point);
            add_result(dist, enter_point);
        }

        size_t cur_vec_num = data_store_.cur_vec_num();
        std::vector<bool> visited(cur_vec_num, false);
        visited[enter_point] = true;

        while (!candidate.empty()) {
            const auto [minus_c_dist, c_idx] = candidate.top();
            candidate.pop();
            if (result_handler.GetSize(0) == result_n && -minus_c_dist > result_handler.GetDistance0(0)) {
                break;
            }

            std::shared_lock<std::shared_mutex> lock;
            if constexpr (WithLock && OwnMem) {
                lock = data_store_.SharedLock(c_idx);
            }

            const auto [neighbors_p, neighbor_size] = data_store_.GetNeighbors(c_idx, layer_idx);
            i32 prefetch_start = 0;
            for (i32 i = 0; i < neighbor_size; ++i) {
                for (size_t j = prefetch_step_; prefetch_start < neighbor_size && j > 0; --j) {
                    data_store_.PrefetchVec(neighbors_p[prefetch_start++]);
                }
                VertexType n_idx = neighbors_p[i];
                if (n_idx >= (VertexType)cur_vec_num || visited[n_idx]) {
                    continue;
                }
                visited[n_idx] = true;
                auto dist = distance_(query, n_idx, data_store_, query_i);
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
    VertexType SearchLayerNearest(VertexType enter_point, const StoreType &query, VertexType query_i, i32 layer_idx) const {
        VertexType cur_p = enter_point;
        auto cur_dist = distance_(query, cur_p, data_store_, query_i);
        bool check = true;
        while (check) {
            check = false;

            std::shared_lock<std::shared_mutex> lock;
            if constexpr (WithLock && OwnMem) {
                lock = data_store_.SharedLock(cur_p);
            }

            const auto [neighbors_p, neighbor_size] = data_store_.GetNeighbors(cur_p, layer_idx);
            for (int i = neighbor_size - 1; i >= 0; --i) {
                VertexType n_idx = neighbors_p[i];
                auto n_dist = distance_(query, n_idx, data_store_, query_i);
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
    void SelectNeighborsHeuristic(std::vector<PDV> candidates, size_t M, VertexType *result_p, VertexListSize *result_size_p) const {
        VertexListSize result_size = 0;
        if (candidates.size() < M) {
            std::sort(candidates.begin(), candidates.end(), CMPReverse());
            for (const auto &[_, idx] : candidates) {
                result_p[result_size++] = idx;
            }
        } else {
            std::make_heap(candidates.begin(), candidates.end(), CMPReverse());
            while (!candidates.empty() && size_t(result_size) < M) {
                std::pop_heap(candidates.begin(), candidates.end(), CMPReverse());
                const auto &[c_dist, c_idx] = candidates.back();
                StoreType c_data = data_store_.GetVec(c_idx);
                bool check = true;
                for (size_t i = 0; i < size_t(result_size); ++i) {
                    VertexType r_idx = result_p[i];
                    auto cr_dist = distance_(c_data, r_idx, data_store_, c_idx);
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
            size_t Mmax = layer_idx == 0 ? data_store_.Mmax0() : data_store_.Mmax();
            if (n_neighbor_size < VertexListSize(Mmax)) {
                *(n_neighbors_p + n_neighbor_size) = vertex_i;
                *n_neighbor_size_p = n_neighbor_size + 1;
                continue;
            }
            StoreType n_data = data_store_.GetVec(n_idx);
            auto n_dist = distance_(n_data, vertex_i, data_store_, n_idx);

            std::vector<PDV> candidates;
            candidates.reserve(n_neighbor_size + 1);
            candidates.emplace_back(n_dist, vertex_i);
            for (int j = 0; j < n_neighbor_size; ++j) {
                candidates.emplace_back(distance_(n_data, n_neighbors_p[j], data_store_, n_idx), n_neighbors_p[j]);
            }

            SelectNeighborsHeuristic(std::move(candidates), Mmax, n_neighbors_p, n_neighbor_size_p); // write in memory
        }
    }

    template <bool WithLock, FilterConcept<LabelType> Filter, LogicalType ColumnLogicalType>
    auto SearchLayerHelper(VertexType enter_point, const StoreType &query, i32 layer_idx, size_t result_n, const Filter &filter) const {
        if constexpr (ColumnLogicalType == LogicalType::kEmbedding) {
            return SearchLayer<WithLock, Filter, ColumnLogicalType>(enter_point, query, kInvalidVertex, layer_idx, result_n, filter);
        } else if constexpr (ColumnLogicalType == LogicalType::kMultiVector) {
            if (result_n <= std::numeric_limits<u8>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u8>(enter_point, query, kInvalidVertex, layer_idx, result_n, filter);
            }
            if (result_n <= std::numeric_limits<u16>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u16>(enter_point, query, kInvalidVertex, layer_idx, result_n, filter);
            }
            if (result_n <= std::numeric_limits<u32>::max()) {
                return SearchLayer<WithLock, Filter, ColumnLogicalType, u32>(enter_point, query, kInvalidVertex, layer_idx, result_n, filter);
            }
            UnrecoverableError(fmt::format("Unsupported result_n : {}, which is larger than u32::max()", result_n));
            return std::tuple<size_t, std::unique_ptr<DistanceType[]>, std::unique_ptr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>{};
        } else {
            static_assert(false, "Unsupported column logical type");
        }
    }

    template <bool WithLock, FilterConcept<LabelType> Filter = std::nullopt_t, LogicalType ColumnLogicalType = LogicalType::kEmbedding>
    std::tuple<size_t, std::unique_ptr<DistanceType[]>, std::unique_ptr<SearchLayerReturnParam3T<ColumnLogicalType>[]>>
    KnnSearchInner(const QueryVecType &q, size_t k, const Filter &filter, const KnnSearchOption &option) const {
        size_t ef = option.ef_;
        if (ef == 0) {
            ef = k;
        }
        QueryType query = data_store_.MakeQuery(q);
        auto [max_layer, ep] = data_store_.GetEnterPoint();
        if (ep == -1) {
            return {0, nullptr, nullptr};
        }
        for (i32 cur_layer = max_layer; cur_layer > 0; --cur_layer) {
            ep = SearchLayerNearest<WithLock>(ep, query, kInvalidVertex, cur_layer);
        }
        return SearchLayerHelper<WithLock, Filter, ColumnLogicalType>(ep, query, 0, ef, filter);
    }

public:
    void InitLSGBuilder(const IndexHnsw *index_hnsw, std::shared_ptr<ColumnDef> column_def) {
        if (!LSG) {
            UnrecoverableError("InsertSampleVecs when LSG not use!");
        }
        lsg_builder_ = HnswLSGBuilder<DataType, DistanceType>(index_hnsw, column_def);
    }

    template <typename Iter>
    size_t InsertSampleVecs(Iter iter, size_t sample_num = std::numeric_limits<size_t>::max()) {
        if (!LSG) {
            UnrecoverableError("InsertSampleVecs when LSG not use!");
        }
        if (!lsg_builder_.has_value()) {
            UnrecoverableError("lsg_builder_ not exist, maybe not Init!");
        }
        return lsg_builder_->InsertSampleVec(std::move(iter), sample_num);
    }

    template <typename Iter>
    void InsertLSAvg(Iter iter, size_t row_count) {
        if (!LSG) {
            UnrecoverableError("InsertLSAvg when LSG not use!");
        }
        if (!lsg_builder_.has_value()) {
            UnrecoverableError("lsg_builder_ not exist, maybe not Init!");
        }
        lsg_builder_->InsertLSAvg(std::move(iter), row_count);
    }

    void SetLSGParam() {
        if (!LSG) {
            UnrecoverableError("InsertSampleVecs when LSG not use!");
        }
        if (!lsg_builder_.has_value()) {
            UnrecoverableError("lsg_builder_ not exist, maybe not Init!");
        }
        distance_.SetLSGParam(lsg_builder_->alpha(), lsg_builder_->avg());
    }

public:
    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    std::pair<size_t, size_t> InsertVecs(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        auto [start_i, end_i] = StoreData(std::move(iter), config);
        for (VertexType vertex_i = start_i; vertex_i < end_i; ++vertex_i) {
            Build(vertex_i);
        }
        return {start_i, end_i};
    }

    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    std::pair<VertexType, VertexType> StoreData(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
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
            ep = SearchLayerNearest<true>(ep, query, vertex_i, cur_layer);
        }
        for (i32 cur_layer = std::min(q_layer, max_layer); cur_layer >= 0; --cur_layer) {
            auto [result_n, d_ptr, v_ptr] = SearchLayer<true>(ep, query, vertex_i, cur_layer, ef_construction_, std::nullopt);
            auto search_result = std::vector<PDV>(result_n);
            for (size_t i = 0; i < result_n; ++i) {
                search_result[i] = {d_ptr[i], v_ptr[i]};
            }

            const auto [q_neighbors_p, q_neighbor_size_p] = data_store_.GetNeighborsMut(vertex_i, cur_layer);
            SelectNeighborsHeuristic(std::move(search_result), M_, q_neighbors_p, q_neighbor_size_p);
            ep = q_neighbors_p[0];
            ConnectNeighbors(vertex_i, q_neighbors_p, *q_neighbor_size_p, cur_layer);
        }
    }

    template <FilterConcept<LabelType> Filter = std::nullopt_t, bool WithLock = true>
    std::tuple<size_t, std::unique_ptr<DistanceType[]>, std::unique_ptr<LabelType[]>>
    KnnSearch(const QueryVecType &q, size_t k, const Filter &filter, const KnnSearchOption &option = {}) const {
        switch (option.column_logical_type_) {
            case LogicalType::kEmbedding: {
                auto [result_n, d_ptr, v_ptr] = KnnSearchInner<WithLock, Filter>(q, k, filter, option);
                auto labels = std::make_unique_for_overwrite<LabelType[]>(result_n);
                for (size_t i = 0; i < result_n; ++i) {
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
    std::tuple<size_t, std::unique_ptr<DistanceType[]>, std::unique_ptr<LabelType[]>>
    KnnSearch(const QueryVecType &q, size_t k, const KnnSearchOption &option = {}) const {
        return KnnSearch<std::nullopt_t, WithLock>(q, k, std::nullopt, option);
    }

    // function for test, add sort for convenience
    template <FilterConcept<LabelType> Filter = std::nullopt_t, bool WithLock = true>
    std::vector<std::pair<DistanceType, LabelType>>
    KnnSearchSorted(const QueryVecType &q, size_t k, const Filter &filter, const KnnSearchOption &option = {}) const {
        auto [result_n, d_ptr, v_ptr] = KnnSearchInner<WithLock, Filter>(q, k, filter, option);
        std::vector<std::pair<DistanceType, LabelType>> result(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            result[i] = {d_ptr[i], GetLabel(v_ptr[i])};
        }
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        return result;
    }

    // function for test
    std::vector<std::pair<DistanceType, LabelType>> KnnSearchSorted(const QueryVecType &q, size_t k, const KnnSearchOption &option = {}) const {
        return KnnSearchSorted<std::nullopt_t>(q, k, std::nullopt, option);
    }

    size_t GetVecNum() const { return data_store_.cur_vec_num(); }

    size_t mem_usage() const { return data_store_.mem_usage(); }

    Distance &distance() { return distance_; }

protected:
    size_t M_;
    size_t ef_construction_;

    // 1 / log(1.0 * M_)
    double mult_;

    DataStore data_store_;
    Distance distance_;

    size_t prefetch_step_;

    std::optional<HnswLSGBuilder<DataType, DistanceType>> lsg_builder_{};

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

export template <typename VecStoreType, typename LabelType, bool OwnMem = true>
class KnnHnsw : public KnnHnswBase<VecStoreType, LabelType, OwnMem> {
public:
    using This = KnnHnsw<VecStoreType, LabelType, OwnMem>;
    using DataStore = DataStore<VecStoreType, LabelType, OwnMem>;
    using Distance = typename VecStoreType::Distance;
    using CompressVecStoreType = decltype(VecStoreType::template ToLVQ<i8>());
    constexpr static bool kOwnMem = OwnMem;

    KnnHnsw(size_t M, size_t ef_construction, DataStore data_store, Distance distance) {
        this->M_ = M;
        this->ef_construction_ = std::max(M, ef_construction);
        this->mult_ = 1 / std::log(1.0 * M);
        this->data_store_ = std::move(data_store);
        this->distance_ = std::move(distance);
    }

public:
    static std::unique_ptr<This> Make(size_t chunk_size, size_t max_chunk_n, size_t dim, size_t M, size_t ef_construction) {
        auto [Mmax0, Mmax] = This::GetMmax(M);
        auto data_store = DataStore::Make(chunk_size, max_chunk_n, dim, Mmax0, Mmax);
        Distance distance(data_store.dim());
        return std::make_unique<This>(M, ef_construction, std::move(data_store), std::move(distance));
    }

    static std::unique_ptr<This> Load(LocalFileHandle &file_handle) {
        size_t M;
        file_handle.Read(&M, sizeof(M));
        size_t ef_construction;
        file_handle.Read(&ef_construction, sizeof(ef_construction));

        auto data_store = DataStore::Load(file_handle);
        Distance distance(data_store.dim());

        return std::make_unique<This>(M, ef_construction, std::move(data_store), std::move(distance));
    }

    static std::unique_ptr<This> LoadFromPtr(LocalFileHandle &file_handle, size_t size) {
        auto buffer = std::make_unique<char[]>(size);
        file_handle.Read(buffer.get(), size);
        const char *ptr = buffer.get();
        size_t M = ReadBufAdv<size_t>(ptr);
        size_t ef_construction = ReadBufAdv<size_t>(ptr);
        auto data_store = DataStore::LoadFromPtr(ptr);
        Distance distance(data_store.dim());
        if (size_t diff = ptr - buffer.get(); diff != size) {
            UnrecoverableError("LoadFromPtr failed");
        }
        return std::make_unique<This>(M, ef_construction, std::move(data_store), std::move(distance));
    }

    std::unique_ptr<KnnHnsw<CompressVecStoreType, LabelType>> CompressToLVQ() && {
        if constexpr (std::is_same_v<VecStoreType, CompressVecStoreType>) {
            return std::make_unique<This>(std::move(*this));
        } else {
            using CompressedDistance = typename CompressVecStoreType::Distance;
            CompressedDistance distance = std::move(this->distance_).ToLVQDistance(this->data_store_.dim());
            auto compressed_datastore = std::move(this->data_store_).template CompressToLVQ<CompressVecStoreType>();
            return std::make_unique<KnnHnsw<CompressVecStoreType, LabelType>>(this->M_,
                                                                              this->ef_construction_,
                                                                              std::move(compressed_datastore),
                                                                              std::move(distance));
        }
    }
};

export template <typename VecStoreType, typename LabelType>
class KnnHnsw<VecStoreType, LabelType, false> : public KnnHnswBase<VecStoreType, LabelType, false> {
public:
    using This = KnnHnsw<VecStoreType, LabelType, false>;
    using DataStore = DataStore<VecStoreType, LabelType, false>;
    using Distance = typename VecStoreType::Distance;
    constexpr static bool kOwnMem = false;

    KnnHnsw(size_t M, size_t ef_construction, DataStore data_store, Distance distance) {
        this->M_ = M;
        this->ef_construction_ = std::max(M, ef_construction);
        this->mult_ = 1 / std::log(1.0 * M);
        this->data_store_ = std::move(data_store);
        this->distance_ = std::move(distance);
    }
    KnnHnsw(This &&other) : KnnHnswBase<VecStoreType, LabelType, false>(std::move(other)) {}
    KnnHnsw &operator=(This &&other) {
        if (this != &other) {
            KnnHnswBase<VecStoreType, LabelType, false>::operator=(std::move(other));
        }
        return *this;
    }

public:
    static std::unique_ptr<This> LoadFromPtr(const char *&ptr, size_t size) {
        const char *ptr_end = ptr + size;
        size_t M = ReadBufAdv<size_t>(ptr);
        size_t ef_construction = ReadBufAdv<size_t>(ptr);
        auto data_store = DataStore::LoadFromPtr(ptr);
        Distance distance(data_store.dim());
        if (size_t diff = ptr_end - ptr; diff != 0) {
            UnrecoverableError("LoadFromPtr failed");
        }
        return std::make_unique<This>(M, ef_construction, std::move(data_store), std::move(distance));
    }
};

} // namespace infinity