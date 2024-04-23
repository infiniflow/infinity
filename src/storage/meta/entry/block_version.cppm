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

export module block_version;

import stl;
import file_system;

namespace infinity {

struct CreateField {
    //    CreateField(TxnTimeStamp create_ts, i32 row_count) : create_ts_(create_ts), row_count_(row_count) {}

    TxnTimeStamp create_ts_{};
    i64 row_count_{};

    bool operator==(const CreateField &rhs) const { return create_ts_ == rhs.create_ts_ && row_count_ == rhs.row_count_; }

    bool operator!=(const CreateField &rhs) const { return !(*this == rhs); }

    void SaveToFile(FileHandler &file_handler) const;

    static CreateField LoadFromFile(FileHandler &file_handler);
};

export struct BlockVersion {
    constexpr static std::string_view PATH = "version";

    static SharedPtr<String> FileName() { return MakeShared<String>(PATH); }

    explicit BlockVersion(SizeT capacity) : deleted_(capacity, 0) {}
    BlockVersion() = default;

    bool operator==(const BlockVersion &rhs) const;
    bool operator!=(const BlockVersion &rhs) const { return !(*this == rhs); };

    i32 GetRowCount(TxnTimeStamp begin_ts) const;

    void SaveToFile(TxnTimeStamp checkpoint_ts, FileHandler &file_handler) const;

    void SpillToFile(FileHandler &file_handler) const;

    static UniquePtr<BlockVersion> LoadFromFile(FileHandler &file_handler);

    // void Cleanup(const String &version_path);

    Vector<CreateField> created_{}; // second field width is same as timestamp, otherwise Valgrind will issue BlockVersion::SaveToFile has
                                    // risk to write uninitialized buffer. (ts, rows)
    Vector<TxnTimeStamp> deleted_{};
};

} // namespace infinity
