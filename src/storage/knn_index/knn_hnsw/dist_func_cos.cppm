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

export module infinity_core:dist_func_cos;

import :stl;
import :third_party;
import :hnsw_common;
import :plain_vec_store;
import :lvq_vec_store;
import :simd_functions;

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
    using SIMDFuncType = f32 (*)(const DataType *, const DataType *, SizeT);

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

    PlainCosDist(SizeT dim) {
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

    LVQDist ToLVQDistance(SizeT dim) &&;

private:
    DistanceType Inner(const StoreType &v1, const StoreType &v2, SizeT dim) const { return -SIMDFunc(v1, v2, dim); }
};

export template <typename DataType, typename CompressType>
class LVQCosCache {
public:
    using LocalCacheType = Pair<DataType, DataType>;
    using GlobalCacheType = Pair<DataType, DataType>;

    static LocalCacheType MakeLocalCache(const CompressType *c, DataType scale, SizeT dim, const MeanType *mean) {
        i64 norm1 = 0;
        MeanType mean_c = 0;
        for (SizeT i = 0; i < dim; ++i) {
            norm1 += c[i];
            mean_c += mean[i] * c[i];
        }
        return {norm1 * scale, mean_c * scale};
    }

    static GlobalCacheType MakeGlobalCache(const MeanType *mean, SizeT dim) {
        MeanType norm1 = 0;
        MeanType norm2 = 0;
        for (SizeT i = 0; i < dim; ++i) {
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
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, SizeT);

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
    LVQCosDist(SizeT dim) {
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
        SizeT dim = vec_store_meta.dim();
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

template <typename DataType>
LVQCosDist<DataType, i8> PlainCosDist<DataType>::ToLVQDistance(SizeT dim) && {
    return LVQCosDist<DataType, i8>(dim);
}

} // namespace infinity