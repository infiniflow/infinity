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

module hnsw_file_worker;

import infinity_exception;
import stl;
import index_file_worker;
import hnsw_alg;
import hnsw_common;
import index_hnsw;

import index_base;
import third_party;
import logger;
import logical_type;
import embedding_info;
import create_index_info;
import internal_types;
import abstract_hnsw;

namespace infinity {
HnswFileWorker::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void HnswFileWorker::AllocateInMemory() {
    if (data_) {
        String error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    if (index_base_->index_type_ != IndexType::kHnsw) {
        String error_message = "Index type isn't HNSW";
        UnrecoverableError(error_message);
    }

    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        String error_message = "Index should be created on embedding column now.";
        UnrecoverableError(error_message);
    }

    SizeT dimension = GetDimension();
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_.get());
    SizeT M = index_hnsw->M_;
    SizeT ef_c = index_hnsw->ef_construction_;
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            AbstractHnsw<f32, SegmentOffset> abstract_hnsw(nullptr, index_hnsw);
            abstract_hnsw.Make(chunk_size_, max_chunk_num_, dimension, M, ef_c);
            data_ = abstract_hnsw.RawPtr();
            break;
        }
        default: {
            String error_message = "Index should be created on float embedding column now.";
            UnrecoverableError(error_message);
        }
    }
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        String error_message = "FreeInMemory: Data is not allocated.";
        UnrecoverableError(error_message);
    }
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_.get());
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            AbstractHnsw<f32, SegmentOffset> abstract_hnsw(data_, index_hnsw);
            abstract_hnsw.Free();
            break;
        }
        default: {
            String error_message = fmt::format("Index should be created on float embedding column now, type: {}",
                                               EmbeddingType::EmbeddingDataType2String(embedding_type));
            UnrecoverableError(error_message);
        }
    }
    data_ = nullptr;
}

void HnswFileWorker::CompressToLVQ(IndexHnsw *index_hnsw) {
    if (!data_) {
        String error_message = "CompressToLVQ: Data is not allocated.";
        UnrecoverableError(error_message);
    }
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            AbstractHnsw<f32, SegmentOffset> abstract_hnsw(data_, index_hnsw);
            abstract_hnsw.CompressToLVQ();
            data_ = abstract_hnsw.RawPtr();
            break;
        }
        default: {
            String error_message = "Index should be created on float embedding column now.";
            UnrecoverableError(error_message);
        }
    }
}

void HnswFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (!data_) {
        String error_message = "WriteToFileImpl: Data is not allocated.";
        UnrecoverableError(error_message);
    }
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_.get());
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            AbstractHnsw<f32, SegmentOffset> abstract_hnsw(data_, index_hnsw);
            abstract_hnsw.Save(*file_handler_);
            break;
        }
        default: {
            String error_message = "Index should be created on float embedding column now.";
            UnrecoverableError(error_message);
        }
    }
    prepare_success = true;
}

void HnswFileWorker::ReadFromFileImpl() {
    // TODO!! not save index parameter in index file.
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_.get());
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            AbstractHnsw<f32, SegmentOffset> abstract_hnsw(nullptr, index_hnsw);
            abstract_hnsw.Load(*file_handler_);
            data_ = abstract_hnsw.RawPtr();
            break;
        }
        default: {
            String error_message = "Index should be created on float embedding column now.";
            UnrecoverableError(error_message);
        }
    }
}

EmbeddingDataType HnswFileWorker::GetType() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Type();
}

SizeT HnswFileWorker::GetDimension() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Dimension();
}
} // namespace infinity