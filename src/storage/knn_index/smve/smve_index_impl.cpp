// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:smve_index.impl;

import :smve_index;
import :smve_transform;
import :bmp_handler;
import :bmp_alg;
import :bmp_util;
import :index_base;
import :index_smve;
import :default_values;
import :infinity_exception;
import :logger;
import :mlas_matrix_multiply;
import :new_catalog;
import :buffer_obj;
import :buffer_handle;
import :column_vector;
import :vector_buffer;
import :local_file_handle;
import :merge_knn;
import :emvb_result_handler;

import std;
import embedding_info;
import column_def;
import logical_type;
import internal_types;
import sparse_info;
import third_party;

namespace infinity {

SMVEIndexInMem::SMVEIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
    : begin_row_id_(begin_row_id) {
    auto *smve_def = static_cast<const IndexSMVE *>(index_base);
    width_ = smve_def->width_;
    topk_ = smve_def->topk_;

    auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());
    embedding_dim_ = embedding_info->Dimension();

    projection_matrix_ = GenerateProjectionMatrix(embedding_dim_, width_);
    bmp_handler_ = BMPHandler::Make(index_base, column_def, true).release();
    if (!bmp_handler_) {
        UnrecoverableError("SMVEIndexInMem: Failed to create BMPHandler");
    }
}

SMVEIndexInMem::~SMVEIndexInMem() {
    if (!own_memory_) return;
    if (bmp_handler_) {
        size_t mem_used = bmp_handler_->MemUsage();
        delete bmp_handler_;
        DecreaseMemoryUsageBase(mem_used);
        bmp_handler_ = nullptr;
    }
}

void SMVEIndexInMem::BuildFromColumn(const ColumnVector &col, SegmentOffset block_offset, BlockOffset offset, BlockOffset row_count) {
    auto sparse_type = std::make_shared<DataType>(LogicalType::kSparse,
                                                   std::make_shared<SparseInfo>(EmbeddingDataType::kElemFloat, EmbeddingDataType::kElemInt32, width_, SparseStoreType::kSorted));
    auto sparse_col = ColumnVector::Make(sparse_type);
    sparse_col->Initialize(ColumnVectorType::kFlat, row_count);

    for (BlockOffset i = 0; i < row_count; ++i) {
        auto [tensor_span, tensor_size] = col.GetTensorRaw(offset + i);
        const f32 *tensor_ptr = reinterpret_cast<const f32 *>(tensor_span.data());
        u32 n_tokens = static_cast<u32>(tensor_size) / (embedding_dim_ * sizeof(f32));

        SMVEResult smve = SMVETransform(tensor_ptr, n_tokens, embedding_dim_, projection_matrix_.get(), width_, topk_, false);

        if (smve.indices.empty()) {
            sparse_col->AppendSparse<float, i32>(0, nullptr, nullptr);
        } else {
            sparse_col->AppendSparse<float, i32>(static_cast<i32>(smve.values.size()), smve.values.data(),
                                                  reinterpret_cast<const i32 *>(smve.indices.data()));
        }
    }
    size_t mem_delta = bmp_handler_->AddDocs(block_offset, *sparse_col, 0, static_cast<BlockOffset>(row_count));
    IncreaseMemoryUsageBase(mem_delta);
    n_docs_ += row_count;
    LOG_TRACE(fmt::format("SMVEIndexInMem::BuildFromColumn: Added {} docs (total {}), width={}, topk={}", row_count, n_docs_, width_, topk_));
}

