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

// namespace infinity {
// struct ChunkIndexMetaInfo;
// }

export module infinity_core:bmp_handler;

import :stl;
import :bmp_alg;
import :bmp_util;
import :index_base;
import column_def;
import internal_types;
import :index_bmp;
import sparse_info;
import internal_types;
import :buffer_handle;
import :base_memindex;
import :memindex_tracer;
import :sparse_util;
import :infinity_exception;
import :abstract_bmp;
import :column_vector;
import data_type;

namespace infinity {

struct ChunkIndexMetaInfo;
class BufferManager;
// class ColumnVector;
class BufferObj;
class LocalFileHandle;

export using AbstractBMP = std::variant<UniquePtr<BMPAlg<f32, i32, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f32, i32, BMPCompressType::kRaw>>,
                                 UniquePtr<BMPAlg<f32, i16, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f32, i16, BMPCompressType::kRaw>>,
                                 UniquePtr<BMPAlg<f32, i8, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f32, i8, BMPCompressType::kRaw>>,
                                 UniquePtr<BMPAlg<f64, i32, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f64, i32, BMPCompressType::kRaw>>,
                                 UniquePtr<BMPAlg<f64, i16, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f64, i16, BMPCompressType::kRaw>>,
                                 UniquePtr<BMPAlg<f64, i8, BMPCompressType::kCompressed>>,
                                 UniquePtr<BMPAlg<f64, i8, BMPCompressType::kRaw>>,

                                 UniquePtr<BMPAlg<f32, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f32, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f32, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f32, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f32, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f32, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 UniquePtr<BMPAlg<f64, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,

                                 std::nullptr_t>;

export class BMPHandler {
public:
    BMPHandler() : bmp_(nullptr) {}
    ~BMPHandler() {}

    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    BMPHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    static UniquePtr<BMPHandler> Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    SizeT AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

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
    BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
        : begin_row_id_(begin_row_id), bmp_handler_(BMPHandler::Make(index_base, column_def).release()) {}
    ~BMPIndexInMem();
    BMPIndexInMem(const BMPIndexInMem &) = delete;
    BMPIndexInMem &operator=(const BMPIndexInMem &) = delete;

    void AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

    void Dump(BufferObj *buffer_obj, SizeT *dump_size = nullptr);

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

    SizeT GetRowCount() const;
    SizeT GetSizeInBytes() const;
    MemIndexTracerInfo GetInfo() const override;
    RowID GetBeginRowID() const override { return begin_row_id_; }
    const BMPHandlerPtr &get() const { return bmp_handler_; }
    BMPHandlerPtr &get_ref() { return bmp_handler_; }

private:
    RowID begin_row_id_ = {};
    BMPHandlerPtr bmp_handler_ = nullptr;
    mutable bool own_memory_ = true;
    mutable BufferHandle chunk_handle_{};
};

} // namespace infinity
