//
// Created by jinhai on 23-6-4.
//

module;

import stl;
import parser;
import data_access_state;

export module txn_store;

namespace infinity {

class Txn;
class TableCollectionEntry;
class SegmentEntry;
class DataBlock;

export class TxnTableStore {
public:
    explicit inline TxnTableStore(String table_name, TableCollectionEntry *table_entry, Txn *txn)
        : table_name_(Move(table_name)), table_entry_(table_entry), txn_(txn) {}

    UniquePtr<String> Append(const SharedPtr<DataBlock> &input_block);

    UniquePtr<String> Import(const SharedPtr<SegmentEntry> &segment);

    UniquePtr<String> Delete(const Vector<RowID> &row_ids);

    void Scan(SharedPtr<DataBlock> &output_block);

    void Rollback();

    void PrepareCommit();

    void Commit();

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    UniquePtr<AppendState> append_state_{};

    SizeT current_block_id_{0};

    String table_name_{};
    TableCollectionEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
