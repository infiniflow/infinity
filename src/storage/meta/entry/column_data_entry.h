//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "base_entry.h"
#include "storage/buffer/object_handle.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

class BufferManager;

struct ColumnDataEntry : public BaseEntry {
public:
    explicit
    ColumnDataEntry(const void* segment_entry, TxnContext* txn_context)
        : BaseEntry(EntryType::kColumnData, txn_context),
        segment_entry_(segment_entry) {}

    const void *segment_entry_{nullptr};
    SharedPtr<DataType> column_type_{};
    SharedPtr<String> base_dir_{};
    SharedPtr<String> file_name_{};
    u64 column_id_{};
    SizeT row_capacity_{};

    BufferHandle* buffer_handle_{};
public:
    static SharedPtr<ColumnDataEntry>
    MakeNewColumnDataEntry(const void* segment_entry,
                           TxnContext* txn_context,
                           u64 column_id,
                           u64 row_capacity,
                           const SharedPtr<DataType>& data_type,
                           BufferManager* buffer_mgr);

    static ObjectHandle
    GetColumnData(ColumnDataEntry* column_data_entry, void* buffer_mgr);

    static void
    Append(ColumnDataEntry* column_data_entry,
           const SharedPtr<ColumnVector>& column_vector,
           SizeT block_start_offset,
           SizeT column_start_offset,
           SizeT rows);

    static void
    Flush(ColumnDataEntry* column_data_entry,
          SizeT row_count);

    static nlohmann::json
    Serialize(const ColumnDataEntry* column_data_entry);
};

}
