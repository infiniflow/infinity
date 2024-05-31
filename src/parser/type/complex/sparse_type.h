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

#pragma once

#include "tensor_type.h"
#include <cstdint>
#include <sstream>
#include <string>

namespace infinity {

#pragma pack(1)

struct SparseType {
    template <typename DataType, typename IndiceType>
    static std::string Sparse2StringT2(const DataType *data_ptr, const IndiceType *indice_ptr, size_t nnz) {
        if (nnz == 0) {
            return {};
        }
        std::stringstream ss;
        if constexpr (std::is_same_v<DataType, bool>) {
            for (size_t i = 0; i < nnz; ++i) {
                ss << std::to_string(indice_ptr[i]);
                if (i < nnz - 1) {
                    ss << ", ";
                }
            }
        } else {
            for (size_t i = 0; i < nnz; ++i) {
                ss << std::to_string(indice_ptr[i]) << ": " << std::to_string(data_ptr[i]);
                if (i < nnz - 1) {
                    ss << ", ";
                }
            }
        }
        return std::move(ss).str();
    }

    template <typename DataType>
    static std::string Sparse2StringT1(const DataType *data_ptr, const char *indice_ptr, EmbeddingDataType indice_type, size_t nnz) {
        switch (indice_type) {
            case EmbeddingDataType::kElemInt8: {
                return Sparse2StringT2(data_ptr, reinterpret_cast<const int8_t *>(indice_ptr), nnz);
            }
            case EmbeddingDataType::kElemInt16: {
                return Sparse2StringT2(data_ptr, reinterpret_cast<const int16_t *>(indice_ptr), nnz);
            }
            case EmbeddingDataType::kElemInt32: {
                return Sparse2StringT2(data_ptr, reinterpret_cast<const int32_t *>(indice_ptr), nnz);
            }
            case EmbeddingDataType::kElemInt64: {
                return Sparse2StringT2(data_ptr, reinterpret_cast<const int64_t *>(indice_ptr), nnz);
            }
            default: {
                return {};
            }
        }
    }

    int64_t nnz_ = 0;
    uint32_t chunk_id_ = 0;
    uint32_t chunk_offset_ = 0;

    static std::string
    Sparse2String(const char *data_ptr, const char *indice_ptr, EmbeddingDataType data_type, EmbeddingDataType indice_type, size_t nnz) {
        switch (data_type) {
            case EmbeddingDataType::kElemBit: {
                return Sparse2StringT1(reinterpret_cast<const bool *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemInt8: {
                return Sparse2StringT1(reinterpret_cast<const int8_t *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemInt16: {
                return Sparse2StringT1(reinterpret_cast<const int16_t *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemInt32: {
                return Sparse2StringT1(reinterpret_cast<const int32_t *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemInt64: {
                return Sparse2StringT1(reinterpret_cast<const int64_t *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemFloat: {
                return Sparse2StringT1(reinterpret_cast<const float *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            case EmbeddingDataType::kElemDouble: {
                return Sparse2StringT1(reinterpret_cast<const double *>(data_ptr), indice_ptr, indice_type, nnz);
            }
            default: {
                return {};
            }
        }
    }
};

struct SparseRefType {
    char *raw_data_ptr_;
    char *raw_indice_ptr_;
    size_t nnz_;
};

#pragma pack()

} // namespace infinity