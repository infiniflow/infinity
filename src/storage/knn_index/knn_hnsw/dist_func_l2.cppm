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

export module dist_func_l2;

namespace infinity {

export template <typename DataType>
class PlainL2Dist {
public:
    using DataStore = PlainStore<DataType>;
    using StoreType = typename DataStore::StoreType;

private:
    using SIMDFuncType = DataType (*)(const DataType *, const DataType *, SizeT);

    SIMDFuncType SIMDFunc = nullptr;

public:
    PlainL2Dist(SizeT dim) {
        if constexpr (std::is_same<DataType, float>()) {
#if defined(USE_AVX512)
            if (dim % 16 == 0) {
                SIMDFunc = F32L2AVX512;
            } else {
                SIMDFunc = F32L2AVX512Residual;
            }
#elif defined(USE_AVX)
            if (dim % 16 == 0) {
                SIMDFunc = F32L2AVX;
            } else {
                SIMDFunc = F32L2AVXResidual;
            }
#else
            SIMDFunc = F32L2BF;
#endif
        }
    }

    DataType operator()(const StoreType &v1, const StoreType &v2, const DataStore &data_store) const { return SIMDFunc(v1, v2, data_store.dim()); }
};

export template <typename DataType, typename CompressType>
class LVQL2Cache {
public:
    // for l2 distance, const1 = scale * norm1(compress), const2 = scale * scale * norm2(compress)
    using LocalCacheType = Pair<DataType, DataType>;
    using GlobalCacheType = Tuple<>;

    static LocalCacheType MakeLocalCache(const CompressType *c, DataType scale, SizeT dim, const MeanType *) {
        i64 norm1 = 0;
        i64 norm2 = 0;
        for (SizeT i = 0; i < dim; ++i) {
            norm1 += c[i];
            norm2 += c[i] * c[i];
        }
        return {norm1 * scale, norm2 * scale * scale};
    }

    static GlobalCacheType MakeGlobalCache(const MeanType *, SizeT ) { return {}; }
};

export template <typename DataType, typename CompressType>
class LVQL2Dist {
public:
    using This = LVQL2Dist<DataType, CompressType>;
    using DataStore = LVQStore<DataType, CompressType, LVQL2Cache<DataType, CompressType>>;
    using StoreType = typename DataStore::StoreType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, SizeT);

    SIMDFuncType SIMDFunc = nullptr;

public:
    LVQL2Dist(SizeT dim) {
        if constexpr (std::is_same<CompressType, i8>()) {
#if defined(USE_AVX512)
            if (dim % 64 == 0) {
                SIMDFunc = I8IPAVX512;
            } else {
                SIMDFunc = I8IPAVX512Residual;
            }
#elif defined(USE_AVX)
            if (dim % 32 == 0) {
                SIMDFunc = I8IPAVX;
            } else {
                SIMDFunc = I8IPAVXResidual;
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
        auto beta = bias1 - bias2;
        auto [norm1_scale_1, norm2sq_scalesq_1] = v1.GetLocalCache();
        auto [norm1_scale_2, norm2sq_scalesq_2] = v2.GetLocalCache();
        return norm2sq_scalesq_1 + norm2sq_scalesq_2 + beta * beta * dim - 2 * scale1 * scale2 * c1c2_ip + 2 * beta * norm1_scale_1 -
               2 * beta * norm1_scale_2;
    }
};

} // namespace infinity