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
        if (worker_id_ == 0) {
            data_ = static_cast<void *>(new SecondaryIndexDataHead(part_capacity_, row_count_, data_type));
        } else {
            if (u32 previous_rows = (worker_id_ - 1) * part_capacity_; previous_rows < row_count_) [[likely]] {
                auto part_size = std::min<u32>(part_capacity_, row_count_ - previous_rows);
                data_ = static_cast<void *>(new SecondaryIndexDataPart(worker_id_ - 1, part_size));
            } else {
                UnrecoverableError(fmt::format("AllocateInMemory: previous_rows: {} >= row_count_: {}.", previous_rows, row_count_));
            }
        }
        LOG_TRACE(fmt::format("Finished AllocateInMemory() by worker_id: {}.", worker_id_));
    } else {
        UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
    }
}

void SecondaryIndexFileWorker::FreeInMemory() {
    if (data_) [[likely]] {
        if (worker_id_ == 0) {
            auto index = static_cast<SecondaryIndexDataHead *>(data_);
            delete index;
        } else {
            auto index = static_cast<SecondaryIndexDataPart *>(data_);
            delete index;
        }
        data_ = nullptr;
        LOG_TRACE(fmt::format("Finished FreeInMemory() by worker_id: {}, deleted data_ ptr.", worker_id_));
    } else {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
}

void SecondaryIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (data_) [[likely]] {
        if (worker_id_ == 0) {
            auto index = static_cast<SecondaryIndexDataHead *>(data_);
            index->SaveIndexInner(*file_handler_);
        } else {
            auto index = static_cast<SecondaryIndexDataPart *>(data_);
            index->SaveIndexInner(*file_handler_);
        }
        prepare_success = true;
        LOG_TRACE(fmt::format("Finished WriteToFileImpl(bool &prepare_success) by worker_id: {}.", worker_id_));
    } else {
        UnrecoverableError("WriteToFileImpl: data_ is nullptr");
    }
}

void SecondaryIndexFileWorker::ReadFromFileImpl() {
    if (!data_) [[likely]] {
        if (worker_id_ == 0) {
            auto index = new SecondaryIndexDataHead();
            index->ReadIndexInner(*file_handler_);
            data_ = static_cast<void *>(index);
        } else {
            auto index = new SecondaryIndexDataPart();
            index->ReadIndexInner(*file_handler_);
            data_ = static_cast<void *>(index);
        }
        LOG_TRACE(fmt::format("Finished ReadFromFileImpl() by worker_id: {}.", worker_id_));
    } else {
        UnrecoverableError("ReadFromFileImpl: data_ is not nullptr");
    }
}

} // namespace infinity