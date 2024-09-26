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
module ivf_index_storage;

import stl;
import infinity_exception;
import status;
import logger;
import third_party;
import index_ivf;
import file_system;
import column_vector;
import internal_types;

namespace infinity {

// IVF_Centroids_Storage

IVF_Centroids_Storage::IVF_Centroids_Storage(const u32 embedding_dimension, const u32 centroids_num)
    : embedding_dimension_(embedding_dimension), centroids_num_(centroids_num) {
    const auto alloc_size = centroids_num_ * (embedding_dimension_ * sizeof(f32));
    centroids_data_ = MakeUniqueForOverwrite<f32[]>(alloc_size);
}

void IVF_Centroids_Storage::Save(FileHandler &file_handler) const {
    file_handler.Write(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handler.Write(&centroids_num_, sizeof(centroids_num_));
    const auto alloc_size = centroids_num_ * (embedding_dimension_ * sizeof(f32));
    file_handler.Write(centroids_data_.get(), alloc_size);
}

void IVF_Centroids_Storage::Load(FileHandler &file_handler) {
    file_handler.Read(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handler.Read(&centroids_num_, sizeof(centroids_num_));
    const auto alloc_size = centroids_num_ * (embedding_dimension_ * sizeof(f32));
    centroids_data_ = MakeUniqueForOverwrite<f32[]>(alloc_size);
    file_handler.Read(centroids_data_.get(), alloc_size);
}

// IVF_Part_Storage

void IVF_Part_Storage::Save(FileHandler &file_handler) const {
    file_handler.Write(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handler.Write(&part_id_, sizeof(part_id_));
    file_handler.Write(&embedding_num_, sizeof(embedding_num_));
    static_assert(std::is_same_v<SegmentOffset, typename decltype(embedding_segment_offsets_)::value_type>);
    assert(embedding_num_ == embedding_segment_offsets_.size());
    file_handler.Write(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
}

void IVF_Part_Storage::Load(FileHandler &file_handler) {
    file_handler.Read(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handler.Read(&part_id_, sizeof(part_id_));
    file_handler.Read(&embedding_num_, sizeof(embedding_num_));
    embedding_segment_offsets_.resize(embedding_num_);
    file_handler.Read(embedding_segment_offsets_.data(), embedding_num_ * sizeof(SegmentOffset));
}

template <IsAnyOf<i8, u8, f32, Float16T, BFloat16T> StorageDataT>
class IVF_Part_Storage_Plain final : public IVF_Part_Storage {
    Vector<StorageDataT> data_;

public:
    IVF_Part_Storage_Plain(const u32 embedding_dimension, const u32 part_id) : IVF_Part_Storage(embedding_dimension, part_id) {}

    void Save(FileHandler &file_handler) const override {
        IVF_Part_Storage::Save(file_handler);
        const u32 element_cnt = embedding_num() * embedding_dimension();
        assert(element_cnt == data_.size());
        file_handler.Write(data_.data(), element_cnt * sizeof(StorageDataT));
    }
    void Load(FileHandler &file_handler) override {
        IVF_Part_Storage::Load(file_handler);
        const u32 element_cnt = embedding_num() * embedding_dimension();
        data_.resize(element_cnt);
        file_handler.Read(data_.data(), element_cnt * sizeof(StorageDataT));
    }

    // only for unit-test, return f32 / i8 / u8 embedding data
    Pair<const void *, SharedPtr<void>> GetDataForTest(const u32 embedding_id) const override {
        if constexpr (IsAnyOf<StorageDataT, i8, u8, f32>) {
            return {data_.data() + embedding_id * embedding_dimension(), nullptr};
        } else if constexpr (IsAnyOf<StorageDataT, Float16T, BFloat16T>) {
            auto tmp_data = MakeUniqueForOverwrite<f32[]>(embedding_dimension());
            const auto *start_ptr = data_.data() + embedding_id * embedding_dimension();
            for (u32 i = 0; i < embedding_dimension(); ++i) {
                tmp_data[i] = static_cast<f32>(start_ptr[i]);
            }
            Pair<const void *, SharedPtr<void>> result(tmp_data.get(),
                                                       SharedPtr<void>(tmp_data.get(), [](void *ptr) { delete[] static_cast<f32 *>(ptr); }));
            tmp_data.release();
            return result;
        } else {
            static_assert(false, "unexpected type");
            return {};
        }
    }
};

UniquePtr<IVF_Part_Storage> IVF_Part_Storage::Make(u32 embedding_dimension, u32 part_id, const IndexIVFStorageOption &ivf_storage_option) {
    switch (ivf_storage_option.type_) {
        case IndexIVFStorageOption::Type::kPlain: {
            auto GetPlainResult = [embedding_dimension, part_id]<typename PlainStorageElementT> {
                return MakeUnique<IVF_Part_Storage_Plain<PlainStorageElementT>>(embedding_dimension, part_id);
            };
            switch (ivf_storage_option.plain_data_type_) {
                case EmbeddingDataType::kElemInt8: {
                    return GetPlainResult.operator()<i8>();
                }
                case EmbeddingDataType::kElemUInt8: {
                    return GetPlainResult.template operator()<u8>();
                }
                case EmbeddingDataType::kElemFloat: {
                    return GetPlainResult.template operator()<f32>();
                }
                case EmbeddingDataType::kElemFloat16: {
                    return GetPlainResult.template operator()<Float16T>();
                }
                case EmbeddingDataType::kElemBFloat16: {
                    return GetPlainResult.template operator()<BFloat16T>();
                }
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemBit:
                case EmbeddingDataType::kElemInt16:
                case EmbeddingDataType::kElemInt32:
                case EmbeddingDataType::kElemInt64:
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid IVF plain_data_type");
                    return {};
                }
            }
        }
        case IndexIVFStorageOption::Type::kScalarQuantization: {
            UnrecoverableError("Not implemented");
            return {};
        }
        case IndexIVFStorageOption::Type::kProductQuantization: {
            UnrecoverableError("Not implemented");
            return {};
        }
    }
}


} // namespace infinity
