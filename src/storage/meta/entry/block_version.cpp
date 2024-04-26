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

#include <fstream>

module block_version;

import stl;
import infinity_exception;
import logger;
import third_party;

import serialize;
import local_file_system;

namespace infinity {

void CreateField::SaveToFile(FileHandler &file_handler) const {
    file_handler.Write(&create_ts_, sizeof(create_ts_));
    file_handler.Write(&row_count_, sizeof(row_count_));
}

CreateField CreateField::LoadFromFile(FileHandler &file_handler) {
    CreateField create_field;
    file_handler.Read(&create_field.create_ts_, sizeof(create_field.create_ts_));
    file_handler.Read(&create_field.row_count_, sizeof(create_field.row_count_));
    return create_field;
}

bool BlockVersion::operator==(const BlockVersion &rhs) const {
    if (this->created_.size() != rhs.created_.size() || this->deleted_.size() != rhs.deleted_.size())
        return false;
    for (SizeT i = 0; i < this->created_.size(); i++) {
        if (this->created_[i] != rhs.created_[i])
            return false;
    }
    for (SizeT i = 0; i < this->deleted_.size(); i++) {
        if (this->deleted_[i] != rhs.deleted_[i])
            return false;
    }
    return true;
}

i32 BlockVersion::GetRowCount(TxnTimeStamp begin_ts) const {
    if (created_.empty())
        return 0;
    i64 idx = created_.size() - 1;
    for (; idx >= 0; idx--) {
        if (created_[idx].create_ts_ <= begin_ts)
            break;
    }
    if (idx < 0)
        return 0;
    return created_[idx].row_count_;
}

void BlockVersion::SaveToFile(TxnTimeStamp checkpoint_ts, FileHandler &file_handler) const {
    BlockOffset create_size = created_.size();
    while (create_size > 0 && created_[create_size - 1].create_ts_ > checkpoint_ts) {
        --create_size;
    }

    file_handler.Write(&create_size, sizeof(create_size));
    for (SizeT j = 0; j < create_size; ++j) {
        created_[j].SaveToFile(file_handler);
    }

    BlockOffset capacity = deleted_.size();
    file_handler.Write(&capacity, sizeof(capacity));
    TxnTimeStamp dump_ts = 0;
    for (const auto &ts : deleted_) {
        if (ts <= checkpoint_ts) {
            file_handler.Write(&ts, sizeof(ts));
        } else {
            file_handler.Write(&dump_ts, sizeof(dump_ts));
        }
    }
}

void BlockVersion::SpillToFile(FileHandler &file_handler) const {
    BlockOffset create_size = created_.size();
    file_handler.Write(&create_size, sizeof(create_size));
    for (const auto &create : created_) {
        create.SaveToFile(file_handler);
    }

    BlockOffset capacity = deleted_.size();
    file_handler.Write(&capacity, sizeof(capacity));
    file_handler.Write(deleted_.data(), capacity * sizeof(TxnTimeStamp));
}

UniquePtr<BlockVersion> BlockVersion::LoadFromFile(FileHandler &file_handler) {
    auto block_version = MakeUnique<BlockVersion>();

    BlockOffset create_size;
    file_handler.Read(&create_size, sizeof(create_size));
    block_version->created_.reserve(create_size);
    for (BlockOffset i = 0; i < create_size; i++) {
        block_version->created_.push_back(CreateField::LoadFromFile(file_handler));
    }
    BlockOffset capacity;
    file_handler.Read(&capacity, sizeof(capacity));
    block_version->deleted_.resize(capacity);
    for (BlockOffset i = 0; i < capacity; i++) {
        file_handler.Read(&block_version->deleted_[i], sizeof(TxnTimeStamp));
    }
    return block_version;
}

// void BlockVersion::Cleanup(const String &version_path) {
//     LocalFileSystem fs;

//     if (fs.Exists(version_path)) {
//         fs.DeleteFile(version_path);
//     }
// }

} // namespace infinity