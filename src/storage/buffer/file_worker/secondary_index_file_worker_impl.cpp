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

module infinity_core:secondary_index_file_worker.impl;

import :secondary_index_file_worker;
import :index_file_worker;
import :file_worker;
import :logger;
import :index_base;
import :index_secondary;
import :index_secondary_functional;
import :secondary_index_data;
import :infinity_exception;
import :persistence_manager;

import third_party;
import create_index_info;

namespace infinity {

SecondaryIndexFileWorker::~SecondaryIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void SecondaryIndexFileWorker::AllocateInMemory() {
    if (data_) [[unlikely]] {
        UnrecoverableError("AllocateInMemory: Already allocated.");
    } else if (auto &data_type = column_def_->type(); data_type->CanBuildSecondaryIndex()) [[likely]] {
        // Determine cardinality and use appropriate function
        // For secondary indexes, cast to IndexSecondary to get cardinality
        SecondaryIndexCardinality cardinality = SecondaryIndexCardinality::kHighCardinality;
        if (index_base_->index_type_ == IndexType::kSecondary) {
            auto secondary_index = std::static_pointer_cast<IndexSecondary>(index_base_);
            cardinality = secondary_index->GetSecondaryIndexCardinality();
        }

        // Use the correct factory function based on cardinality
        if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
            data_ = static_cast<void *>(GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, row_count_, true));
        } else {
            data_ = static_cast<void *>(GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, row_count_, true));
        }
        LOG_TRACE("Finished AllocateInMemory().");
    } else {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
    }
}

void SecondaryIndexFileWorker::FreeInMemory() {
    if (data_) [[likely]] {
        // Determine cardinality and delete the correct type
        SecondaryIndexCardinality cardinality = SecondaryIndexCardinality::kHighCardinality;
        if (index_base_->index_type_ == IndexType::kSecondary) {
            auto secondary_index = std::static_pointer_cast<IndexSecondary>(index_base_);
            cardinality = secondary_index->GetSecondaryIndexCardinality();
        }

        if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
            auto index = static_cast<SecondaryIndexDataBase<HighCardinalityTag> *>(data_);
            delete index;
        } else {
            auto index = static_cast<SecondaryIndexDataBase<LowCardinalityTag> *>(data_);
            delete index;
        }
        data_ = nullptr;
        LOG_TRACE("Finished SecondaryIndexFileWorker::FreeInMemory(), deleted data_ ptr.");
    } else {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
}

bool SecondaryIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
    if (data_) [[likely]] {
        // Determine cardinality and use the correct type
        SecondaryIndexCardinality cardinality = SecondaryIndexCardinality::kHighCardinality;
        if (index_base_->index_type_ == IndexType::kSecondary) {
            auto secondary_index = std::static_pointer_cast<IndexSecondary>(index_base_);
            cardinality = secondary_index->GetSecondaryIndexCardinality();
        }

        if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
            auto index = static_cast<SecondaryIndexDataBase<HighCardinalityTag> *>(data_);
            index->SaveIndexInner(*file_handle_);
        } else {
            auto index = static_cast<SecondaryIndexDataBase<LowCardinalityTag> *>(data_);
            index->SaveIndexInner(*file_handle_);
        }
        prepare_success = true;
        LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
    } else {
        UnrecoverableError("WriteToFileImpl: data_ is nullptr");
    }
    return true;
}

void SecondaryIndexFileWorker::ReadFromFileImpl(size_t file_size, bool from_spill) {
    if (!data_) [[likely]] {
        // Determine cardinality and use appropriate function
        // For secondary indexes, cast to IndexSecondary to get cardinality
        SecondaryIndexCardinality cardinality = SecondaryIndexCardinality::kHighCardinality;
        if (index_base_->index_type_ == IndexType::kSecondary) {
            auto secondary_index = std::static_pointer_cast<IndexSecondary>(index_base_);
            cardinality = secondary_index->GetSecondaryIndexCardinality();
        }

        if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
            // auto index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(column_def_->type(), row_count_, false);
            auto index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(std::make_shared<DataType>(index_data_type_), row_count_, false);
            index->ReadIndexInner(*file_handle_);
            data_ = static_cast<void *>(index);
        } else {
            // auto index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(column_def_->type(), row_count_, false);
            auto index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(std::make_shared<DataType>(index_data_type_), row_count_, false);
            index->ReadIndexInner(*file_handle_);
            data_ = static_cast<void *>(index);
        }

        // auto index = GetSecondaryIndexData(std::make_shared<DataType>(index_data_type_), row_count_, false);
        LOG_TRACE("Finished ReadFromFileImpl().");
    } else {
        UnrecoverableError("ReadFromFileImpl: data_ is not nullptr");
    }
}

} // namespace infinity
