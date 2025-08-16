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

export module infinity_core:vec_store_type;

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

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQCosVecStoreType;

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQL2VecStoreType;

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQIPVecStoreType;

export template <typename DataT, bool LSG = false>
class PlainCosVecStoreType {
    // friend infinity::DataType;
public:
    using DataType = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<DataType>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<DataType, OwnMem>;
    using QueryVecType = const DataType *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainCosLSGDist<DataType>, PlainCosDist<DataType>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQCosVecStoreType<DataType, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, bool LSG = false>
class PlainL2VecStoreType {
public:
    using DataType = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<DataType>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<DataType, OwnMem>;
    using QueryVecType = const DataType *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainL2LSGDist<DataType>, PlainL2Dist<DataType>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQL2VecStoreType<DataType, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, bool LSG = false>
class PlainIPVecStoreType {
public:
    using DataType = DataT;
    using CompressType = void;
    template <bool OwnMem>
    using Meta = PlainVecStoreMeta<DataType>;
    template <bool OwnMem>
    using Inner = PlainVecStoreInner<DataType, OwnMem>;
    using QueryVecType = const DataType *;
    using StoreType = typename Meta<true>::StoreType;
    using QueryType = typename Meta<true>::QueryType;
    using Distance = std::conditional_t<LSG, PlainIPLSGDist<DataType>, PlainIPDist<DataType>>;

    static constexpr bool HasOptimize = false;

    template <typename CompressType>
    static constexpr LVQIPVecStoreType<DataType, CompressType> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename IndexT>
class SparseIPVecStoreType {
public:
    using DataType = DataT;
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
    static constexpr SparseIPVecStoreType<DataType, IndexT> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQCosVecStoreType {
public:
    using DataType = DataT;
    using CompressType = CompressT;
    using LVQCacheType = LVQCosCache<DataType, CompressType>;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<DataType, CompressType, LVQCacheType, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<DataType, CompressType, LVQCacheType, OwnMem>;
    using QueryVecType = const DataType *;
    using MetaType = LVQVecStoreMetaType<DataType, CompressType, LVQCacheType>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = std::conditional_t<LSG, LVQCosLSGDist<DataType, CompressType, LVQCacheType>, LVQCosDist<DataType, CompressType>>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQCosVecStoreType<DataType, CompressType, LSG> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQL2VecStoreType {
public:
    using DataType = DataT;
    using CompressType = CompressT;
    using LVQCacheType = LVQL2Cache<DataType, CompressType>;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<DataType, CompressType, LVQCacheType, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<DataType, CompressType, LVQCacheType, OwnMem>;
    using QueryVecType = const DataType *;
    using MetaType = LVQVecStoreMetaType<DataType, CompressType, LVQCacheType>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = std::conditional_t<LSG, LVQL2LSGDist<DataType, CompressType, LVQCacheType>, LVQL2Dist<DataType, CompressType>>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQL2VecStoreType<DataType, CompressType, LSG> ToLVQ() {
        return {};
    }
};

export template <typename DataT, typename CompressT, bool LSG = false>
class LVQIPVecStoreType {
public:
    using DataType = DataT;
    using CompressType = CompressT;
    using LVQCacheType = LVQIPCache<DataType, CompressType>;
    template <bool OwnMem>
    using Meta = LVQVecStoreMeta<DataType, CompressType, LVQCacheType, OwnMem>;
    template <bool OwnMem>
    using Inner = LVQVecStoreInner<DataType, CompressType, LVQCacheType, OwnMem>;
    using QueryVecType = const DataType *;
    using MetaType = LVQVecStoreMetaType<DataType, CompressType, LVQCacheType>;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using Distance = std::conditional_t<LSG, LVQIPLSGDist<DataType, CompressType, LVQCacheType>, LVQIPDist<DataType, CompressType>>;

    static constexpr bool HasOptimize = true;

    template <typename CompressType>
    static constexpr LVQIPVecStoreType<DataType, CompressType, LSG> ToLVQ() {
        return {};
    }
};

} // namespace infinity
