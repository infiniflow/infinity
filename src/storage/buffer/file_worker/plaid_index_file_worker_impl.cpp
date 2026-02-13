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

module infinity_core:plaid_index_file_worker.impl;

import :plaid_index_file_worker;
import :index_file_worker;
import :file_worker;
import :index_base;
import :plaid_index;
import :index_plaid;
import :infinity_exception;
import :file_worker_type;
import :status;
import :local_file_handle;

import third_party;

import logical_type;
import embedding_info;
import create_index_info;
import knn_expr;
import column_def;
import internal_types;

namespace infinity {

void PlaidIndexFileWorker::AllocateInMemory() {
    if (data_) {
        UnrecoverableError("PlaidIndexFileWorker::AllocateInMemory: Data is already allocated.");
    }
    if (index_base_->index_type_ != IndexType::kPLAID) {
        UnrecoverableError("Index type is mismatched");
    }
    const auto &data_type = column_def_->type();
    if (data_type->type() != LogicalType::kTensor) {
        UnrecoverableError("PLAID Index should be created on Tensor column now.");
    }
    const EmbeddingInfo *column_embedding_info = GetEmbeddingInfo();
    if (column_embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        UnrecoverableError("PLAID Index should be created on Float column now.");
    }
    const auto column_embedding_dim = column_embedding_info->Dimension();
    const auto *index_plaid = static_cast<IndexPLAID *>(index_base_.get());
    const auto nbits = index_plaid->nbits_;
    const auto n_centroids = index_plaid->n_centroids_;
    // Allocate PlaidIndex object directly (like EMVB)
    data_ = static_cast<void *>(new PlaidIndex(start_segment_offset_, column_embedding_dim, nbits, n_centroids));
}

void PlaidIndexFileWorker::FreeInMemory() {
    if (!data_) {
        UnrecoverableError("PlaidIndexFileWorker::FreeInMemory: Data is not allocated.");
    }
    auto *index = static_cast<PlaidIndex *>(data_);
    delete index;
    data_ = nullptr;
}

bool PlaidIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    auto *index = static_cast<PlaidIndex *>(data_);
    index->SaveIndexInner(*file_handle_);
    prepare_success = true;
    return true;
}

void PlaidIndexFileWorker::ReadFromFileImpl(size_t file_size, bool from_spill) {
    if (data_) {
        UnrecoverableError("PlaidIndexFileWorker::ReadFromFileImpl: Data is already allocated.");
    }
    const auto column_embedding_dim = GetEmbeddingInfo()->Dimension();
    const auto *index_plaid = static_cast<IndexPLAID *>(index_base_.get());
    const auto nbits = index_plaid->nbits_;
    const auto n_centroids = index_plaid->n_centroids_;
    auto *index = new PlaidIndex(start_segment_offset_, column_embedding_dim, nbits, n_centroids);
    data_ = static_cast<void *>(index);
    index->ReadIndexInner(*file_handle_);
}

const EmbeddingInfo *PlaidIndexFileWorker::GetEmbeddingInfo() const { return static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get()); }

} // namespace infinity
