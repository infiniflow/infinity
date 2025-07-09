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

export module infinity_core:vec_store_type;

import :stl;
import :plain_vec_store;
import :sparse_vec_store;
import :lvq_vec_store;
import :dist_func_cos;
import :dist_func_l2;
import :dist_func_ip;
import :dist_func_sparse_ip;
import :sparse_util;
import :dist_func_lsg_wrapper;
import data_type;

namespace infinity {

export template <typename DataT, typename CompressT>
class LVQCosVecStoreType;

export template <typename DataT, typename CompressT>
class LVQL2VecStoreType;

export template <typename DataT, typename CompressT>
class LVQIPVecStoreType;

export template <typename DataT, bool LSG = false>
class PlainCosVecStoreType {
    // friend infinity::DataType;
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<JustMoreMisleadingName>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<JustMoreMisleadingName, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainCosLSGDist<JustMoreMisleadingName>, PlainCosDist<JustMoreMisleadingName>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQCosVecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, bool LSG = false>
class PlainL2VecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<JustMoreMisleadingName>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<JustMoreMisleadingName, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainL2LSGDist<JustMoreMisleadingName>, PlainL2Dist<JustMoreMisleadingName>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQL2VecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, bool LSG = false>
class PlainIPVecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<JustMoreMisleadingName>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<JustMoreMisleadingName, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainIPLSGDist<JustMoreMisleadingName>, PlainIPDist<JustMoreMisleadingName>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQIPVecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename IndexT>
class SparseIPVecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = SparseVecStoreMeta<DataT, IndexT>;
    template <bool OwnMem>
    using Inner = SparseVecStoreInner<DataT, IndexT>;
    using QueryVecType = SparseVecRef<DataT, IndexT>;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = SparseIPDist<DataT, IndexT>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr SparseIPVecStoreType<JustMoreMisleadingName, IndexT> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT>
class LVQCosVecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = CompressT;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<JustMoreMisleadingName, CompressType, LVQCosCache<JustMoreMisleadingName, CompressType>, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<JustMoreMisleadingName, CompressType, LVQCosCache<JustMoreMisleadingName, CompressType>, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using MetaType = LVQVecStoreMetaType<JustMoreMisleadingName, CompressType, LVQCosCache<JustMoreMisleadingName, CompressType>>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = LVQCosDist<JustMoreMisleadingName, CompressType>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQCosVecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT>
class LVQL2VecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = CompressT;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<JustMoreMisleadingName, CompressType, LVQL2Cache<JustMoreMisleadingName, CompressType>, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<JustMoreMisleadingName, CompressType, LVQL2Cache<JustMoreMisleadingName, CompressType>, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using MetaType = LVQVecStoreMetaType<JustMoreMisleadingName, CompressType, LVQL2Cache<JustMoreMisleadingName, CompressType>>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = LVQL2Dist<JustMoreMisleadingName, CompressType>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQL2VecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT>
class LVQIPVecStoreType {
public:
    using JustMoreMisleadingName = DataT;
    using CompressType = CompressT;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<JustMoreMisleadingName, CompressType, LVQIPCache<JustMoreMisleadingName, CompressType>, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<JustMoreMisleadingName, CompressType, LVQIPCache<JustMoreMisleadingName, CompressType>, OwnMem>;
    using QueryVecType = const JustMoreMisleadingName *;
    using MetaType = LVQVecStoreMetaType<JustMoreMisleadingName, CompressType, LVQIPCache<JustMoreMisleadingName, CompressType>>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = LVQIPDist<JustMoreMisleadingName, CompressType>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQIPVecStoreType<JustMoreMisleadingName, CompressType> ToLVQ() {
        return {};
    }
};

} // namespace infinity
