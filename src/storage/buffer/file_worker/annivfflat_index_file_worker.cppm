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

import stl;
import file_worker;
import parser;
import index_def;
import annivfflat_index_data;
import infinity_exception;
import ivfflat_index_def;

export module annivfflat_index_file_worker;

namespace infinity {

export template <typename DataType>
class AnnIVFFlatIndexFileWorker : public FileWorker {
    SharedPtr<ColumnDef> column_def_;
    SharedPtr<IndexDef> index_def_;

public:
    explicit AnnIVFFlatIndexFileWorker(SharedPtr<String> file_dir,
                                       SharedPtr<String> file_name,
                                       SharedPtr<IndexDef> index_def,
                                       SharedPtr<ColumnDef> column_def)
        : FileWorker(Move(file_dir), Move(file_name), 0), index_def_(Move(index_def)), column_def_(Move(column_def)) {}

    virtual ~AnnIVFFlatIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;
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
    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        StorageException("Index should be created on embedding column now.");
    }
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    SizeT dimension = embedding_info->Dimension();

    if (index_def_->method_type_ == IndexMethod::kIVFFlat) {
        auto ivfflat_index_def = dynamic_cast<IVFFlatIndexDef *>(index_def_.get());
        auto index = new AnnIVFFlatIndexData<DataType>(ivfflat_index_def->metric_type_, dimension, ivfflat_index_def->centroids_count_);
        data_ = static_cast<void *>(index);
    } else {
        Error<StorageException>("Not implemented.");
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

} // namespace infinity