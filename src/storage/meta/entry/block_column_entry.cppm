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

export module catalog:block_column_entry;

import :base_entry;

import stl;
import buffer_obj;
import parser;
import third_party;
import buffer_manager;
import column_vector;
import local_file_system;
import vector_buffer;
import txn;

namespace infinity {

export struct BlockEntry;
export struct TableEntry;
export struct SegmentEntry;

export struct BlockColumnEntry : public BaseEntry {
    friend struct BlockEntry;

public:
    explicit BlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, const SharedPtr<String> &base_dir_ref);

    static UniquePtr<BlockColumnEntry> NewBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, Txn *txn);

    static UniquePtr<BlockColumnEntry> NewReplayBlockColumnEntry(const BlockEntry *block_entry, ColumnID column_id, BufferManager *buffer_manager);

    nlohmann::json Serialize();

    static UniquePtr<BlockColumnEntry> Deserialize(const nlohmann::json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr);

public:
    // Getter
    inline const BlockEntry *GetBlockEntry() const { return block_entry_; }
    inline const SharedPtr<DataType> &column_type() const { return column_type_; }
    inline BufferObj *buffer() const { return buffer_; }
    inline u64 column_id() const { return column_id_; }
    inline const SharedPtr<String> &base_dir() const { return base_dir_; }
    inline const BlockEntry *block_entry() const { return block_entry_; }

    SharedPtr<String> OutlineFilename(SizeT file_idx) const { return MakeShared<String>(fmt::format("col_{}_out_{}", column_id_, file_idx)); }

    String FilePath() { return LocalFileSystem::ConcatenateFilePath(*base_dir_, *file_name_); }
    Vector<String> OutlinePaths() const;

    ColumnVector GetColumnVector(BufferManager *buffer_mgr);

public:
    void Append(const ColumnVector *input_column_vector, u16 input_offset, SizeT append_rows, BufferManager *buffer_mgr);

    static void Flush(BlockColumnEntry *block_column_entry, SizeT row_count);

protected:
    const BlockEntry *block_entry_{nullptr};
    ColumnID column_id_{};
    SharedPtr<DataType> column_type_{};
    BufferObj *buffer_{};

    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};

public:
    Vector<BufferObj *> outline_buffers_{};
};

} // namespace infinity
