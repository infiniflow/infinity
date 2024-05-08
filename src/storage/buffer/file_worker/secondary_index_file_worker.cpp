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

module secondary_index_file_worker;

import stl;
import index_file_worker;
import file_worker;

import logger;
import index_base;
import index_secondary;
import secondary_index_data;
import infinity_exception;
import third_party;
import file_system;

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
        data_ = static_cast<void *>(GetSecondaryIndexData(data_type, row_count_, true));
        LOG_TRACE("Finished AllocateInMemory().");
    } else {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
    }
}

void SecondaryIndexFileWorker::FreeInMemory() {
    if (data_) [[likely]] {
        auto index = static_cast<SecondaryIndexData *>(data_);
        delete index;
        data_ = nullptr;
        LOG_TRACE("Finished FreeInMemory(), deleted data_ ptr.");
    } else {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
}

void SecondaryIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (data_) [[likely]] {
        auto index = static_cast<SecondaryIndexData *>(data_);
        index->SaveIndexInner(*file_handler_);
        prepare_success = true;
        LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
    } else {
        UnrecoverableError("WriteToFileImpl: data_ is nullptr");
    }
}

void SecondaryIndexFileWorker::ReadFromFileImpl() {
    if (!data_) [[likely]] {
        auto index = GetSecondaryIndexData(column_def_->type(), row_count_, false);
        index->ReadIndexInner(*file_handler_);
        data_ = static_cast<void *>(index);
        LOG_TRACE("Finished ReadFromFileImpl().");
    } else {
        UnrecoverableError("ReadFromFileImpl: data_ is not nullptr");
    }
}

SecondaryIndexFileWorkerParts::SecondaryIndexFileWorkerParts(SharedPtr<String> file_dir,
                                                             SharedPtr<String> file_name,
                                                             SharedPtr<IndexBase> index_base,
                                                             SharedPtr<ColumnDef> column_def,
                                                             u32 row_count,
                                                             u32 part_id)
    : IndexFileWorker(file_dir, file_name, index_base, column_def), row_count_(row_count), part_id_(part_id) {
    data_pair_size_ = GetSecondaryIndexDataPairSize(column_def_->type());
}

SecondaryIndexFileWorkerParts::~SecondaryIndexFileWorkerParts() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void SecondaryIndexFileWorkerParts::AllocateInMemory() {
    if (row_count_ < part_id_ * 8192) {
        UnrecoverableError(fmt::format("AllocateInMemory: row_count_: {} < part_id_ * 8192: {}", row_count_, part_id_ * 8192));
    }
    if (data_) [[unlikely]] {
        UnrecoverableError("AllocateInMemory: Already allocated.");
    } else if (auto &data_type = column_def_->type(); data_type->CanBuildSecondaryIndex()) [[likely]] {
        data_ = static_cast<void *>(new char[part_row_count_ * data_pair_size_]);
        LOG_TRACE("Finished AllocateInMemory().");
    } else {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
    }
}

void SecondaryIndexFileWorkerParts::FreeInMemory() {
    if (data_) [[likely]] {
        delete[] static_cast<char *>(data_);
        data_ = nullptr;
        LOG_TRACE("Finished FreeInMemory(), deleted data_ ptr.");
    } else {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
}

void SecondaryIndexFileWorkerParts::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (data_) [[likely]] {
        file_handler_->Write(data_, part_row_count_ * data_pair_size_);
        prepare_success = true;
        LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
    } else {
        UnrecoverableError("WriteToFileImpl: data_ is nullptr");
    }
}

void SecondaryIndexFileWorkerParts::ReadFromFileImpl() {
    if (row_count_ < part_id_ * 8192) {
        UnrecoverableError(fmt::format("ReadFromFileImpl: row_count_: {} < part_id_ * 8192: {}", row_count_, part_id_ * 8192));
    }
    if (!data_) [[likely]] {
        const u32 read_bytes = part_row_count_ * data_pair_size_;
        data_ = static_cast<void *>(new char[read_bytes]);
        file_handler_->Read(data_, read_bytes);
        LOG_TRACE("Finished ReadFromFileImpl().");
    } else {
        UnrecoverableError("ReadFromFileImpl: data_ is not nullptr");
    }
}

} // namespace infinity