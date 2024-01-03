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
import column_buffer;
import outline_info;
import buffer_manager;
import column_vector;
import local_file_system;
import vector_buffer;


namespace infinity {

export struct BlockEntry;
export struct TableEntry;
export struct SegmentEntry;

export struct BlockColumnEntry : public BaseEntry {
    friend struct BlockEntry;
public:
    static UniquePtr<BlockColumnEntry>
    MakeNewBlockColumnEntry(const BlockEntry *block_entry, u64 column_id, BufferManager *buffer_manager, bool is_replay = false);

    inline explicit BlockColumnEntry(const BlockEntry *block_entry, u64 column_id, const SharedPtr<String> &base_dir_ref)
        : BaseEntry(EntryType::kBlockColumn), block_entry_(block_entry), column_id_(column_id), base_dir_(base_dir_ref) {}

    static Json Serialize(BlockColumnEntry *block_column_entry);

    static UniquePtr<BlockColumnEntry> Deserialize(const Json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr);

public:
    // Getter
    inline const SharedPtr<DataType> &column_type() const { return column_type_; }
    inline BufferObj *buffer() const { return buffer_; }
    inline u64 column_id() const { return column_id_; }
    inline const SharedPtr<String> &base_dir() const { return base_dir_; }
    inline const BlockEntry *block_entry() const { return block_entry_; }

    static SharedPtr<String> OutlineFilename(u64 column_id, SizeT file_idx) {
        return MakeShared<String>(Format("col_{}_out_{}", column_id, file_idx));
    }

    String FilePath() { return LocalFileSystem::ConcatenateFilePath(*base_dir_, *file_name_); }
    Vector<String> OutlinePaths() const;

    ColumnBuffer GetColumnData(BufferManager *buffer_manager);

    // Append used in import and wal_replay
    void
    AppendRaw(SizeT dst_offset, const_ptr_t src_ptr, SizeT data_size, SharedPtr<VectorBuffer> vector_buffer);

protected:

    static void
    Append(BlockColumnEntry *column_entry, u16 column_entry_offset, ColumnVector *input_column_vector, u16 input_offset, SizeT append_rows);


    static void Flush(BlockColumnEntry *block_column_entry, SizeT row_count);

protected:
    const BlockEntry *block_entry_{nullptr};
    u64 column_id_{};
    SharedPtr<DataType> column_type_{};
    BufferObj *buffer_{};

    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};

    UniquePtr<OutlineInfo> outline_info_{};
};

} // namespace infinity
