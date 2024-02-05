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

#include <cassert>
#include <xmmintrin.h>

import stl;
import file_system;
import hnsw_common;
import infinity_exception;

export module plain_store;

namespace infinity {

export template <typename DataType, typename LabelType>
class PlainStore {
public:
    using This = PlainStore<DataType, LabelType>;
    using InitArgs = Tuple<>;
    using StoreType = const DataType *;
    using QueryType = StoreType;

private:
    DataStoreMeta meta_;
    UniquePtr<DataType[]> ptr_;
    UniquePtr<LabelType[]> labels_;

public:
    static This Make(SizeT max_vec_num, SizeT dim, This::InitArgs = {}) {
        DataStoreMeta data_store(max_vec_num, dim);
        return This(std::move(data_store));
    }

    PlainStore(DataStoreMeta meta)
        : meta_(std::move(meta)), ptr_(MakeUnique<DataType[]>(meta_.max_vec_num() * meta_.dim())),
          labels_(MakeUnique<LabelType[]>(meta_.max_vec_num())) {}

    void Save(FileHandler &file_handler) const {
        meta_.Save(file_handler);
        file_handler.Write(ptr_.get(), sizeof(DataType) * cur_vec_num() * dim());
    }

    static This Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs = {}) {
        DataStoreMeta meta = DataStoreMeta::Load(file_handler, max_vec_num);
        This ret(std::move(meta));
        file_handler.Read(ret.ptr_.get(), sizeof(DataType) * ret.cur_vec_num() * ret.dim());
        return ret;
    }

public:
    SizeT cur_vec_num() const { return meta_.cur_vec_num(); }
    SizeT max_vec_num() const { return meta_.max_vec_num(); }
    SizeT dim() const { return meta_.dim(); }

public:
    SizeT AddVec(const DataType *vec, SizeT vec_num) { return AddVec(DenseVectorIterator(vec, dim()), vec_num); }

    template <DataIteratorConcept<const DataType *, LabelType> Iterator>
    SizeT AddVec(Iterator query_iter, SizeT vec_num) {
        SizeT new_idx = meta_.AllocateVec(vec_num);
        DataType *ptr = ptr_.get() + new_idx * dim();

        SizeT actual_size = 0;
        while (true) {
            auto vec_opt = query_iter.Next();
            if (!vec_opt.has_value()) {
                break;
            }
            const auto &[vec, label] = vec_opt.value();
            Copy(vec, vec + dim(), ptr);
            labels_[new_idx + actual_size] = label;
            ptr += dim();
            ++actual_size;
            if (actual_size > vec_num) {
                UnrecoverableError("vec_num is too small");
            }
        }
        meta_.ReturnNotUsed(vec_num - actual_size);

        return new_idx;
    }

    StoreType GetVec(SizeT vec_i) const {
        assert(vec_i < cur_vec_num());
        return ptr_.get() + vec_i * dim();
    }

    QueryType MakeQuery(const DataType *vec) const { return vec; }

    LabelType GetLabel(VertexType vec_i) const {
        if ((SizeT)vec_i >= cur_vec_num()) {
            UnrecoverableError("vec_i is out of range");
        }
        return labels_[vec_i];
    }

    void Prefetch(SizeT vec_i) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i)), _MM_HINT_T0); }
};
} // namespace infinity