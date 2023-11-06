//
// Created by jinhai on 23-6-4.
//

module;

import stl;
import parser;
import data_access_state;
import index_entry;

export module txn_store;

namespace infinity {

class Txn;
class TableCollectionEntry;
class SegmentEntry;
class DataBlock;

export class TxnTableStore {
public:
    explicit inline TxnTableStore(TableCollectionEntry *table_entry, Txn *txn) : table_entry_(table_entry), txn_(txn) {}

    UniquePtr<String> Append(const SharedPtr<DataBlock> &input_block);

    UniquePtr<String> Import(const SharedPtr<SegmentEntry> &segment);

    UniquePtr<String> CreateIndexFile(u32 segment_id, SharedPtr<IndexEntry> index);

    UniquePtr<String> Delete(const Vector<RowID> &row_ids);

    void Scan(SharedPtr<DataBlock> &output_block);

    void Rollback();

    void PrepareCommit();

    void Commit() const;

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    HashMap<u32, SharedPtr<IndexEntry>> uncommitted_indexes_{};
    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

    SizeT current_block_id_{0};

    TableCollectionEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
