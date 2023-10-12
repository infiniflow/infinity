//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "common/default_values.h"
#include "common/types/alias/concurrency.h"
#include "common/types/alias/containers.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "json.hpp"
#include "storage/meta/entry/base_entry.h"
#include "storage/meta/entry/block_column_entry.h"

namespace infinity {

class BufferManager;

class Txn;

class SegmentEntry;

class DataBlock;

struct BlockVersion {
    BlockVersion(SizeT capacity) : created_(capacity), deleted_(capacity), txn_ptr_(capacity) {
        for (SizeT i = 0; i < capacity; ++i) {
            created_[i] = 0;
            deleted_[i] = 0;
            txn_ptr_[i] = nullptr;
        }
    }
    Vector<TxnTimeStamp> created_{};
    Vector<TxnTimeStamp> deleted_{};
    Vector<Txn*> txn_ptr_{};
};

struct BlockEntry : public BaseEntry {
public:
    explicit BlockEntry(const SegmentEntry *segment_entry, i16 block_id, TxnTimeStamp checkpoint_ts, u64 column_count, BufferManager *buffer_mgr);

    RWMutex rw_locker_{};

    const SegmentEntry *segment_entry_{};

    SharedPtr<String> base_dir_{};

    i16 block_id_{};
    i16 row_count_{};
    i16 row_capacity_{};

    Vector<UniquePtr<BlockColumnEntry>> columns_;

    UniquePtr<BlockVersion> block_version_{};

    TxnTimeStamp min_row_ts_{0}; // Indicate the commit_ts which create this BlockEntry
    TxnTimeStamp max_row_ts_{0}; // Indicate the max commit_ts which create/update/delete data inside this BlockEntry
    TxnTimeStamp checkpoint_ts_{0};

    Txn* txn_ptr_{nullptr};

    // checkpoint state
    i16 checkpoint_row_count_{0};
    std::string checkpoint_dir_{};

public:
    static int AppendData(BlockEntry *block_entry, Txn *txn_ptr, DataBlock *input_data_block, offset_t input_offset, i16 append_rows);

    static void DeleteData(BlockEntry *block_entry, Txn *txn_ptr, i16 block_offset);

    static void CommitAppend(BlockEntry *block_entry, Txn *txn_ptr);

    static void CommitDelete(BlockEntry *block_entry, Txn *txn_ptr);

    static void Flush(BlockEntry *block_entry, TxnTimeStamp checkpoint_ts);

    inline static BlockColumnEntry *GetColumnDataByID(BlockEntry *block_entry, u64 column_id) { return block_entry->columns_[column_id].get(); }

    static nlohmann::json Serialize(BlockEntry *segment_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<BlockEntry> Deserialize(const nlohmann::json &table_entry_json, SegmentEntry *table_entry, BufferManager *buffer_mgr);

    static int Room(BlockEntry *block_entry);

private:
    static SharedPtr<String> DetermineDir(const String &parent_dir, u64 block_id);
};

} // namespace infinity
