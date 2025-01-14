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

import stl;
import hnsw_common;
import plain_vec_store;
import lvq_vec_store;
import simd_functions;

export module dist_func_l2;

namespace infinity {

export template <typename DataType, typename CompressType>
class LVQL2Dist;

export template <typename DataType>
class PlainLSL2Dist;

export template <typename DataType>
class PlainL2Dist {
public:
    using VecStoreMeta = PlainVecStoreMeta<DataType>;
    using StoreType = typename VecStoreMeta::StoreType;
    using DistanceType = typename VecStoreMeta::DistanceType;

private:
    using SIMDFuncType = std::conditional_t<std::is_same_v<DataType, float>, f32, i32> (*)(const DataType *, const DataType *, SizeT);

    SIMDFuncType SIMDFunc = nullptr;

public:
    PlainL2Dist() : SIMDFunc(nullptr) {}
    PlainL2Dist(PlainL2Dist &&other) : SIMDFunc(std::exchange(other.SIMDFunc, nullptr)) {}
    PlainL2Dist &operator=(PlainL2Dist &&other) {
        if (this != &other) {
            SIMDFunc = std::exchange(other.SIMDFunc, nullptr);
        }
        return *this;
    }
    ~PlainL2Dist() = default;

    PlainL2Dist(SizeT dim) {
        if constexpr (std::is_same<DataType, float>()) {
            if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_F32L2_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_F32L2_ptr_;
            }
        } else if constexpr (std::is_same<DataType, i8>()) {
            if (dim % 64 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8L2_64_ptr_;
            } else if (dim % 32 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8L2_32_ptr_;
            } else if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8L2_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_I8L2_ptr_;
            }
        } else if constexpr (std::is_same<DataType, u8>()) {
            if (dim % 64 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_U8L2_64_ptr_;
            } else if (dim % 32 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_U8L2_32_ptr_;
            } else if (dim % 16 == 0) {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_U8L2_16_ptr_;
            } else {
                SIMDFunc = GetSIMD_FUNCTIONS().HNSW_U8L2_ptr_;
            }
        }
    }

    DistanceType operator()(const StoreType &v1, const StoreType &v2, const VecStoreMeta &vec_store_meta) const {
        return SIMDFunc(v1, v2, vec_store_meta.dim());
    }

    LVQL2Dist<DataType, i8> ToLVQDistance(SizeT dim) &&;
};

export template <typename DataType>
class PlainLSL2Dist : public PlainL2Dist<DataType> {
public:
    using Base = PlainL2Dist<DataType>;
    using VecStoreMeta = PlainVecStoreMeta<DataType>;
    using StoreType = typename VecStoreMeta::StoreType;
    using DistanceType = typename VecStoreMeta::DistanceType;

    PlainLSL2Dist() = default;
    PlainLSL2Dist(PlainLSL2Dist &&other) : Base(std::move(other)), gt_(std::exchange(other.gt_, nullptr)), alpha_(other.alpha_) {}
    PlainLSL2Dist &operator=(PlainLSL2Dist &&other) {
        if (this != &other) {
            Base::operator=(std::move(other));
            gt_ = std::exchange(other.gt_, nullptr);
            alpha_ = other.alpha_;
        }
        return *this;
    }
    ~PlainLSL2Dist() = default;

    PlainLSL2Dist(SizeT dim, UniquePtr<DataType[]> gt, float alpha) : Base(dim), gt_(std::move(gt)), alpha_(alpha) {}

    DistanceType operator()(const StoreType &v1, const StoreType &v2, VertexType v1_i, VertexType v2_i, const VecStoreMeta &vec_store_meta) const {
        DistanceType dist = Base::operator()(v1, v2, vec_store_meta);
        if (v1_i == v2_i) {
            return dist;
        }
        if (v1_i < 0 || v2_i < 0) {
            return dist;
        }
        auto k = std::pow(std::sqrt(gt_[v1_i]) * std::sqrt(gt_[v2_i]), alpha_);
        return dist / k;
    }

private:
    UniquePtr<DataType[]> gt_;
    float alpha_ = 1.0;
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

    static GlobalCacheType MakeGlobalCache(const MeanType *, SizeT) { return {}; }

public:
    static void DumpLocalCache(std::ostream &os, const LocalCacheType &local_cache) {
        os << "norm1_scale: " << local_cache.first << ", norm2sq_scalesq: " << local_cache.second << std::endl;
    }

    static void DumpGlobalCache(std::ostream &, const GlobalCacheType &) {}
};

export template <typename DataType, typename CompressType>
class LVQL2Dist {
public:
    using This = LVQL2Dist<DataType, CompressType>;
    using VecStoreMetaType = LVQVecStoreMetaType<DataType, CompressType, LVQL2Cache<DataType, CompressType>>;
    using StoreType = typename VecStoreMetaType::StoreType;
    using DistanceType = typename VecStoreMetaType::DistanceType;

private:
    using SIMDFuncType = i32 (*)(const CompressType *, const CompressType *, SizeT);

    SIMDFuncType SIMDFunc = nullptr;

public:
    LVQL2Dist() : SIMDFunc(nullptr) {}
    LVQL2Dist(LVQL2Dist &&other) : SIMDFunc(std::exchange(other.SIMDFunc, nullptr)) {}
    LVQL2Dist &operator=(LVQL2Dist &&other) {
        if (this != &other) {
            SIMDFunc = std::exchange(other.SIMDFunc, nullptr);
        }
        return *this;
    }
    ~LVQL2Dist() = default;
    LVQL2Dist(SizeT dim) {
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

    template <typename VecStoreMeta>
    DataType operator()(const StoreType &v1, const StoreType &v2, const VecStoreMeta &vec_store_meta) const {
        SizeT dim = vec_store_meta.dim();
        i32 c1c2_ip = SIMDFunc(v1->compress_vec_, v2->compress_vec_, dim);
        auto scale1 = v1->scale_;
        auto scale2 = v2->scale_;
        auto beta = v1->bias_ - v2->bias_;

        auto [norm1_scale_1, norm2sq_scalesq_1] = v1->local_cache_;
        auto [norm1_scale_2, norm2sq_scalesq_2] = v2->local_cache_;
        return norm2sq_scalesq_1 + norm2sq_scalesq_2 + beta * beta * dim - 2 * scale1 * scale2 * c1c2_ip + 2 * beta * norm1_scale_1 -
               2 * beta * norm1_scale_2;
    }
};

template <typename DataType>
LVQL2Dist<DataType, i8> PlainL2Dist<DataType>::ToLVQDistance(SizeT dim) && {
    return LVQL2Dist<DataType, i8>(dim);
}

} // namespace infinity