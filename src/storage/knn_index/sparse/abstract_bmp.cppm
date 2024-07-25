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
struct SegmentEntry;
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

namespace infinity {

class BufferManager;
class ChunkIndexEntry;
class SegmentIndexEntry;
class BlockColumnEntry;

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
    static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp) {
        switch (index_bmp->compress_type_) {
            case BMPCompressType::kCompressed: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kCompressed>;
                return static_cast<BMPIndex *>(nullptr);
            }
            case BMPCompressType::kRaw: {
                using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kRaw>;
                return static_cast<BMPIndex *>(nullptr);
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
                return InitAbstractIndex<DataType, i8>(index_bmp);
            }
            case EmbeddingDataType::kElemInt16: {
                return InitAbstractIndex<DataType, i16>(index_bmp);
            }
            case EmbeddingDataType::kElemInt32: {
                return InitAbstractIndex<DataType, i32>(index_bmp);
            }
            default: {
                return nullptr;
            }
        }
    }

public:
    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def);

    ~BMPIndexInMem();

    SizeT GetRowCount() const;

    void AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count);

    void AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts);

    const AbstractBMP &get() const { return bmp_; }

    AbstractBMP &get_ref() { return bmp_; }

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr);

private:
    RowID begin_row_id_ = {};
    AbstractBMP bmp_ = nullptr;
};

} // namespace infinity