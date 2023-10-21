//
// Created by jinhai on 23-6-30.
//

module;

import stl;
import base_entry;
import parser;
import buffer_handle;
import outline_info;
import column_buffer;
import column_vector;
import buffer_manager;
import third_party;

export module segment_column_entry;

namespace infinity {

// class BufferManager;

class SegmentEntry;

// class ColumnVector;

export struct SegmentColumnEntry : public BaseEntry {
public:
    explicit SegmentColumnEntry(const SegmentEntry *segment_entry) : BaseEntry(EntryType::kSegmentColumn), segment_entry_(segment_entry) {}

    const SegmentEntry *segment_entry_{nullptr};
    SharedPtr<DataType> column_type_{};
    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};
    u64 column_id_{};
    SizeT row_capacity_{};

    BufferHandle *buffer_handle_{};

    UniquePtr<OutlineInfo> outline_info_{};

public:
    static SharedPtr<SegmentColumnEntry> MakeNewColumnDataEntry(const SegmentEntry *segment_entry,
                                                                u64 column_id,
                                                                u64 row_capacity,
                                                                const SharedPtr<DataType> &data_type,
                                                                BufferManager *buffer_mgr);

    static ColumnBuffer GetColumnData(SegmentColumnEntry *column_data_entry, BufferManager *buffer_mgr);

    static void Append(SegmentColumnEntry *column_data_entry,
                       const SharedPtr<ColumnVector> &column_vector,
                       SizeT block_start_offset,
                       SizeT column_start_offset,
                       SizeT row_n);

    static void AppendRaw(SegmentColumnEntry *column_data_entry, SizeT dst_offset, ptr_t src_ptr, SizeT data_size);

    static void Flush(SegmentColumnEntry *column_data_entry, SizeT row_count);

    static Json Serialize(const SegmentColumnEntry *column_data_entry);

    static SharedPtr<SegmentColumnEntry> Deserialize(const Json &column_data_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    static SharedPtr<String> OutlineFilename(SizeT file_idx) { return MakeShared<String>(Format("out_{}", file_idx)); }
};

} // namespace infinity