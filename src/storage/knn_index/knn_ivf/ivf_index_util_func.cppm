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

export module ivf_index_util_func;

import stl;
import internal_types;
import column_vector;

namespace infinity {

export template <IsAnyOf<u8, i8, f64, f32, Float16T, BFloat16T> ColumnEmbeddingElementT>
Pair<const f32 *, UniquePtr<f32[]>> GetF32Ptr(const ColumnEmbeddingElementT *src_data_ptr, const u32 src_data_cnt) {
    Pair<const f32 *, UniquePtr<f32[]>> dst_data_ptr;
    if constexpr (std::is_same_v<f32, ColumnEmbeddingElementT>) {
        dst_data_ptr.first = src_data_ptr;
    } else {
        dst_data_ptr.second = MakeUniqueForOverwrite<f32[]>(src_data_cnt);
        dst_data_ptr.first = dst_data_ptr.second.get();
        for (u32 i = 0; i < src_data_cnt; ++i) {
            if constexpr (std::is_same_v<f64, ColumnEmbeddingElementT>) {
                dst_data_ptr.second[i] = static_cast<f32>(src_data_ptr[i]);
            } else {
                dst_data_ptr.second[i] = src_data_ptr[i];
            }
        }
    }
    return dst_data_ptr;
}

export template <IsAnyOf<u8, i8, f32> QueryDataType, IsAnyOf<u8, i8, f64, f32, Float16T, BFloat16T> ColumnEmbeddingElementT>
Pair<const QueryDataType *, UniquePtr<QueryDataType[]>> GetSearchCalcPtr(const ColumnEmbeddingElementT *src_data_ptr, const u32 src_data_cnt) {
    if constexpr (std::is_same_v<QueryDataType, ColumnEmbeddingElementT>) {
        return {src_data_ptr, nullptr};
    } else {
        static_assert(std::is_same_v<QueryDataType, f32>);
        static_assert(IsAnyOf<ColumnEmbeddingElementT, f64, Float16T, BFloat16T>);
        return GetF32Ptr(src_data_ptr, src_data_cnt);
    }
}

template <EmbeddingDataType t>
struct EmbeddingDataTypeToCppType;

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemInt8> {
    using type = i8;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemUInt8> {
    using type = u8;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemDouble> {
    using type = f64;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemFloat> {
    using type = f32;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemFloat16> {
    using type = Float16T;
};

template <>
struct EmbeddingDataTypeToCppType<EmbeddingDataType::kElemBFloat16> {
    using type = BFloat16T;
};

export template <EmbeddingDataType t>
using EmbeddingDataTypeToCppTypeT = typename EmbeddingDataTypeToCppType<t>::type;

} // namespace infinity
