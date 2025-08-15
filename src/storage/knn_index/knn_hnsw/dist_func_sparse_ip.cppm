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

export module infinity_core:dist_func_sparse_ip;

import :stl;
import :sparse_vec_store;
import :sparse_vector_distance;
import :sparse_util;
import :hnsw_common;

namespace infinity {

export template <typename DataType, typename IdxType>
class SparseIPDist {
public:
    using VecStoreMeta = SparseVecStoreMeta<DataType, IdxType>;
    using DistanceType = typename VecStoreMeta::DistanceType;
    using StoreType = typename VecStoreMeta::StoreType;

public:
    SparseIPDist() = default;
    SparseIPDist(size_t dim) {}

    template <typename DataStore>
    DistanceType operator()(VertexType v1_i, VertexType v2_i, const DataStore &data_store) const {
        return Inner(data_store.GetVec(v1_i), data_store.GetVec(v2_i));
    }

    template <typename DataStore>
    DistanceType operator()(const StoreType &v1, VertexType v2_i, const DataStore &data_store, VertexType v1_i = kInvalidVertex) const {
        return Inner(v1, data_store.GetVec(v2_i));
    }

private:
    DistanceType Inner(const StoreType &v1, const StoreType &v2) const {
        return -SparseIPDistance(v1.data_, v1.indices_, v1.nnz_, v2.data_, v2.indices_, v2.nnz_);
    }
};

} // namespace infinity