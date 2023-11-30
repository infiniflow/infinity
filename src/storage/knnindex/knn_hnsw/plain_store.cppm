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
#include <algorithm>
#include <cassert>
#include <xmmintrin.h>

import stl;
import file_system;
import hnsw_common;

export module plain_store;

namespace infinity {

export template <typename DataType>
class PlainStore {
public:
    using This = PlainStore<DataType>;
    using InitArgs = std::tuple<>;
    using StoreType = const DataType *;
    using QueryType = StoreType;

private:
    DataStoreMeta meta_;
    const UniquePtr<DataType[]> ptr_;

public:
    static This Make(SizeT max_vec_num, SizeT dim, This::InitArgs = {}) {
        DataStoreMeta data_store(max_vec_num, dim);
        return This(Move(data_store));
    }

    PlainStore(DataStoreMeta meta) : meta_(Move(meta)), ptr_(MakeUnique<DataType[]>(meta_.max_vec_num_ * meta_.dim_)) {}

    PlainStore(This &&other) : meta_(Move(other.meta_)), ptr_(Move(const_cast<UniquePtr<DataType[]> &>(other.ptr_))) {}
    PlainStore &operator=(This &&other) {
        meta_ = Move(other.meta_);
        const_cast<UniquePtr<DataType[]> &>(ptr_) = Move(const_cast<UniquePtr<DataType[]> &>(other.ptr_));
        return *this;
    }

    ~PlainStore() = default;

    void Save(FileHandler &file_handler) const {
        meta_.Save(file_handler);
        file_handler.Write(ptr_.get(), sizeof(DataType) * cur_vec_num() * dim());
    }

    static This Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs = {}) {
        DataStoreMeta meta = DataStoreMeta::Load(file_handler, max_vec_num);
        This ret(Move(meta));
        file_handler.Read(ret.ptr_.get(), sizeof(DataType) * ret.cur_vec_num() * ret.dim());
        return ret;
    }

public:
    static constexpr SizeT ERR_IDX = DataStoreMeta::ERR_IDX;
    SizeT cur_vec_num() const { return meta_.cur_vec_num(); }
    SizeT max_vec_num() const { return meta_.max_vec_num_; }
    SizeT dim() const { return meta_.dim_; }

public:
    SizeT AddVec(const DataType *vecs, SizeT vec_num) {
        SizeT new_idx = meta_.AllocateVec(vec_num);
        if (new_idx != ERR_IDX) {
            DataType *ptr = ptr_.get() + new_idx * dim();
            std::copy(vecs, vecs + vec_num * dim(), ptr);
        }
        return new_idx;
    }

    StoreType GetVec(SizeT vec_i) const {
        assert(vec_i < cur_vec_num());
        return ptr_.get() + vec_i * dim();
    }

    QueryType MakeQuery(const DataType *vec) const { return vec; }

    void Prefetch(SizeT vec_i) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i)), _MM_HINT_T0); }
};
} // namespace infinity