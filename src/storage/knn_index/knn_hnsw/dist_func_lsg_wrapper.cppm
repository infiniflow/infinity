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

import stl;
import plain_vec_store;
import lvq_vec_store;
import dist_func_l2;
import dist_func_ip;
import dist_func_cos;
import hnsw_common;

export module dist_func_lsg_wrapper;

namespace infinity {

export template <typename Dist, typename VecStoreMeta>
class LSGDistWrapper {
public:
    using StoreType = typename VecStoreMeta::StoreType;
    using DistanceType = typename VecStoreMeta::DistanceType;
    using LSG = void;
    using LVQDist = typename Dist::LVQDist;

    LSGDistWrapper() = default;
    LSGDistWrapper(LSGDistWrapper &&other) : avg_(std::exchange(other.avg_, nullptr)), alpha_(other.alpha_), dist_(std::move(other.dist_)) {}
    LSGDistWrapper &operator=(LSGDistWrapper &&other) {
        if (this != &other) {
            avg_ = std::exchange(other.avg_, nullptr);
            alpha_ = other.alpha_;
            dist_ = std::move(other.dist_);
        }
        return *this;
    }
    ~LSGDistWrapper() = default;

    LSGDistWrapper(SizeT dim) : dist_(dim) {}

    template <typename DataStore>
    DistanceType operator()(VertexType v1_i, VertexType v2_i, const DataStore &data_store) const {
        DistanceType d = dist_(v1_i, v2_i, data_store);
        return Inner(d, avg_[v1_i], avg_[v2_i]);
    }

    template <typename DataStore>
    DistanceType operator()(const StoreType &v1, VertexType v2_i, const DataStore &data_store, VertexType v1_i = kInvalidVertex) const {
        DistanceType d = dist_(v1, v2_i, data_store, v1_i);
        if (v1_i == kInvalidVertex || avg_.get() == nullptr) {
            return d;
        }
        return Inner(d, avg_[v1_i], avg_[v2_i]);
    }

    void SetLSGParam(float alpha, UniquePtr<float[]> avg) {
        alpha_ = alpha;
        avg_ = std::move(avg);
    }

    LVQDist ToLVQDistance(SizeT dim) && {
        if constexpr (std::is_same_v<typename Dist::This, typename Dist::LVQDist>) {
            return std::move(dist_);
        } else {
            return std::move(dist_).ToLVQDistance(dim);
        }
    }

private:
    DistanceType Inner(DistanceType d, float gt1, float gt2) const {
        float k = std::pow(std::sqrt(gt1 * gt2), alpha_);
        if (k == 0.0) {
            return std::numeric_limits<DistanceType>::max();
        }
        return d / k;
    }

private:
    UniquePtr<float[]> avg_;
    float alpha_ = 1.0;
    Dist dist_;
};

export template <typename DataType>
using PlainL2LSGDist = LSGDistWrapper<PlainL2Dist<DataType>, PlainVecStoreMeta<DataType>>;

export template <typename DataType>
using PlainIPLSGDist = LSGDistWrapper<PlainIPDist<DataType>, PlainVecStoreMeta<DataType>>;

export template <typename DataType>
using PlainCosLSGDist = LSGDistWrapper<PlainCosDist<DataType>, PlainVecStoreMeta<DataType>>;

export template <typename DataType, typename CompressType, typename LVQCache>
using LVQL2LSGDist = LSGDistWrapper<LVQL2Dist<DataType, CompressType>, LVQVecStoreMeta<DataType, CompressType, LVQCache, true>>;

export template <typename DataType, typename CompressType, typename LVQCache>
using LVQIPLSGDist = LSGDistWrapper<LVQIPDist<DataType, CompressType>, LVQVecStoreMeta<DataType, CompressType, LVQCache, true>>;

export template <typename DataType, typename CompressType, typename LVQCache>
using LVQCosLSGDist = LSGDistWrapper<LVQCosDist<DataType, CompressType>, LVQVecStoreMeta<DataType, CompressType, LVQCache, true>>;

export template <typename Distance>
concept IsLSGDistance = requires { typename Distance::LSG; };

static_assert(IsLSGDistance<PlainL2LSGDist<f32>>);

} // namespace infinity
