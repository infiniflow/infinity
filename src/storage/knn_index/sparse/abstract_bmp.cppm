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

export module abstract_bmp;

import stl;
import bmp_alg;
import bmp_util;
import index_base;
import column_def;
import internal_types;
import index_bmp;
import sparse_info;
import internal_types;

namespace infinity {

class BufferManager;
class ChunkIndexEntry;
class SegmentIndexEntry;

export using AbstractBMP = std::variant<BMPAlg<f32, i32, BMPCompressType::kCompressed> *,
                                        BMPAlg<f32, i32, BMPCompressType::kRaw> *,
                                        BMPAlg<f32, i16, BMPCompressType::kCompressed> *,
                                        BMPAlg<f32, i16, BMPCompressType::kRaw> *,
                                        BMPAlg<f32, i8, BMPCompressType::kCompressed> *,
                                        BMPAlg<f32, i8, BMPCompressType::kRaw> *,
                                        BMPAlg<f64, i32, BMPCompressType::kCompressed> *,
                                        BMPAlg<f64, i32, BMPCompressType::kRaw> *,
                                        BMPAlg<f64, i16, BMPCompressType::kCompressed> *,
                                        BMPAlg<f64, i16, BMPCompressType::kRaw> *,
                                        BMPAlg<f64, i8, BMPCompressType::kCompressed> *,
                                        BMPAlg<f64, i8, BMPCompressType::kRaw> *,
                                        std::nullptr_t>;

export struct BMPIndexInMem {
public:
    BMPIndexInMem() : bmp_(nullptr) {}

    BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
        : begin_row_id_(begin_row_id), bmp_(InitAbstractIndex(index_base, column_def)) {}

private:
    template <typename DataType, typename IndexType>
    static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info) {
        SizeT term_num = sparse_info->Dimension();
        SizeT block_size = index_bmp->block_size_;
        switch (index_bmp->compress_type_) {
            case BMPCompressType::kCompressed: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kCompressed>;
                return new BMPIndex(term_num, block_size);
            }
            case BMPCompressType::kRaw: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kRaw>;
                return new BMPIndex(term_num, block_size);
            }
            default: {
                return nullptr;
            }
        }
    }

    template <typename DataType>
    static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info) {
        switch (sparse_info->IndexType()) {
            case EmbeddingDataType::kElemInt8: {
                return InitAbstractIndex<DataType, i8>(index_bmp, sparse_info);
            }
            case EmbeddingDataType::kElemInt16: {
                return InitAbstractIndex<DataType, i16>(index_bmp, sparse_info);
            }
            case EmbeddingDataType::kElemInt32: {
                return InitAbstractIndex<DataType, i32>(index_bmp, sparse_info);
            }
            default: {
                return nullptr;
            }
        }
    }

public:
    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
        const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
        const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());

        switch (sparse_info->DataType()) {
            case EmbeddingDataType::kElemFloat: {
                return InitAbstractIndex<f32>(index_bmp, sparse_info);
            }
            case EmbeddingDataType::kElemDouble: {
                return InitAbstractIndex<f64>(index_bmp, sparse_info);
            }
            default: {
                return nullptr;
            }
        }
    }

    const AbstractBMP &get() const { return bmp_; }

    AbstractBMP &get_ref() { return bmp_; }

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr);

    SizeT GetRowCount() const {
        return std::visit(
            [](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return SizeT(0);
                } else {
                    return index->DocNum();
                }
            },
            bmp_);
    }

    ~BMPIndexInMem() {
        std::visit(
            [](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return;
                } else {
                    if (index != nullptr) {
                        delete index;
                    }
                }
            },
            bmp_);
    }

private:
    RowID begin_row_id_ = {};
    AbstractBMP bmp_ = nullptr;
};

} // namespace infinity