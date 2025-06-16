// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module new_txn_store;

import stl;

import status;
import internal_types;
import index_base;
import extra_ddl_info;
import column_def;
import third_party;
import logger;
import meta_key;

namespace infinity {

class NewTxn;
struct Catalog;
struct DBEntry;
struct TableIndexEntry;
struct TableEntry;
struct SegmentEntry;
struct BlockEntry;
struct BlockColumnEntry;
struct DataBlock;
struct SegmentIndexEntry;
struct ChunkIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactStatementType;
class CatalogDeltaEntry;
class BufferManager;

class KVInstance;
struct WalSegmentInfo;
struct WalCmdDumpIndexV2;

struct AppendState;
struct DeleteState;
struct AccessState;

export class NewTxnTableStore1 {
public:
    NewTxnTableStore1();

    ~NewTxnTableStore1();

    Status Append(const SharedPtr<DataBlock> &input_block);

    Status Delete(const Vector<RowID> &row_ids);

    void GetAccessState(const Vector<RowID> &row_ids, AccessState &access_state);

    AppendState *append_state() const { return append_state_.get(); }

    DeleteState &delete_state() const { return *delete_state_; }
    DeleteState &undo_delete_state();

private:
    UniquePtr<AppendState> append_state_{};
    UniquePtr<DeleteState> delete_state_{};      // Used for commit delete operation
    UniquePtr<DeleteState> undo_delete_state_{}; // Used for rollback delete operation
};

export class NewTxnStore {
public:
    explicit NewTxnStore(NewTxn *txn);

    void AddDBStore(DBEntry *db_entry);

    void DropDBStore(DBEntry *dropped_db_entry);

    void AddTableStore(TableEntry *table_entry);

    void DropTableStore(TableEntry *dropped_table_entry);

    void AddDeltaOp(CatalogDeltaEntry *local_delta_opsm, TxnManager *txn_mgr) const;

    void MaintainCompactionAlg() const;

    bool CheckConflict(Catalog *catalog);

    Optional<String> CheckConflict(const NewTxnStore &txn_store);

    void PrepareCommit1();

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void CommitBottom(TransactionID txn_id, TxnTimeStamp commit_ts);

    void Rollback(TransactionID txn_id, TxnTimeStamp abort_ts);

    bool ReadOnly() const;

    std::mutex mtx_{};

    void RevertTableStatus();

    void SetCompacting(TableEntry *table_entry);

    void SetCreatingIndex(TableEntry *table_entry);

private:
    // Txn store
    NewTxn *txn_{}; // TODO: remove this

public:
    NewTxnTableStore1 *GetNewTxnTableStore1(const String &db_id_str, const String &table_id_str);

private:
    HashMap<String, UniquePtr<NewTxnTableStore1>> txn_tables_store1_{};
};

} // namespace infinity
