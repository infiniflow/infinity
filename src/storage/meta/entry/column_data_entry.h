//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "base_entry.h"

#include "storage/buffer/column_buffer.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

class BufferManager;

class SegmentEntry;

struct OutlineInfo {
    Vector<Pair<BufferHandle*, SizeT>> written_buffers_{};

    BufferManager* buffer_mgr_{};

    SizeT next_file_idx{};

    OutlineInfo(BufferManager* buffer_mgr) : buffer_mgr_(buffer_mgr) {}
};

struct ColumnDataEntry : public BaseEntry {
public:
    explicit
    ColumnDataEntry(const SegmentEntry* segment_entry)
            : BaseEntry(EntryType::kColumnData),
              segment_entry_(segment_entry) {}

    const SegmentEntry* segment_entry_{nullptr};
    SharedPtr<DataType> column_type_{};
    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};
    u64 column_id_{};
    SizeT row_capacity_{};

    BufferHandle* buffer_handle_{};

    UniquePtr<OutlineInfo> outline_info_;
public:
    static SharedPtr<ColumnDataEntry>
    MakeNewColumnDataEntry(const SegmentEntry* segment_entry,
                           u64 column_id,
                           u64 row_capacity,
                           const SharedPtr<DataType>& data_type,
                           BufferManager* buffer_mgr);

    static ColumnBuffer
    GetColumnData(ColumnDataEntry* column_data_entry, BufferManager* buffer_mgr);

    static void
    Append(ColumnDataEntry* column_data_entry,
           const SharedPtr<ColumnVector>& column_vector,
           SizeT block_start_offset,
           SizeT column_start_offset,
           SizeT row_n);


    static void
    AppendRaw(ColumnDataEntry* column_data_entry, SizeT dst_offset, ptr_t src_ptr, SizeT data_size);

    static void
    Flush(ColumnDataEntry* column_data_entry,
          SizeT row_count);

    static nlohmann::json
    Serialize(const ColumnDataEntry* column_data_entry);

    static SharedPtr<ColumnDataEntry>
    Deserialize(const nlohmann::json& column_data_json, SegmentEntry* table_entry, BufferManager* buffer_mgr);

    static SharedPtr<String>
    OutlineFilename(SizeT file_idx) {
        return MakeShared<String>("out_" + std::to_string(file_idx));
    }
};

}
