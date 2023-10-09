//
// Created by jinhai on 23-10-2.
//

#pragma once

#include "common/types/data_type.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/column_buffer.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/meta/entry/outline_info.h"

namespace infinity {

class BufferManager;

class BlockEntry;

class ColumnVector;

struct BlockColumnEntry : public BaseEntry {
public:
    explicit BlockColumnEntry(const BlockEntry *block_entry, u64 column_id, const SharedPtr<String> &base_dir_ref)
        : BaseEntry(EntryType::kSegmentColumn), block_entry_(block_entry), column_id_(column_id), base_dir_(base_dir_ref) {}

    const BlockEntry *block_entry_{nullptr};
    u64 column_id_{};
    SharedPtr<DataType> column_type_{};
    BufferHandle *buffer_handle_{};

    const SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};

    UniquePtr<OutlineInfo> outline_info_{};

public:
    static UniquePtr<BlockColumnEntry> MakeNewBlockColumnEntry(const BlockEntry *block_entry, u64 column_id, BufferManager *buffer_manager);

    static ColumnBuffer GetColumnData(BlockColumnEntry *column_data_entry, BufferManager *buffer_manager);

    static void
    Append(BlockColumnEntry *column_entry, offset_t column_entry_offset, ColumnVector *input_column_vector, offset_t input_offset, SizeT append_rows);

    static void AppendRaw(BlockColumnEntry *block_column_entry, SizeT dst_offset, ptr_t src_ptr, SizeT data_size);

    static void Flush(BlockColumnEntry *block_column_entry, SizeT row_count);

    static nlohmann::json Serialize(const BlockColumnEntry *block_column_entry);

    static UniquePtr<BlockColumnEntry> Deserialize(const nlohmann::json &column_data_json, BlockEntry *block_entry, BufferManager *buffer_mgr);
};

} // namespace infinity