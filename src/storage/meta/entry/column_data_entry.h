//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "base_entry.h"
#include "common/types/internal_types.h"
#include "storage/buffer/object_handle.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

class BufferManager;
class SegmentEntry;
struct ColumnDataEntry : public BaseEntry {
public:
    explicit
    ColumnDataEntry(const SegmentEntry* segment_entry)
        : BaseEntry(EntryType::kColumnData),
        segment_entry_(segment_entry) {}

    const SegmentEntry *segment_entry_{nullptr};
    SharedPtr<DataType> column_type_{};
    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};
    u64 column_id_{};
    SizeT row_capacity_{};

    BufferHandle* buffer_handle_{};
public:
    static SharedPtr<ColumnDataEntry>
    MakeNewColumnDataEntry(const SegmentEntry* segment_entry,
                           u64 column_id,
                           u64 row_capacity,
                           const SharedPtr<DataType>& data_type,
                           BufferManager* buffer_mgr);

    static ObjectHandle
    GetColumnData(ColumnDataEntry* column_data_entry, BufferManager* buffer_mgr);

    static void
    Append(ColumnDataEntry* column_data_entry,
           const SharedPtr<ColumnVector>& column_vector,
           SizeT block_start_offset,
           SizeT column_start_offset,
           SizeT rows);
    
    static void
    Append(ColumnDataEntry* column_data_entry, const StringView& data, SizeT offset);

    static void
    AppendEmbedding(ColumnDataEntry* column_data_entry, const StringView& data, SizeT offset, char delimiter);

    static void
    Flush(ColumnDataEntry* column_data_entry,
          SizeT row_count);

    static nlohmann::json
    Serialize(const ColumnDataEntry* column_data_entry);

    static SharedPtr<ColumnDataEntry>
    Deserialize(const nlohmann::json& column_data_json, SegmentEntry* table_entry, BufferManager* buffer_mgr);
};

}
