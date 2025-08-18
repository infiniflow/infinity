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

export module infinity_core:dist_func_cos;

import :hnsw_common;
import :plain_vec_store;
import :lvq_vec_store;
import :rabitq_vec_store;
import :simd_functions;

import std;
import third_party;

namespace infinity {

export template <typename DataType, typename CompressType>
class LVQCosDist;

export template <typename DataType>
class PlainCosDist {
public:
    using This = PlainCosDist<DataType>;
    using VecStoreMeta = PlainVecStoreMeta<DataType>;
    using StoreType = typename VecStoreMeta::StoreType;
    using DistanceType = typename VecStoreMeta::DistanceType;
    using LVQDist = LVQCosDist<DataType, i8>;

private:
    using SIMDFuncType = f32 (*)(const DataType *, const DataType *, size_t);

    SIMDFuncType SIMDFunc = nullptr;

public:
    PlainCosDist() : SIMDFunc(nullptr) {}
    PlainCosDist(PlainCosDist &&other) : SIMDFunc(std::exchange(other.SIMDFunc, nullptr)) {}
    PlainCosDist &operator=(PlainCosDist &&other) {
        if (this != &other) {
            SIMDFunc = std::exchange(other.SIMDFunc, nullptr);
        }
        return *this;
    }
    ~PlainCosDist() = default;

    PlainCosDist(size_t dim) {
        if constexpr (std::is_same<DataType, float>()) {
            if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_F32Cos_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_F32Cos_ptr_;
            }
        } else if constexpr (std::is_same<DataType, u8>()) {
            SIMDFunc = GetSIMD_FUNCTIONS().HNSW_U8Cos_ptr_;
        } else if constexpr (std::is_same<DataType, i8>()) {
            SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8Cos_ptr_;
        }
    }

    template <typename DataStore>
    DistanceType operator()(VertexType v1_i, VertexType v2_i, const DataStore &data_store) const {
        return Inner(data_store.GetVec(v1_i), data_store.GetVec(v2_i), data_store.dim());
    }

    template <typename DataStore>
    DistanceType operator()(const StoreType &v1, VertexType v2_i, const DataStore &data_store, VertexType v1_i = kInvalidVertex) const {
        return Inner(v1, data_store.GetVec(v2_i), data_store.dim());
    }

    LVQDist ToLVQDistance(size_t dim) &&;

private:
    DistanceType Inner(const StoreType &v1, const StoreType &v2, size_t dim) const { return -SIMDFunc(v1, v2, dim); }
};

export template <typename DataType, typename CompressType>
class LVQCosCache {
public:
    using LocalCacheType = std::pair<DataType, DataType>;
    using GlobalCacheType = std::pair<DataType, DataType>;

    static LocalCacheType MakeLocalCache(const CompressType *c, DataType scale, size_t dim, const MeanType *mean) {
        i64 norm1 = 0;
        MeanType mean_c = 0;
        for (size_t i = 0; i < dim; ++i) {
            norm1 += c[i];
            mean_c += mean[i] * c[i];
        }
        return {norm1 * scale, mean_c * scale};
    }

    static GlobalCacheType MakeGlobalCache(const MeanType *mean, size_t dim) {
        MeanType norm1 = 0;
        MeanType norm2 = 0;
        for (size_t i = 0; i < dim; ++i) {
            norm1 += mean[i];
            norm2 += mean[i] * mean[i];
        }
        return {norm1, norm2};
    }

    static void DumpLocalCache(std::ostream &os, const LocalCacheType &local_cache) {
        os << "norm1: " << local_cache.first << ", mean_c: " << local_cache.second << std::endl;
    }

    static void DumpGlobalCache(std::ostream &os, const GlobalCacheType &global_cache) {
        os << "norm1_mean: " << global_cache.first << ", norm2_mean: " << global_cache.second << std::endl;
    }
};

export template <typename DataType, typename CompressType>
class LVQCosDist {
public:
    using This = LVQCosDist<DataType, CompressType>;
    using LVQDist = This;
    using VecStoreMetaType = LVQVecStoreMetaType<DataType, CompressType, LVQCosCache<DataType, CompressType>>;
    using StoreType = typename VecStoreMetaType::StoreType;
    using DistanceType = typename VecStoreMetaType::DistanceType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, size_t);

    SIMDFuncType SIMDFunc = nullptr;

