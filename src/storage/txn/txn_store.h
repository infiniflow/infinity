//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/data_block.h"
#include "src/storage/meta/entry/data_access_state.h"
#include "storage/meta/entry/table_collection_entry.h"

namespace infinity {

class Txn;

class TxnTableStore {
public:
    explicit
    TxnTableStore(String table_name, TableCollectionEntry* table_entry, Txn* txn)
            : table_name_(std::move(table_name)), table_entry_(table_entry), txn_(txn) {}

    UniquePtr<String>
    Append(const SharedPtr<DataBlock>& input_block);

    UniquePtr<String>
    Import(const SharedPtr<SegmentEntry>& segment);

    UniquePtr<String>
    Delete(const Vector<RowID>& row_ids);

    void
    Scan(SharedPtr<DataBlock>& output_block);

    void
    Rollback();

    void
    PrepareCommit();

    void
    Commit();

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    UniquePtr<AppendState> append_state_{};

    SizeT current_block_id_{0};

    String table_name_{};
    TableCollectionEntry* table_entry_{};
    Txn* txn_{};
};

}
