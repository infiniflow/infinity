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

i32 BlockVersion::GetRowCount(TxnTimeStamp begin_ts) {
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

void BlockVersion::LoadFromFile(const String &version_path) {
    std::ifstream ifs(version_path);
    if (!ifs.is_open()) {
        LOG_WARN(fmt::format("Failed to open block_version file: {}", version_path));
        // load the block_version file not exist return and create version
        return;
    }
    int buf_len = std::filesystem::file_size(version_path);
    Vector<char> buf(buf_len);
    ifs.read(buf.data(), buf_len);
    ifs.close();
    char *ptr = buf.data();
    i32 created_size = ReadBufAdv<i32>(ptr);
    i32 deleted_size = ReadBufAdv<i32>(ptr);
    created_.resize(created_size);
    deleted_.resize(deleted_size);
    std::memcpy(created_.data(), ptr, created_size * sizeof(CreateField));
    ptr += created_size * sizeof(CreateField);
    std::memcpy(deleted_.data(), ptr, deleted_size * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != buf_len) {
        UnrecoverableError(fmt::format("Failed to load block_version file: {}", version_path));
    }
}

void BlockVersion::SaveToFile(const String &version_path) {
    i32 exp_size = sizeof(i32) + created_.size() * sizeof(CreateField);
    exp_size += sizeof(i32) + deleted_.size() * sizeof(TxnTimeStamp);
    Vector<char> buf(exp_size, 0);
    char *ptr = buf.data();
    WriteBufAdv<i32>(ptr, i32(created_.size()));
    WriteBufAdv<i32>(ptr, i32(deleted_.size()));
    std::memcpy(ptr, created_.data(), created_.size() * sizeof(CreateField));
    ptr += created_.size() * sizeof(CreateField);
    std::memcpy(ptr, deleted_.data(), deleted_.size() * sizeof(TxnTimeStamp));
    ptr += deleted_.size() * sizeof(TxnTimeStamp);
    if (ptr - buf.data() != exp_size) {
        UnrecoverableError(fmt::format("Failed to save block_version file: {}", version_path));
    }
    std::ofstream ofs = std::ofstream(version_path, std::ios::trunc | std::ios::binary);
    if (!ofs.is_open()) {
        UnrecoverableError(fmt::format("Failed to open block_version file: {}", version_path));
    }
    ofs.write(buf.data(), ptr - buf.data());
    ofs.flush();
    ofs.close();
}

void BlockVersion::Cleanup(const String &version_path) {
    LocalFileSystem fs;

    fs.DeleteFile(version_path);
}

} // namespace infinity