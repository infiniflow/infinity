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

namespace infinity {
struct ChunkIndexMetaInfo;
}

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
import buffer_handle;
import base_memindex;
import memindex_tracer;
import table_index_entry;

namespace infinity {

class BufferManager;
class ColumnVector;
class BufferObj;

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

                                        BMPAlg<f32, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f32, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,
                                        BMPAlg<f32, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f32, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,
                                        BMPAlg<f32, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f32, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse> *,
                                        BMPAlg<f64, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse> *,

                                        std::nullptr_t>;

export struct BMPIndexInMem final : public BaseMemIndex {
public:
    BMPIndexInMem() : bmp_(nullptr) {}

    BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def);

    MemIndexTracerInfo GetInfo() const override;

private:
    template <typename DataType, typename IndexType, BMPOwnMem OwnMem>
    static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp) {
        switch (index_bmp->compress_type_) {
            case BMPCompressType::kCompressed: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kCompressed, OwnMem>;
                return static_cast<BMPIndex *>(nullptr);
            }
            case BMPCompressType::kRaw: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kRaw, OwnMem>;
                return static_cast<BMPIndex *>(nullptr);
            }
            default: {
                return nullptr;
            }
        }
    }

    template <typename DataType, BMPOwnMem OwnMem>
    static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info) {
        switch (sparse_info->IndexType()) {
            case EmbeddingDataType::kElemInt8: {
                return InitAbstractIndex<DataType, i8, OwnMem>(index_bmp);
            }
            case EmbeddingDataType::kElemInt16: {
                return InitAbstractIndex<DataType, i16, OwnMem>(index_bmp);
            }
            case EmbeddingDataType::kElemInt32: {
                return InitAbstractIndex<DataType, i32, OwnMem>(index_bmp);
            }
            default: {
                return nullptr;
            }
        }
    }

    template <BMPOwnMem OwnMem>
    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
        const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
        const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());

        switch (sparse_info->DataType()) {
            case EmbeddingDataType::kElemFloat: {
                return InitAbstractIndex<f32, OwnMem>(index_bmp, sparse_info);
            }
            case EmbeddingDataType::kElemDouble: {
                return InitAbstractIndex<f64, OwnMem>(index_bmp, sparse_info);
            }
            default: {
                return nullptr;
            }
        }
    }

public:
    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true) {
        if (own_mem) {
            return InitAbstractIndex<BMPOwnMem::kTrue>(index_base, column_def);
        } else {
            return InitAbstractIndex<BMPOwnMem::kFalse>(index_base, column_def);
        }
    }

    BMPIndexInMem(const BMPIndexInMem &) = delete;
    BMPIndexInMem &operator=(const BMPIndexInMem &) = delete;

    ~BMPIndexInMem();

    RowID GetBeginRowID() const { return begin_row_id_; }

    SizeT GetRowCount() const;

    SizeT GetSizeInBytes() const;

    void AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

    const AbstractBMP &get() const { return bmp_; }

    AbstractBMP &get_ref() { return bmp_; }

    void Dump(BufferObj *buffer_obj, SizeT *dump_size = nullptr);

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

private:
    RowID begin_row_id_ = {};
    AbstractBMP bmp_ = nullptr;
    mutable bool own_memory_ = true;
    mutable BufferHandle chunk_handle_{};
};

} // namespace infinity
