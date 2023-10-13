//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "src/storage/meta/entry/data_access_state.h"

namespace infinity {

class Txn;
class TableCollectionEntry;
class SegmentEntry;
class DataBlock;
class IndexEntry;

class TxnTableStore {
public:
    explicit TxnTableStore(String table_name, TableCollectionEntry *table_entry, Txn *txn)
        : table_name_(std::move(table_name)), table_entry_(table_entry), txn_(txn) {}

    UniquePtr<String> Append(const SharedPtr<DataBlock> &input_block);

    UniquePtr<String> Import(const SharedPtr<SegmentEntry> &segment);

    UniquePtr<String> CreateIndexFile(u64 segment_id, const SharedPtr<IndexEntry> &index);

    UniquePtr<String> Delete(const Vector<RowID> &row_ids);

    void Scan(SharedPtr<DataBlock> &output_block);

    void Rollback();

    void PrepareCommit();

    void Commit();

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    HashMap<u64, SharedPtr<IndexEntry>> uncommitted_indexes_{};
    UniquePtr<AppendState> append_state_{};

    SizeT current_block_id_{0};

    String table_name_{};
    TableCollectionEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
