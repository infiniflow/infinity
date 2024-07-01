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

#include "../header.h"
#include <ostream>

import stl;
import logger;
import third_party;
import hnsw_common;
import hnsw_simd_func;
import plain_vec_store;
import lvq_vec_store;

export module dist_func_cos;

namespace infinity {

export template <typename DataType, typename CompressType>
class LVQCosDist;

export template <typename DataType>
class PlainCosDist {
public:
    using VecStoreMeta = PlainVecStoreMeta<DataType>;
    using StoreType = typename VecStoreMeta::StoreType;

private:
    using SIMDFuncType = DataType (*)(const DataType *, const DataType *, SizeT);

    SIMDFuncType SIMDFunc;

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
#if defined(USE_AVX512)
            if (dim % 16 == 0) {
                SIMDFunc = F32CosAVX512;
            } else {
                SIMDFunc = F32CosAVX512Residual;
            }
#elif defined(USE_AVX)
            if (dim % 16 == 0) {
                SIMDFunc = F32CosAVX;
            } else {
                SIMDFunc = F32CosAVXResidual;
            }
#elif defined(USE_SSE)
            if (dim % 16 == 0) {
                SIMDFunc = F32CosSSE;
            } else {
                SIMDFunc = F32CosSSEResidual;
            }
#else
            SIMDFunc = F32CosBF;
#endif
        }
    }

    DataType operator()(const StoreType &v1, const StoreType &v2, const VecStoreMeta &vec_store_meta) const {
        return -SIMDFunc(v1, v2, vec_store_meta.dim());
    }

    LVQCosDist<DataType, i8> ToLVQDistance(SizeT dim) &&;
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
    using VecStoreMeta = LVQVecStoreMeta<DataType, CompressType, LVQCosCache<DataType, CompressType>>;
    using StoreType = typename VecStoreMeta::StoreType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, SizeT);

    SIMDFuncType SIMDFunc;

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
#if defined(USE_AVX512)
            if (dim % 16 == 0) {
                SIMDFunc = I8IPAVX512;
            } else {
                SIMDFunc = I8IPAVX512Residual;
            }
#elif defined(USE_AVX)
            if (dim % 16 == 0) {
                SIMDFunc = I8IPAVX;
            } else {
                SIMDFunc = I8IPAVXResidual;
            }
#elif defined(USE_SSE)
            if (dim % 16 == 0) {
                SIMDFunc = I8IPSSE;
            } else {
                SIMDFunc = I8IPSSEResidual;
            }
#else
            SIMDFunc = I8IPBF;
#endif
        }
    }

    DataType operator()(const StoreType &v1, const StoreType &v2, const VecStoreMeta &vec_store_meta) const {
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

template<typename DataType>
LVQCosDist<DataType, i8> PlainCosDist<DataType>::ToLVQDistance(SizeT dim) && {
    return LVQCosDist<DataType, i8>(dim);
}

} // namespace infinity