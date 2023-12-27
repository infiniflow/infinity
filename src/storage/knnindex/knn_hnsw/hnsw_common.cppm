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
#include <utility>
#include <limits>

import stl;
import file_system;
import infinity_exception;

export module hnsw_common;

namespace infinity {
export constexpr SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

export using MeanType = double;

export template <typename Distance, typename DataType>
concept DistanceConcept = requires(Distance d) {
    { Distance((SizeT)0) };

    {
        d(std::declval<const typename Distance::StoreType &>(),
          std::declval<const typename Distance::StoreType &>(),
          std::declval<const typename Distance::DataStore &>())
    } -> std::same_as<DataType>;
};

export template <typename LVQCache, typename DataType, typename CompressType>
concept LVQCacheConcept = requires(LVQCache) {
    {
        LVQCache::MakeLocalCache(std::declval<const CompressType *>(), std::declval<DataType>(), (SizeT)0, std::declval<const MeanType *>())
    } -> std::same_as<typename LVQCache::LocalCacheType>;

    { LVQCache::MakeGlobalCache(std::declval<const MeanType *>(), (SizeT)0) } -> std::same_as<typename LVQCache::GlobalCacheType>;
};

export template <typename DataStore, typename DataType>
concept DataStoreConcept = requires(DataStore s) {
    { DataStore::Make((SizeT)0, (SizeT)0, std::declval<typename DataStore::InitArgs>()) } -> std::same_as<DataStore>;
    { s.Save(std::declval<FileHandler &>()) };
    { DataStore::Load(std::declval<FileHandler &>(), (SizeT)0, std::declval<typename DataStore::InitArgs>()) } -> std::same_as<DataStore>;

    { DataStore::ERR_IDX };
    { s.cur_vec_num() } -> std::same_as<SizeT>;
    { s.dim() } -> std::same_as<SizeT>;
    { s.max_vec_num() } -> std::same_as<SizeT>;

    // todo: how to add constraint for a template member function.
    // { s.AddVec(iter, (SizeT)0) } -> std::same_as<SizeT>;
    { s.GetVec((SizeT)0) } -> std::same_as<typename DataStore::StoreType>;
    { s.Prefetch((SizeT)0) };
    { s.MakeQuery((const DataType *)nullptr) } -> std::same_as<typename DataStore::QueryType>;
    { typename DataStore::StoreType(std::declval<const typename DataStore::QueryType &>()) };
};

export class DataStoreMeta {
public:
    static constexpr SizeT ERR_IDX = std::numeric_limits<SizeT>::max();

private:
    SizeT cur_vec_num_;

public:
    const SizeT max_vec_num_;
    const SizeT dim_;

public:
    DataStoreMeta(SizeT max_vec_num, SizeT dim) : cur_vec_num_(0), max_vec_num_(max_vec_num), dim_(dim) {}

    DataStoreMeta(DataStoreMeta &&other) = default;
    DataStoreMeta &operator=(DataStoreMeta &&other) {
        cur_vec_num_ = other.cur_vec_num_;
        const_cast<SizeT &>(max_vec_num_) = other.max_vec_num_;
        const_cast<SizeT &>(dim_) = other.dim_;
        return *this;
    }

    SizeT AllocateVec(SizeT alloc_n) {
        if (cur_vec_num_ + alloc_n > max_vec_num_) {
            return ERR_IDX;
        }
        SizeT ret = cur_vec_num_;
        cur_vec_num_ += alloc_n;
        return ret;
    }

    void Save(FileHandler &file_handler) const {
        file_handler.Write(&cur_vec_num_, sizeof(SizeT));
        file_handler.Write(&max_vec_num_, sizeof(SizeT));
        file_handler.Write(&dim_, sizeof(SizeT));
    }

    static DataStoreMeta Load(FileHandler &file_handler, SizeT new_vec_n) {
        SizeT cur_vec_num, max_vec_num, dim;
        file_handler.Read(&cur_vec_num, sizeof(SizeT));
        file_handler.Read(&max_vec_num, sizeof(SizeT));
        if (new_vec_n != 0) {
            if (new_vec_n < cur_vec_num) {
                Error<StorageException>("input vec num is smaller than the stored vec num");
            }
            max_vec_num = new_vec_n;
        }
        file_handler.Read(&dim, sizeof(SizeT));
        DataStoreMeta ret(max_vec_num, dim);
        ret.cur_vec_num_ = cur_vec_num;
        return ret;
    }

public:
    SizeT cur_vec_num() const { return cur_vec_num_; }
};

export using VertexType = i32;
export using AtomicVtxType = ai32;
export using VertexListSize = i32;
export using LayerSize = i32;

export template <typename Iterator, typename DataType>
concept DataIteratorConcept = requires(Iterator iter) {
    { iter.Next() } -> std::same_as<Optional<DataType>>;
};

export template <typename DataType>
class DenseVectorIter {
    const DataType *ptr_;
    const SizeT dim_;
    const SizeT vec_num_;
    const DataType *ptr_end_;

public:
    DenseVectorIter(const DataType *ptr, SizeT dim, SizeT vec_num) : ptr_(ptr), dim_(dim), vec_num_(vec_num), ptr_end_(ptr_ + dim * vec_num) {}

    Optional<const DataType *> Next() {
        auto ret = ptr_;
        if (ret == ptr_end_) {
            return None;
        }
        ptr_ += dim_;
        return ret;
    }
};

} // namespace infinity