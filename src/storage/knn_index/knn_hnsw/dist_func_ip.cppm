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

#include "header.h"
#include <type_traits>

import stl;
import hnsw_common;
import plain_store;
import lvq_store;
import hnsw_simd_func;

export module dist_func_ip;

namespace infinity {

export template <typename DataType, typename LabelType>
class PlainIPDist {
public:
    using DataStore = PlainStore<DataType, LabelType>;
    using StoreType = typename DataStore::StoreType;

private:
    using SIMDFuncType = DataType (*)(const DataType *, const DataType *, SizeT);

    SIMDFuncType SIMDFunc;

public:
    PlainIPDist(SizeT dim) {
        if constexpr (std::is_same<DataType, float>()) {
#if defined(USE_AVX512)
            if (dim % 16 == 0) {
                SIMDFunc = F32IPAVX512;
            } else {
                SIMDFunc = F32IPBF;
            }
#elif defined(USE_AVX)
            if (dim % 16 == 0) {
                SIMDFunc = F32IPAVX;
            } else {
                SIMDFunc = F32IPBF;
            }
#else
            SIMDFunc = F32IPBF;
#endif
        }
    }

    DataType operator()(const StoreType &v1, const StoreType &v2, const DataStore &data_store) const { return -SIMDFunc(v1, v2, data_store.dim()); }
};

export template <typename DataType, typename CompressType>
class LVQIPCache {
public:
    // for ip distance, const1 = scale * norm1(compress), const2 = scale * <mean, compress>
    using LocalCacheType = Pair<DataType, DataType>;
    // for ip distance, const1 = norm1(mean), const2 = norm2(mean)
    using GlobalCacheType = Pair<double, double>;

    static LocalCacheType MakeLocalCache(const CompressType *c, DataType scale, SizeT dim, const MeanType *mean) {
        i64 norm1 = 0;
        MeanType mean_c = 0;
        for (SizeT i = 0; i < dim; i++) {
            norm1 += c[i];
            mean_c += mean[i] * c[i];
        }
        return {norm1 * scale, mean_c * scale};
    }

    static GlobalCacheType MakeGlobalCache(const MeanType *mean, SizeT dim) {
        MeanType norm1 = 0;
        MeanType norm2 = 0;
        for (SizeT i = 0; i < dim; i++) {
            norm1 += mean[i];
            norm2 += mean[i] * mean[i];
        }
        return {norm1, norm2};
    }
};

export template <typename DataType, typename LabelType, typename CompressType>
class LVQIPDist {
public:
    using This = LVQIPDist<DataType, LabelType, CompressType>;
    using DataStore = LVQStore<DataType, LabelType, CompressType, LVQIPCache<DataType, CompressType>>;
    using StoreType = typename DataStore::StoreType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, SizeT);

    SIMDFuncType SIMDFunc;

public:
    LVQIPDist(SizeT dim) {
        if constexpr (std::is_same<CompressType, i8>()) {
#if defined(USE_AVX512)
            if (dim % 16 == 0) {
                SIMDFunc = I8IPAVX512;
            } else {
                SIMDFunc = I8IPBF;
            }
#elif defined(USE_AVX)
            if (dim % 16 == 0) {
                SIMDFunc = I8IPAVX;
            } else {
                SIMDFunc = I8IPBF;
            }
#else
            SIMDFunc = I8IPBF;
#endif
        }
    }

    DataType operator()(const StoreType &v1, const StoreType &v2, const DataStore &data_store) const {
        SizeT dim = data_store.dim();
        i32 c1c2_ip = SIMDFunc(v1.GetCompressVec(), v2.GetCompressVec(), dim);
        auto [scale1, bias1] = v1.GetScalar();
        auto [scale2, bias2] = v2.GetScalar();
        auto [norm1_scale_1, mean_c_scale_1] = v1.GetLocalCache();
        auto [norm1_scale_2, mean_c_scale_2] = v2.GetLocalCache();
        auto [norm1_mean, norm2_mean] = data_store.GetGlobalCache();
        auto dist = scale1 * scale2 * c1c2_ip + bias2 * norm1_scale_1 + bias1 * norm1_scale_2 + dim * bias1 * bias2 + norm2_mean +
                    (bias1 + bias2) * norm1_mean + mean_c_scale_1 + mean_c_scale_2;
        return -dist;
    }
};

} // namespace infinity