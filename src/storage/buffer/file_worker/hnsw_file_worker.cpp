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

import infinity_exception;
import stl;
import index_file_worker;
import dist_func;
import hnsw_alg;
import index_hnsw;
import parser;
import index_base;
import dist_func_l2;
import dist_func_ip;

import plain_store;

module hnsw_file_worker;

namespace infinity {
HnswFileWorker::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void HnswFileWorker::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
    if (index_base_->index_type_ != IndexType::kHnsw) {
        Error<StorageException>("Bug.");
    }

    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        StorageException("Index should be created on embedding column now.");
    }

    SizeT dimension = GetDimension();
    const IndexHnsw* index_hnsw = static_cast<const IndexHnsw*>(index_base_);
    switch (GetType()) {
        case kElemFloat: {
            // auto dist_func = GetDistFunc<f32>(dimension);
            // data_ = static_cast<void *>(new KnnHnsw<f32, u64>(max_element_,
            //                                                   dimension,
            //                                                   *dist_func,
            //                                                   hnsw_index_def->M_,
            //                                                   hnsw_index_def->ef_construction_,
            //                                                   hnsw_index_def->ef_));
            break;
        }
        default: {
            Error<StorageException>("Index should be created on float embedding column now.");
        }
    }
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        Error<StorageException>("Data is not allocated.");
    }
    switch (GetType()) {
        case kElemFloat: {
            // auto hnsw_index_ptr = static_cast<KnnHnsw<f32, u64, PlainStore<f32>> *>(data_);
            // delete hnsw_index_ptr;
            break;
        }
        default: {
            Error<StorageException>("Index should be created on float embedding column now.");
        }
    }
    data_ = nullptr;
}

void HnswFileWorker::WriteToFileImpl(bool &prepare_success) {
    switch (GetType()) {
        case kElemFloat: {
            // auto *hnsw_index_ptr = static_cast<KnnHnsw<f32, u64, PlainStore<f32>> *>(data_);
            // hnsw_index_ptr->SaveIndex(*file_handler_);
            break;
        }
        default: {
            Error<StorageException>("Index should be created on float embedding column now.");
        }
    }
    prepare_success = true;
}

void HnswFileWorker::ReadFromFileImpl() {
    SizeT dimension = GetDimension();
    switch (GetType()) {
        case kElemFloat: {
            // auto dist_func = GetDistFunc<f32>(dimension);
            // UniquePtr<KnnHnsw<f32, u64>> hnsw_index = KnnHnsw<f32, u64>::LoadIndexInner(*file_handler_, *dist_func);
            // TODO!! not save index parameter in index file.
            // data_ = static_cast<void *>(hnsw_index.release());
            break;
        }
        default: {
            Error<StorageException>("Index should be created on float embedding column now.");
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

// template <typename DataType>
// UniquePtr<SpaceBase<DataType>> HnswFileWorker::GetDistFunc(SizeT dimension) const {
//     UniquePtr<SpaceBase<f32>> space = nullptr;
//     const auto* hnsw_index_def = static_cast<const IndexHnsw *>(index_base_);
//     switch (hnsw_index_def->metric_type_) {
//         case MetricType::kMerticInnerProduct: {
//             space = MakeUnique<DistFuncIP<f32>>(dimension);
//             break;
//         }
//         case MetricType::kMerticL2: {
//             space = MakeUnique<DistFuncL2<f32>>(dimension);
//             break;
//         }
//         case MetricType::kInvalid: {
//             StorageException("Bug.");
//         }
//     }
//     return space;
// }
} // namespace infinity