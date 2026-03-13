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
import :logger;

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
    // For mmap-loaded index, object is in mmap_data_
    // For regular-loaded index, object is in data_
    if (mmap_data_) {
        auto *index = reinterpret_cast<PlaidIndex *>(mmap_data_);
        delete index;
        mmap_data_ = nullptr;
    } else if (data_) {
        auto *index = static_cast<PlaidIndex *>(data_);
        delete index;
        data_ = nullptr;
    } else {
        UnrecoverableError("PlaidIndexFileWorker::FreeInMemory: Data is not allocated.");
    }
}

bool PlaidIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    // For mmap-loaded index, object is in mmap_data_
    // For regular-loaded index, object is in data_
    PlaidIndex *index = nullptr;
    if (mmap_data_) {
        index = reinterpret_cast<PlaidIndex *>(mmap_data_);
    } else if (data_) {
        index = static_cast<PlaidIndex *>(data_);
    } else {
        UnrecoverableError("PlaidIndexFileWorker::WriteToFileImpl: No index data available.");
    }
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

bool PlaidIndexFileWorker::ReadFromMmapImpl(const void *ptr, size_t size) {
    LOG_INFO(fmt::format("PlaidIndexFileWorker::ReadFromMmapImpl: ptr={}, size={}, mmap_data_={}", ptr, size, static_cast<const void *>(mmap_data_)));
    if (mmap_data_ != nullptr) {
        UnrecoverableError("PlaidIndexFileWorker::ReadFromMmapImpl: Mmap data is already allocated.");
    }
    const auto column_embedding_dim = GetEmbeddingInfo()->Dimension();

    // Read header from the mmap data (first 4 fields: start_segment_offset, embedding_dimension, nbits, n_centroids)
    const char *src = static_cast<const char *>(ptr);
    u32 stored_start_segment_offset, stored_embedding_dimension, stored_nbits, stored_n_centroids;
    size_t offset = 0;
    std::memcpy(&stored_start_segment_offset, src + offset, sizeof(stored_start_segment_offset));
    offset += sizeof(stored_start_segment_offset);
    std::memcpy(&stored_embedding_dimension, src + offset, sizeof(stored_embedding_dimension));
    offset += sizeof(stored_embedding_dimension);
    std::memcpy(&stored_nbits, src + offset, sizeof(stored_nbits));
    offset += sizeof(stored_nbits);
    std::memcpy(&stored_n_centroids, src + offset, sizeof(stored_n_centroids));

    // Verify embedding dimension matches
    if (stored_embedding_dimension != column_embedding_dim) {
        UnrecoverableError(fmt::format("PlaidIndexFileWorker::ReadFromMmapImpl: embedding dimension mismatch: stored={}, expected={}",
                                       stored_embedding_dimension,
                                       column_embedding_dim));
    }

    // Use nbits and n_centroids from file, not from index_base_
    const auto nbits = stored_nbits;
    const auto n_centroids = stored_n_centroids;

    LOG_INFO(fmt::format(
        "PlaidIndexFileWorker::ReadFromMmapImpl: stored_start_segment_offset={}, stored_embedding_dim={}, stored_nbits={}, stored_n_centroids={}",
        stored_start_segment_offset,
        stored_embedding_dimension,
        stored_nbits,
        stored_n_centroids));

    // Allocate PlaidIndex object with correct nbits from file
    auto *index = new PlaidIndex(start_segment_offset_, column_embedding_dim, nbits, n_centroids);

    LOG_INFO(fmt::format("PlaidIndexFileWorker::ReadFromMmapImpl: Created PlaidIndex with nbits={}, n_centroids={}", nbits, n_centroids));

    // Load from mmap pointer - this will copy data into internal vectors
    // Note: For true zero-copy mmap, PlaidIndex would need to use pointers into mmap region
    // instead of owning vectors. This is a future optimization.
    index->LoadFromPtr(const_cast<void *>(ptr), size, size);

    // Store PlaidIndex object in mmap_data_ so GetMmapData() returns it
    mmap_data_ = reinterpret_cast<u8 *>(index);

    LOG_INFO(fmt::format("PlaidIndexFileWorker::ReadFromMmapImpl: Loaded index with {} docs, {} embeddings, {} bytes, mmap_data_={}",
                         index->GetDocNum(),
                         index->GetTotalEmbeddingNum(),
                         size,
                         static_cast<const void *>(mmap_data_)));
    return true;
}

void PlaidIndexFileWorker::FreeFromMmapImpl() {
    if (mmap_data_ == nullptr) {
        return;
    }
    auto *index = reinterpret_cast<PlaidIndex *>(mmap_data_);
    delete index;
    mmap_data_ = nullptr;
}

size_t PlaidIndexFileWorker::GetMemoryCost() const {
    // Return the actual memory cost of the loaded PlaidIndex
    if (data_) {
        auto *index = static_cast<PlaidIndex *>(data_);
        return index->MemUsage();
    } else if (mmap_data_) {
        auto *index = reinterpret_cast<PlaidIndex *>(mmap_data_);
        return index->MemUsage();
    }
    return 0;
}

} // namespace infinity
