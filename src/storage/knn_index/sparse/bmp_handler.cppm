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

export module infinity_core:bmp_handler;

import :bmp_alg;
import :bmp_util;
import :index_base;
import :index_bmp;
import :base_memindex;
import :memindex_tracer;
import :sparse_util;
import :infinity_exception;
import :column_vector;

import data_type;
import column_def;
import internal_types;
import sparse_info;
import internal_types;

namespace infinity {

struct ChunkIndexMetaInfo;
class LocalFileHandle;

using AbstractBMP = std::variant<std::unique_ptr<BMPAlg<f32, i32, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f32, i32, BMPCompressType::kRaw>>,
                                 std::unique_ptr<BMPAlg<f32, i16, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f32, i16, BMPCompressType::kRaw>>,
                                 std::unique_ptr<BMPAlg<f32, i8, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f32, i8, BMPCompressType::kRaw>>,
                                 std::unique_ptr<BMPAlg<f64, i32, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f64, i32, BMPCompressType::kRaw>>,
                                 std::unique_ptr<BMPAlg<f64, i16, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f64, i16, BMPCompressType::kRaw>>,
                                 std::unique_ptr<BMPAlg<f64, i8, BMPCompressType::kCompressed>>,
                                 std::unique_ptr<BMPAlg<f64, i8, BMPCompressType::kRaw>>,

                                 std::unique_ptr<BMPAlg<f32, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f32, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f32, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f32, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f32, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f32, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i32, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i32, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i16, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i16, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i8, BMPCompressType::kCompressed, BMPOwnMem::kFalse>>,
                                 std::unique_ptr<BMPAlg<f64, i8, BMPCompressType::kRaw, BMPOwnMem::kFalse>>,

                                 std::nullptr_t>;

export class BMPHandler {
public:
    BMPHandler() : bmp_(nullptr) {}
    ~BMPHandler() {}

    static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    BMPHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    static std::unique_ptr<BMPHandler> Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    size_t AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count);

    template <typename ResultType, typename DistFunc, typename Filter = std::nullopt_t, typename MergeHeap = std::nullopt_t>
    void SearchIndex(const auto &query,
                     i32 topn,
                     const BmpSearchOptions &options,
                     const Filter &filter,
                     size_t query_id,
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
                        size_t res_n = doc_ids.size();
                        for (size_t i = 0; i < res_n; ++i) {
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

    size_t MemUsage() const;
    size_t DocNum() const;
    size_t GetSizeInBytes() const;

    void Save(LocalFileHandle &file_handle) const;
    void SaveToPtr(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    void LoadFromPtr(LocalFileHandle &file_handle, size_t file_size);
    void LoadFromPtr(const char *ptr, size_t file_size);
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

    void Dump(FileWorker *index_file_worker, size_t *dump_size = nullptr);

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

    size_t GetRowCount() const;
    size_t GetSizeInBytes() const;
    MemIndexTracerInfo GetInfo() const override;
    RowID GetBeginRowID() const override { return begin_row_id_; }
    const BMPHandlerPtr &get() const { return bmp_handler_; }
    BMPHandlerPtr &get_ref() { return bmp_handler_; }

private:
    RowID begin_row_id_ = {};
    BMPHandlerPtr bmp_handler_ = nullptr;
    mutable bool own_memory_ = true;
    mutable FileWorker *index_file_worker_{};
};

} // namespace infinity
