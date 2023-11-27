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
#include "storage/knnindex/header.h"

import stl;
import file_system;
import hnsw_common;

export module plain_store;

namespace infinity {

export template <typename T>
class PlainStore {
public:
    using This = PlainStore<T>;
    using InitArgs = std::tuple<>;
    using DataType = T;
    using RtnType = const T *;
    using QueryCtx = const T *;

    static constexpr SizeT ERR_IDX = DataStoreMeta::ERR_IDX;

private:
    DataStoreMeta base_;
    const UniquePtr<T[]> ptr_;

public:
    static This Make(SizeT max_vec_num, SizeT dim, This::InitArgs = {}) {
        DataStoreMeta data_store(max_vec_num, dim);
        return This(Move(data_store));
    }

    PlainStore(DataStoreMeta base) : base_(Move(base)), ptr_(MakeUnique<T[]>(base_.max_vec_num_ * base_.dim_)) {}

    PlainStore(PlainStore &&other) : base_(Move(other.base_)), ptr_(Move(const_cast<UniquePtr<T[]> &>(other.ptr_))) {}
    PlainStore &operator=(PlainStore &&other) {
        base_ = Move(other.base_);
        const_cast<UniquePtr<T[]> &>(ptr_) = Move(const_cast<UniquePtr<T[]> &>(other.ptr_));
        return *this;
    }

    ~PlainStore() = default;

    SizeT cur_vec_num() const { return base_.cur_vec_num(); }
    SizeT max_vec_num() const { return base_.max_vec_num_; }
    SizeT dim() const { return base_.dim_; }

    static SizeT err_idx() { return DataStoreMeta::ERR_IDX; }

public:
    SizeT AddVec(const DataType *vecs, SizeT vec_num) {
        SizeT new_idx = base_.AllocateVec(vec_num);
        if (new_idx != err_idx()) {
            T *ptr = ptr_.get() + new_idx * dim();
            std::copy(vecs, vecs + vec_num * dim(), ptr);
        }
        return new_idx;
    }

    RtnType GetVec(SizeT vec_i) const {
        assert(vec_i < cur_vec_num());
        return ptr_.get() + vec_i * dim();
    }

    RtnType GetVec(const QueryCtx &ctx) const { return ctx; }

    void Save(FileHandler &file_handler) const {
        base_.Save(file_handler);
        file_handler.Write(ptr_.get(), sizeof(T) * cur_vec_num() * dim());
    }

    static PlainStore Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs = {}) {
        DataStoreMeta data_store = DataStoreMeta::Load(file_handler, max_vec_num);
        PlainStore ret(Move(data_store));
        file_handler.Read(ret.ptr_.get(), sizeof(T) * ret.cur_vec_num() * ret.dim());
        return ret;
    }

    void Prefetch(SizeT vec_i) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i)), _MM_HINT_T0); }
};
} // namespace infinity