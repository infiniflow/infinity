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

#include <concepts>
#include <limits>

import stl;
import file_system;
import infinity_exception;

export module hnsw_common;

namespace infinity {
export constexpr SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

export template <typename Store, typename DataType>
concept DataStoreConcept = requires(Store s) {
    { std::same_as<typename Store::DataType, DataType> };
    { s.AddVec(std::declval<const typename Store::DataType *>(), std::declval<SizeT>()) } -> std::same_as<SizeT>;
    { s.GetVec(std::declval<SizeT>()) } -> std::same_as<typename Store::RtnType>;
    { s.MakeCtx(std::declval<const typename Store::DataType *>()) } -> std::same_as<typename Store::QueryCtx>;
    { s.GetVec(std::declval<const typename Store::QueryCtx &>()) } -> std::same_as<typename Store::RtnType>;
    { Store::ERR_IDX };
    { s.cur_vec_num() } -> std::same_as<SizeT>;
    { s.dim() } -> std::same_as<SizeT>;
    { s.max_vec_num() } -> std::same_as<SizeT>;
    { s.Save(std::declval<FileHandler &>()) };
    { Store::Load(std::declval<FileHandler &>(), std::declval<SizeT>(), std::declval<typename Store::InitArgs>()) } -> std::same_as<Store>;
    { Store::Make(std::declval<SizeT>(), std::declval<SizeT>(), std::declval<typename Store::InitArgs>()) } -> std::same_as<Store>;
    { s.Prefetch(std::declval<SizeT>()) };
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
            Assert<StorageException>(new_vec_n >= cur_vec_num, "input vec num is smaller than the stored vec num");
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
export using VertexListSize = i32;
export using LayerSize = i32;

} // namespace infinity