public:
    LVQCosDist() : SIMDFunc(nullptr) {}
    LVQCosDist(LVQCosDist &&other) : SIMDFunc(std::exchange(other.SIMDFunc, nullptr)) {}
    LVQCosDist &operator=(LVQCosDist &&other) {
        if (this != &other) {
            SIMDFunc = std::exchange(other.SIMDFunc, nullptr);
        }
        return *this;
    }
    ~LVQCosDist() = default;
    LVQCosDist(size_t dim) {
        if constexpr (std::is_same<CompressType, i8>()) {
            if (dim % 64 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8IP_64_ptr_;
            } else if (dim % 32 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8IP_32_ptr_;
            } else if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8IP_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8IP_ptr_;
            }
        }
    }

    template <typename DataStore>
    DistanceType operator()(VertexType v1_i, VertexType v2_i, const DataStore &data_store) const {
        return Inner(data_store.GetVec(v1_i), data_store.GetVec(v2_i), data_store.vec_store_meta());
    }

    template <typename DataStore>
    DistanceType operator()(const StoreType &v1, VertexType v2_i, const DataStore &data_store, VertexType v1_i = kInvalidVertex) const {
        return Inner(v1, data_store.GetVec(v2_i), data_store.vec_store_meta());
    }

private:
    template <typename VecStoreMeta>
    DistanceType Inner(const StoreType &v1, const StoreType &v2, VecStoreMeta &vec_store_meta) const {
        size_t dim = vec_store_meta.dim();
        i32 c1c2_ip = SIMDFunc(v1->compress_vec_, v2->compress_vec_, dim);
        auto scale1 = v1->scale_;
        auto scale2 = v2->scale_;
        auto bias1 = v1->bias_;
        auto bias2 = v2->bias_;
        auto [norm1_scale_1, mean_c_scale_1] = v1->local_cache_;
        auto [norm1_scale_2, mean_c_scale_2] = v2->local_cache_;
        auto [norm1_mean, norm2_mean] = vec_store_meta.global_cache();
        auto dist = scale1 * scale2 * c1c2_ip + bias2 * norm1_scale_1 + bias1 * norm1_scale_2 + dim * bias1 * bias2 + norm2_mean +
                    (bias1 + bias2) * norm1_mean + mean_c_scale_1 + mean_c_scale_2;

        return -dist;
    }
};

export template <typename DataType>
class RabitqCosDist {
public:
    using This = RabitqCosDist<DataType>;
    using RabitqDist = This;
    using MetaType = RabitqVecStoreMetaType<DataType>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using DistanceType = typename MetaType::DistanceType;
    using CompressType = typename MetaType::CompressType;
    using AlignType = typename MetaType::AlignType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, size_t);

    SIMDFuncType SIMDFunc = nullptr;

public:
    RabitqCosDist() : SIMDFunc(nullptr) {}
    RabitqCosDist(RabitqCosDist &&other) : SIMDFunc(std::exchange(other.SIMDFunc, nullptr)) {}
    RabitqCosDist &operator=(RabitqCosDist &&other) {
        if (this != &other) {
            SIMDFunc = std::exchange(other.SIMDFunc, nullptr);
        }
        return *this;
    }
    ~RabitqCosDist() = default;
    RabitqCosDist(size_t dim) {
        if constexpr (std::is_same<CompressType, u8>() && std::is_same<AlignType, u8>()) {
            if (dim % 64 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().Rabitq_U8IP_64_ptr_;
            } else if (dim % 32 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().Rabitq_U8IP_32_ptr_;
            } else if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().Rabitq_U8IP_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().Rabitq_U8IP_ptr_;
            }
        }
    }

    template <typename DataStore>
    DistanceType operator()(const QueryType &v1, VertexType v2_i, const DataStore &data_store, VertexType v1_i = kInvalidVertex) const {
        const StoreType &v2 = data_store.GetVec(v2_i);
        size_t align_dim = data_store.align_dim();
        return Inner(v1, v2, align_dim);
    }

private:
    DistanceType Inner(const QueryType &query, const StoreType &base, size_t dim) const {
        // estimate <x, q>
        // DistanceType ip_estimate = MetaType::IpDistanceBetweenQueryAndBinaryCode(query->query_compress_vec_, base->compress_vec_, dim);
        DistanceType ip_estimate = SIMDFunc(query->query_compress_vec_, base->compress_vec_, dim);
        DistanceType ip_recover =
            MetaType::RecoverIpDistance(ip_estimate, dim, base->sum_, query->query_sum_, query->query_lower_bound_, query->query_delta_);

        // estimate ||o_r, q_r||^2
        DistanceType res = MetaType::RecoverL2DistanceSqr(ip_recover / base->error_, base->norm_, query->query_norm_);

        // Recover cos distance
        if (MetaType::IsApproxZero(base->raw_norm_) || MetaType::IsApproxZero(query->query_raw_norm_)) {
            res = 1;
        } else {
            res = 1 - (query->query_raw_norm_ * query->query_raw_norm_ + base->raw_norm_ * base->raw_norm_ - res) * 0.5f /
                          (query->query_raw_norm_ * base->raw_norm_);
        }
        return res;
    }
};

template <typename DataType>
LVQCosDist<DataType, i8> PlainCosDist<DataType>::ToLVQDistance(size_t dim) && {
    return LVQCosDist<DataType, i8>(dim);
}

} // namespace infinity