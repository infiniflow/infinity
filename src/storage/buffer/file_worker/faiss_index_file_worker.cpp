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
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/impl/FaissException.h"
#include "faiss/impl/io.h"
#include "faiss/index_io.h"

import stl;
import infinity_exception;
import parser;
import ivfflat_def;
import base_index;
import file_system;
import third_party;

module faiss_index_file_worker;

namespace infinity {

FaissIndexFileWorker::~FaissIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

struct FSIOReader : faiss::IOReader {
    FSIOReader(FileHandler *file_handler) : file_handler_(file_handler) {}
    size_t operator()(void *ptr, size_t size, size_t nitems) override {
        file_handler_->Read(ptr, size * nitems);
        return nitems;
    }
    FileHandler *file_handler_;
};

struct FSIOWriter : faiss::IOWriter {
    FSIOWriter(FileHandler *file_handler) : file_handler_(file_handler) {}
    size_t operator()(const void *ptr, size_t size, size_t nitems) override {
        file_handler_->Write(ptr, size * nitems);
        return nitems;
    }
    FileHandler *file_handler_;
};

void FaissIndexFileWorker::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        StorageException("Index should be created on embedding column now.");
    }
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    if (embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        Error<StorageException>("Index should be created on float embedding column now.");
    }
    SizeT dimension = embedding_info->Dimension();

    if (index_def_->index_type_ != IndexType::kIVFFlat) {
        Error<StorageException>("Not implemented.");
    }
    auto ivfflat_index_def = static_cast<IVFFlatDef *>(index_def_);
    faiss::IndexFlat *quantizer = nullptr;
    faiss::MetricType metric = faiss::MetricType::METRIC_L2;

    switch (ivfflat_index_def->metric_type_) {
        case MetricType::kMerticL2: {
            quantizer = new faiss::IndexFlatL2(dimension);
            metric = faiss::MetricType::METRIC_L2;
            break;
        }
        case MetricType::kMerticInnerProduct: {
            quantizer = new faiss::IndexFlatIP(dimension);
            metric = faiss::MetricType::METRIC_INNER_PRODUCT;
            break;
        }
        case MetricType::kInvalid: {
            Error<StorageException>("Metric type is invalid");
        }
        default: {
            Error<NotImplementException>("Not implemented");
        }
    }
    faiss::Index *index = new faiss::IndexIVFFlat(quantizer, dimension, ivfflat_index_def->centroids_count_, metric);
    auto faiss_index_ptr = new FaissIndexPtr(index, quantizer);
    data_ = static_cast<void *>(faiss_index_ptr);
}

void FaissIndexFileWorker::FreeInMemory() {
    if (!data_) {
        Error<StorageException>("Data is not allocated.");
    }
    auto faiss_index_ptr = static_cast<FaissIndexPtr *>(data_);
    delete faiss_index_ptr->index_;
    delete faiss_index_ptr->quantizer_;
    delete faiss_index_ptr;
    data_ = nullptr;
}

void FaissIndexFileWorker::WriteToFileImpl(bool &prepare_success) {
    try {
        auto faiss_index_ptr = static_cast<FaissIndexPtr *>(data_);
        FSIOWriter writer(file_handler_.get());
        faiss::write_index(faiss_index_ptr->index_, &writer);
        prepare_success = true; // Not run defer_fn
    } catch (faiss::FaissException &xcp) {
        Error<StorageException>(Format("Faiss exception: {}", xcp.what()));
    }
}

void FaissIndexFileWorker::ReadFromFileImpl() {
    FSIOReader reader(file_handler_.get());
    auto index = faiss::read_index(&reader);
    data_ = static_cast<void *>(new FaissIndexPtr(index, nullptr));
}
} // namespace infinity