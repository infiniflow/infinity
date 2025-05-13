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
import column_def;
import third_party;

namespace infinity {

class DataBlock;
class IndexBase;

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
    explicit BaseTxnStore(TransactionType type) : type_(type) {
    };

    TransactionType type_{TransactionType::kInvalid};

    virtual String CompactInfo() const = 0;
    virtual ~BaseTxnStore() = default;
};

export struct CreateDBTxnStore : public BaseTxnStore {
    CreateDBTxnStore() : BaseTxnStore(TransactionType::kCreateDB) {
    }

    String db_name_{};
    u64 db_id_{};
    SharedPtr<String> comment_ptr_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id:{}, comment: {}", TransactionType2Str(type_), db_name_, db_id_, *comment_ptr_);
    }
};

export struct DropDBTxnStore : public BaseTxnStore {
    DropDBTxnStore() : BaseTxnStore(TransactionType::kDropDB) {
    }

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}", TransactionType2Str(type_), db_name_, db_id_);
    }
};

export struct CreateTableTxnStore : public BaseTxnStore {
    CreateTableTxnStore() : BaseTxnStore(TransactionType::kCreateTable) {
    }

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_,
                           table_id_);
    }
};

export struct DropTableTxnStore : public BaseTxnStore {
    DropTableTxnStore() : BaseTxnStore(TransactionType::kDropTable) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, table_name_, table_id_str_);
    }
};

export struct CreateIndexTxnStore : public BaseTxnStore {
    CreateIndexTxnStore() : BaseTxnStore(TransactionType::kCreateIndex) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    SharedPtr<IndexBase> index_base_{};
    u64 index_id_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_,
                           index_id_);
    }
};

export struct DropIndexTxnStore : public BaseTxnStore {
    DropIndexTxnStore() : BaseTxnStore(TransactionType::kDropIndex) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    String index_name_{};
    String index_id_str_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_,
                           index_name_,
                           index_id_str_);
    }
};

export struct AppendTxnStore : public BaseTxnStore {
    AppendTxnStore() : BaseTxnStore(TransactionType::kAppend) {
    }

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

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_,
                           table_name_,
                           table_id_);
    }
};

export struct ImportTxnStore : public BaseTxnStore {
    ImportTxnStore() : BaseTxnStore(TransactionType::kImport) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<DataBlock>> input_blocks_{};
    SegmentID segment_id_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_);

    }
};

export struct DumpMemIndexTxnStore : public BaseTxnStore {
    DumpMemIndexTxnStore() : BaseTxnStore(TransactionType::kDumpMemIndex) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    String index_name_{};
    String index_id_str_{};
    u64 index_id_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_,
                           index_name_,
                           index_id_str_);
    }
};


export struct AddColumnsTxnStore : public BaseTxnStore {
    AddColumnsTxnStore() : BaseTxnStore(TransactionType::kAddColumn) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<ColumnDef>> column_defs_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_,
                           column_defs_.size());
    }
};

export struct DropColumnsTxnStore : public BaseTxnStore {
    DropColumnsTxnStore() : BaseTxnStore(TransactionType::kDropColumn) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<String> column_names_{};
    Vector<ColumnID> column_ids_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_,
                           table_name_,
                           table_id_,
                           column_names_.size());
    }
};

export struct CompactTxnStore : public BaseTxnStore {
    CompactTxnStore() : BaseTxnStore(TransactionType::kCompact) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SegmentID> segment_ids_{};


    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_str_,
                           table_name_,
                           table_id_str_);
    }
};

export struct DeleteTxnStore : public BaseTxnStore {
    DeleteTxnStore() : BaseTxnStore(TransactionType::kDelete) {
    }

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<RowID> row_ids_{};

    String CompactInfo() const final {
        return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, deleted: {}",
                           TransactionType2Str(type_),
                           db_name_,
                           db_id_,
                           table_name_,
                           table_id_,
                           row_ids_.size());
    }
};

} // namespace infinity