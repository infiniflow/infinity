// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module base_txn_store;

import stl;
import internal_types;
import txn_state;

namespace infinity {

class DataBlock;

export struct MemIndexRange {
    String index_id_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    SegmentOffset start_offset_{};
    SizeT row_count_{};
};

// enum class TxnStoreType {
//     kInvalid,
//     kCreateDB,
//     kCreateTable,
//     kAppend,
//     kImport,
//     kDelete,
//     kUpdate,
//     kCreateIndex,
//     kDumpMemIndex,
//     kOptimizeIndex,
//     kCompact,
// };
// kCheckpoint, // Develop know it's a checkpoint txn
// kRead,       // Developer know it's a read txn
// kNormal,     // Developer doesn't know what type is this txn
// kReplay,
// kRecovery,
// kNewCheckpoint,
// kAppend,
// kUpdate,
// kImport,
// kDumpMemIndex,
// kOptimizeIndex,
// kCreateIndex,
// kDelete,
// kCompact,

export struct BaseTxnStore {
    explicit BaseTxnStore(TransactionType type) : type_(type) {};

    TransactionType type_{TransactionType::kInvalid};
};

export struct CreateDBTxnStore : public BaseTxnStore {
    CreateDBTxnStore() : BaseTxnStore(TransactionType::kCreateDB) {}

    String db_name_{};
    u64 db_id_{};
};

export struct DropDBTxnStore : public BaseTxnStore {
    DropDBTxnStore() : BaseTxnStore(TransactionType::kDropDB) {}

    String db_name_{};
    u64 db_id_{};
};

export struct CreateTableTxnStore : public BaseTxnStore {
    CreateTableTxnStore() : BaseTxnStore(TransactionType::kCreateTable) {}

    String db_name_{};
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
};

export struct DropTableTxnStore : public BaseTxnStore {
    DropTableTxnStore() : BaseTxnStore(TransactionType::kDropTable) {}

    String db_name_{};
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
};

export struct CreateIndexTxnStore : public BaseTxnStore {
    CreateIndexTxnStore() : BaseTxnStore(TransactionType::kCreateIndex) {}

    String db_name_{};
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
    String index_name_{};
    u64 index_id_{};
};

export struct DropIndexTxnStore : public BaseTxnStore {
    DropIndexTxnStore() : BaseTxnStore(TransactionType::kDropIndex) {}

    String db_name_{};
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
    String index_name_{};
    u64 index_id_{};
};

export struct AppendTxnStore : public BaseTxnStore {
    AppendTxnStore() : BaseTxnStore(TransactionType::kAppend) {}

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    SharedPtr<DataBlock> input_block_{};
    Vector<String> index_ids_{}; // indexes will be appended

    // For data append
    Vector<Pair<RowID, u64>> row_ranges_{};

    // For mem index
    Vector<MemIndexRange> mem_indexes_to_append_{};
    Vector<MemIndexRange> mem_indexes_to_dump_{};
};

export struct ImportTxnStore : public BaseTxnStore {
    ImportTxnStore() : BaseTxnStore(TransactionType::kImport) {}

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<DataBlock>> input_blocks_{};
    SegmentID segment_id_{};
};

} // namespace infinity