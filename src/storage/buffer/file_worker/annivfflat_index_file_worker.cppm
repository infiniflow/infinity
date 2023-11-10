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

#include <cmath>

import stl;
import index_file_worker;
import file_worker;
import parser;
import index_def;
import annivfflat_index_data;
import infinity_exception;
import ivfflat_index_def;

export module annivfflat_index_file_worker;

namespace infinity {

export struct CreateAnnIVFFlatPara : public CreateIndexPara {
    // used when ivfflat_index_def->centroids_count_ == 0
    const SizeT row_count_;

    CreateAnnIVFFlatPara(SharedPtr<IndexDef> index_def, SharedPtr<ColumnDef> column_def, SizeT row_count)
        : CreateIndexPara(index_def, column_def), row_count_(row_count) {}
};

export template <typename DataType>
class AnnIVFFlatIndexFileWorker : public IndexFileWorker {
    u32 default_centroid_num_;

public:
    explicit AnnIVFFlatIndexFileWorker(SharedPtr<String> file_dir,
                                       SharedPtr<String> file_name,
                                       SharedPtr<IndexDef> index_def,
                                       SharedPtr<ColumnDef> column_def,
                                       SizeT row_count)
        : IndexFileWorker(file_dir, file_name, index_def, column_def), default_centroid_num_((u32)sqrt(row_count)) {}

    virtual ~AnnIVFFlatIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    EmbeddingDataType GetType() const;

    SizeT GetDimension() const;
};

template <typename DataType>
AnnIVFFlatIndexFileWorker<DataType>::~AnnIVFFlatIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
    if (index_def_->method_type_ != IndexMethod::kIVFFlat) {
        Error<StorageException>("Bug.");
    }
    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        StorageException("Index should be created on embedding column now.");
    }
    SizeT dimension = GetDimension();

    auto ivfflat_index_def = static_cast<IVFFlatIndexDef *>(index_def_.get());
    auto centroids_count = ivfflat_index_def->centroids_count_;
    if (centroids_count == 0) {
        centroids_count = default_centroid_num_;
    }
    switch (GetType()) {
        case kElemFloat: {
            data_ = static_cast<void *>(new AnnIVFFlatIndexData<DataType>(ivfflat_index_def->metric_type_, dimension, centroids_count));
            break;
        }
        default: {
            Error<StorageException>("Index should be created on float embedding column now.");
        }
    }
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::FreeInMemory() {
    if (!data_) {
        Error<StorageException>("Data is not allocated.");
    }
    auto index = static_cast<AnnIVFFlatIndexData<DataType> *>(data_);
    delete index;
    data_ = nullptr;
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::WriteToFileImpl(bool &prepare_success) {
    auto *index = static_cast<AnnIVFFlatIndexData<DataType> *>(data_);
    index->SaveIndexInner(*file_handler_);
    prepare_success = true;
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::ReadFromFileImpl() {
    auto *index = static_cast<AnnIVFFlatIndexData<DataType> *>(data_);
    index->ReadIndexInner(*file_handler_);
}

template <typename DataType>
EmbeddingDataType AnnIVFFlatIndexFileWorker<DataType>::GetType() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Type();
}

template <typename DataType>
SizeT AnnIVFFlatIndexFileWorker<DataType>::GetDimension() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Dimension();
}
} // namespace infinity