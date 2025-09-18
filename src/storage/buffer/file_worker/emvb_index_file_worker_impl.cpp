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

module infinity_core:emvb_index_file_worker.impl;

import :emvb_index_file_worker;
import :index_file_worker;
import :file_worker;
import :index_base;
import :emvb_index;
import :index_emvb;
import :infinity_exception;
import :file_worker_type;
import :emvb_product_quantization;
import :status;

import third_party;

import logical_type;
import embedding_info;
import create_index_info;
import knn_expr;
import column_def;
import internal_types;

namespace infinity {

EMVBIndexFileWorker::~EMVBIndexFileWorker() {
    // if (data_ != nullptr) {
    //     FreeInMemory();
    //     data_ = nullptr;
    // }
    FreeInMemory();
}

void EMVBIndexFileWorker::AllocateInMemory() {
    // if (data_) {
    //     UnrecoverableError("Data is already allocated.");
    // }
    // if (index_base_->index_type_ != IndexType::kEMVB) {
    //     UnrecoverableError("Index type is mismatched");
    // }
    const auto &data_type = column_def_->type();
    if (data_type->type() != LogicalType::kTensor) {
        UnrecoverableError("EMVB Index should be created on Tensor column now.");
    }
    const EmbeddingInfo *column_embedding_info = GetEmbeddingInfo();
    if (column_embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        UnrecoverableError("EMVB Index should be created on Float column now.");
    }
    const auto column_embedding_dim = column_embedding_info->Dimension();
    const auto *index_emvb = static_cast<IndexEMVB *>(index_base_.get());
    const auto residual_pq_subspace_num = index_emvb->residual_pq_subspace_num_;
    const auto residual_pq_subspace_bits = index_emvb->residual_pq_subspace_bits_;
    if (column_embedding_dim % residual_pq_subspace_num != 0) {
        const auto error_message = fmt::format("The dimension of the column embedding should be divisible by residual_pq_subspace_num: {} % {} != 0",
                                               column_embedding_dim,
                                               residual_pq_subspace_num);
        RecoverableError(Status::InvalidParameter(error_message));
    }
    data_ = static_cast<void *>(new EMVBIndex(start_segment_offset_, column_embedding_dim, residual_pq_subspace_num, residual_pq_subspace_bits));
}

void EMVBIndexFileWorker::FreeInMemory() {
    // if (!data_) {
    //     UnrecoverableError("Data is not allocated.");
    // }
    auto index = static_cast<EMVBIndex *>(data_);
    delete index;
    data_ = nullptr;
}

bool EMVBIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto *index = static_cast<EMVBIndex *>(data_);
    index->SaveIndexInner(*file_handle_);
    prepare_success = true;
    return true;
}

void EMVBIndexFileWorker::ReadFromFileImpl(size_t file_size, bool from_spill) {
    if (data_) {
        UnrecoverableError("Data is already allocated.");
    }
    const auto column_embedding_dim = GetEmbeddingInfo()->Dimension();
    const auto *index_emvb = static_cast<IndexEMVB *>(index_base_.get());
    const auto residual_pq_subspace_num = index_emvb->residual_pq_subspace_num_;
    const auto residual_pq_subspace_bits = index_emvb->residual_pq_subspace_bits_;
    auto *index = new EMVBIndex(start_segment_offset_, column_embedding_dim, residual_pq_subspace_num, residual_pq_subspace_bits);
    data_ = static_cast<void *>(index);
    index->ReadIndexInner(*file_handle_);
}

const EmbeddingInfo *EMVBIndexFileWorker::GetEmbeddingInfo() const { return static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get()); }

} // namespace infinity