void SMVEIndexInMem::Dump(BufferObj *buffer_obj, size_t *dump_size_ptr) {
    if (!own_memory_) {
        UnrecoverableError("SMVEIndexInMem::Dump() called with own_memory_ = false.");
    }

    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<SMVESerializedData *>(handle.GetDataMut());
    data_ptr->width = width_;
    data_ptr->topk = topk_;
    data_ptr->embedding_dim = embedding_dim_;
    data_ptr->n_docs = n_docs_;

    // Copy projection matrix into serialized data
    size_t proj_size = embedding_dim_ * width_;
    if (proj_size > 0 && projection_matrix_) {
        data_ptr->projection_matrix_copy = std::make_unique<f32[]>(proj_size);
        std::copy_n(projection_matrix_.get(), proj_size, data_ptr->projection_matrix_copy.get());
    }

    // Transfer BMP handler ownership
    data_ptr->bmp_handler = bmp_handler_;
    bmp_handler_ = nullptr;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);

    if (dump_size_ptr) {
        *dump_size_ptr = GetSizeInBytes();
    }
    LOG_TRACE(fmt::format("SMVEIndexInMem::Dump: Dumped {} docs, width={}, topk={}", n_docs_, width_, topk_));
}

std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>>
SMVEIndexInMem::Search(const f32 *query_ptr,
                       u32 n_query_tokens,
                       u32 top_k,
                       u32 overfetch,
                       const BlockIndex *block_index,
                       TxnTimeStamp begin_ts) const {
    // Step 1: SMVE transform query
    SMVEResult smve_query = SMVETransform(query_ptr, n_query_tokens, embedding_dim_, projection_matrix_.get(), width_, topk_, true);

    if (smve_query.indices.empty() || !bmp_handler_) {
        return std::make_tuple(0u, nullptr, nullptr);
    }

    // Step 2: Build SparseVecRef from SMVE result for BMP search
    i32 candidate_topk = static_cast<i32>(top_k * overfetch);
    SparseVecRef<float, i32> query_ref(
        static_cast<i32>(smve_query.values.size()),
        reinterpret_cast<const i32 *>(smve_query.indices.data()),
        smve_query.values.data()
    );

    // Step 3: BMP search
    BmpSearchOptions options;
    options.alpha_ = 1.0f;
    options.beta_ = 1.0f;
    options.use_tail_ = true;
    options.use_lock_ = false;

    SMVEBMPCollector collector;
    auto accept_all = [](u32) { return true; };
    struct SMVEBMPDist {
        using DataT = float;
        using IndexT = i32;
    };
    bmp_handler_->SearchIndex<float, SMVEBMPDist>(
        query_ref, candidate_topk, options, accept_all, 0, 0, &collector
    );

    if (collector.results.empty()) {
        return std::make_tuple(0u, nullptr, nullptr);
    }

    // Step 4: Sort by score descending, take top_k
    std::partial_sort(collector.results.begin(),
                      collector.results.begin() + std::min<size_t>(top_k, collector.results.size()),
                      collector.results.end(),
                      [](const auto &a, const auto &b) { return a.first > b.first; });

    u32 n_results = std::min<u32>(top_k, static_cast<u32>(collector.results.size()));
    auto scores = std::make_unique<f32[]>(n_results);
    auto row_ids = std::make_unique<u32[]>(n_results);
    for (u32 i = 0; i < n_results; ++i) {
        scores[i] = collector.results[i].first;
        row_ids[i] = collector.results[i].second;
    }

    LOG_TRACE(fmt::format("SMVEIndexInMem::Search: candidates={}, top_k={}, results={}",
                          collector.results.size(), top_k, n_results));

    return std::make_tuple(n_results, std::move(scores), std::move(row_ids));
}

const ChunkIndexMetaInfo SMVEIndexInMem::GetChunkIndexMetaInfo() const {
    return ChunkIndexMetaInfo{"", begin_row_id_, n_docs_, 0, 0};
}

size_t SMVEIndexInMem::GetSizeInBytes() const {
    size_t size = sizeof(SMVEIndexInMem);
    size += width_ * embedding_dim_ * sizeof(f32);
    if (bmp_handler_) {
        size += bmp_handler_->GetSizeInBytes();
    }
    return size;
}

MemIndexTracerInfo SMVEIndexInMem::GetInfo() const {
    return MemIndexTracerInfo(std::make_shared<std::string>("SMVE"),
                              std::make_shared<std::string>(""),
                              std::make_shared<std::string>(""),
                              GetSizeInBytes(),
                              n_docs_);
}

} // namespace infinity
