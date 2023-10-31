//
// Created by jinhai on 23-10-2.
//

module;

import stl;
import base_entry;
import buffer_obj;
import parser;
import third_party;
import column_buffer;
import outline_info;
import buffer_manager;
import column_vector;

export module block_column_entry;

namespace infinity {

class BlockEntry;

export struct BlockColumnEntry : public BaseEntry {
public:
    inline explicit BlockColumnEntry(const BlockEntry *block_entry, u64 column_id, const SharedPtr<String> &base_dir_ref)
        : BaseEntry(EntryType::kSegmentColumn), block_entry_(block_entry), column_id_(column_id), base_dir_(base_dir_ref) {}

    const BlockEntry *block_entry_{nullptr};
    u64 column_id_{};
    SharedPtr<DataType> column_type_{};
    BufferObj *buffer_{};

    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};

    UniquePtr<OutlineInfo> outline_info_{};

public:
    static UniquePtr<BlockColumnEntry>
    MakeNewBlockColumnEntry(const BlockEntry *block_entry, u64 column_id, BufferManager *buffer_manager, bool is_replay = false);

    static ColumnBuffer GetColumnData(BlockColumnEntry *column_data_entry, BufferManager *buffer_manager);

    static void
    Append(BlockColumnEntry *column_entry, offset_t column_entry_offset, ColumnVector *input_column_vector, offset_t input_offset, SizeT append_rows);

    static void AppendRaw(BlockColumnEntry *block_column_entry, SizeT dst_offset, ptr_t src_ptr, SizeT data_size);

    static void Flush(BlockColumnEntry *block_column_entry, SizeT row_count);

    static Json Serialize(BlockColumnEntry *block_column_entry);

    static UniquePtr<BlockColumnEntry> Deserialize(const Json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr);

    static SharedPtr<String> OutlineFilename(SizeT file_idx) { return MakeShared<String>(Format("out_{}", file_idx)); }
};

} // namespace infinity
