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

export module annivfflat_index_file_worker;

import stl;
import index_file_worker;
import file_worker;

import index_base;
import annivfflat_index_data;
import infinity_exception;
import index_ivfflat;
import logical_type;
import embedding_info;
import create_index_info;
import knn_expr;
import column_def;
import logger;

namespace infinity {

export struct CreateAnnIVFFlatParam : public CreateIndexParam {
    // used when ivfflat_index_def->centroids_count_ == 0
    const SizeT row_count_{};

    CreateAnnIVFFlatParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def, SizeT row_count)
        : CreateIndexParam(index_base, column_def), row_count_(row_count) {}
};

export template <typename DataType>
class AnnIVFFlatIndexFileWorker : public IndexFileWorker {
    u32 default_centroid_num_;

public:
    explicit AnnIVFFlatIndexFileWorker(SharedPtr<String> file_dir,
                                       SharedPtr<String> file_name,
                                       SharedPtr<IndexBase> index_base,
                                       SharedPtr<ColumnDef> column_def,
                                       SizeT row_count)
        : IndexFileWorker(std::move(file_dir), std::move(file_name), index_base, column_def), default_centroid_num_((u32)std::sqrt(row_count)) {}

    virtual ~AnnIVFFlatIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) override;

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
        String error_message = "Data is already allocated.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    if (index_base_->index_type_ != IndexType::kIVFFlat) {
        String error_message = "Index type is mismatched";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        String error_message = "Index should be created on embedding column now.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    SizeT dimension = GetDimension();

    const auto *index_ivfflat = static_cast<const IndexIVFFlat *>(index_base_.get());
    auto centroids_count = index_ivfflat->centroids_count_;
    if (centroids_count == 0) {
        centroids_count = default_centroid_num_;
    }
    switch (GetType()) {
        case kElemFloat: {
            data_ = static_cast<void *>(new AnnIVFFlatIndexData<DataType>(index_ivfflat->metric_type_, dimension, centroids_count));
            break;
        }
        default: {
            String error_message = "Index should be created on float embedding column now.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::FreeInMemory() {
    if (!data_) {
        String error_message = "Data is not allocated.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    auto index = static_cast<AnnIVFFlatIndexData<DataType> *>(data_);
    delete index;
    data_ = nullptr;
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    auto *index = static_cast<AnnIVFFlatIndexData<DataType> *>(data_);
    index->SaveIndexInner(*file_handler_);
    prepare_success = true;
}

template <typename DataType>
void AnnIVFFlatIndexFileWorker<DataType>::ReadFromFileImpl() {
    data_ = new AnnIVFFlatIndexData<DataType>();
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