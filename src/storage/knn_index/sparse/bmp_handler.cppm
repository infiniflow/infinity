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

export module bmp_handler;

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
import sparse_util;
import infinity_exception;
import chunk_index_meta;

namespace infinity {

class BufferManager;
struct ChunkIndexEntry;
struct SegmentIndexEntry;
struct BlockColumnEntry;
class ColumnVector;
class BufferObj;
class LocalFileHandle;

using AbstractBMP = std::variant<BMPAlg<f32, i32, BMPCompressType::kCompressed> *,
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

export class BMPHandler {
public:
    BMPHandler() : bmp_(nullptr) {}
    BMPHandler(const void *ptr) : bmp_(*reinterpret_cast<const AbstractBMP *>(ptr)) {}
    ~BMPHandler();

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

    BMPHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    static UniquePtr<BMPHandler> Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    SizeT AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count);

    SizeT AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

    void AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts);

    void AddDocs(int row_count, const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts);

    template <typename ResultType, typename DistFunc, typename Filter = NoneType, typename MergeHeap = NoneType>
    void SearchIndex(const auto &query,
                     i32 topn,
                     const BmpSearchOptions &options,
                     const Filter &filter,
                     SizeT query_id,
                     SegmentID segment_id,
                     MergeHeap *merge_heap) const {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (std::is_same_v<typename IndexT::DataT, typename DistFunc::DataT> &&
                                  std::is_same_v<typename IndexT::IdxT, typename DistFunc::IndexT>) {
                        auto [doc_ids, scores] = index->SearchKnn(query, topn, options, filter);
                        SizeT res_n = doc_ids.size();
                        for (SizeT i = 0; i < res_n; ++i) {
                            RowID row_id(segment_id, doc_ids[i]);
                            ResultType d = scores[i];
                            merge_heap->Search(query_id, &d, &row_id, 1);
                        }
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            bmp_);
    }

    SizeT MemUsage() const;
    SizeT DocNum() const;
    SizeT GetSizeInBytes() const;

    void Save(LocalFileHandle &file_handle) const;
    void SaveToPtr(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    void LoadFromPtr(LocalFileHandle &file_handle, SizeT file_size);
    void LoadFromPtr(const char *ptr, SizeT file_size);
    void Optimize(const BMPOptimizeOptions &options);

private:
    AbstractBMP bmp_ = nullptr;
};

export using BMPHandlerPtr = BMPHandler *;

export struct BMPIndexInMem final : public BaseMemIndex {
public:
    BMPIndexInMem() : bmp_handler_(nullptr) {}
    BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, SegmentIndexEntry *segment_index_entry)
        : begin_row_id_(begin_row_id), bmp_handler_(BMPHandler::Make(index_base, column_def).release()), segment_index_entry_(segment_index_entry) {}
    ~BMPIndexInMem();
    BMPIndexInMem(const BMPIndexInMem &) = delete;
    BMPIndexInMem &operator=(const BMPIndexInMem &) = delete;

    void AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count);

    void AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

    void AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts);

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr, SizeT *dump_size = nullptr);

    void Dump(BufferObj *buffer_obj, SizeT *dump_size = nullptr);

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

    SizeT GetRowCount() const;
    SizeT GetSizeInBytes() const;
    MemIndexTracerInfo GetInfo() const override;
    TableIndexEntry *table_index_entry() const override;
    RowID GetBeginRowID() const { return begin_row_id_; }
    const BMPHandlerPtr &get() const { return bmp_handler_; }
    BMPHandlerPtr &get_ref() { return bmp_handler_; }

private:
    RowID begin_row_id_ = {};
    BMPHandlerPtr bmp_handler_ = nullptr;
    mutable bool own_memory_ = true;
    mutable BufferHandle chunk_handle_{};
    SegmentIndexEntry *segment_index_entry_;
};

} // namespace infinity
