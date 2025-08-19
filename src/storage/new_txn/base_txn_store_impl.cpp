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

#include <ranges>
#include <vector>

module infinity_core:base_txn_store.impl;

import :base_txn_store;
import :third_party;
import :stl;
import :wal_entry;
import :data_block;
import :default_values;
import :meta_cache;

namespace infinity {

void BaseTxnStore::ClearData() { return; }

String DummyTxnStore::ToString() const { return fmt::format("{}: dummy", TransactionType2Str(type_)); }

SharedPtr<WalEntry> DummyTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDummy>();
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DummyTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

String CreateDBTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id:{}, comment: {}", TransactionType2Str(type_), db_name_, db_id_, *comment_ptr_);
}

SharedPtr<WalEntry> CreateDBTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateDatabaseV2>(db_name_, db_id_str_, *comment_ptr_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CreateDBTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

String DropDBTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, create_ts: {}", TransactionType2Str(type_), db_name_, db_id_, create_ts_);
}

SharedPtr<WalEntry> DropDBTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropDatabaseV2>(db_name_, db_id_str_, create_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DropDBTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

String CreateTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_id_);
}

SharedPtr<WalEntry> CreateTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTableV2>(db_name_, db_id_str_, table_id_str_, table_def_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CreateTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String CreateTableSnapshotTxnStore::ToString() const {
    return fmt::format("{}: database: {}, table: {}, snapshot: {}, max_commit_ts: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       table_name_,
                       snapshot_name_,
                       max_commit_ts_);
}

// check if we need it
SharedPtr<WalEntry> CreateTableSnapshotTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCreateTableSnapshot>(db_name_, table_name_, snapshot_name_, max_commit_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CreateTableSnapshotTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

String DropTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, table: {}, table_id: {}, create_ts: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       table_name_,
                       table_id_str_,
                       create_ts_);
}

SharedPtr<WalEntry> DropTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDropTableV2>(db_name_, db_id_str_, table_name_, table_id_str_, create_ts_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DropTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String RestoreTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_name_, table_id_);
}

SharedPtr<WalEntry> RestoreTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;

    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdRestoreTableSnapshot>(db_name_,
                                                                           db_id_str_,
                                                                           table_name_,
                                                                           table_id_str_,
                                                                           snapshot_name_,
                                                                           table_def_,
                                                                           segment_infos_,
                                                                           index_cmds_,
                                                                           files_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> RestoreTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String RestoreDatabaseTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}", TransactionType2Str(type_), db_name_, db_id_str_);
}

SharedPtr<WalEntry> RestoreDatabaseTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    Vector<WalCmdRestoreTableSnapshot> restore_table_wal_cmds;
    for (const auto &restore_table_txn_store : restore_table_txn_stores_) {
        // Create WalCmdRestoreTableSnapshot directly from the stored data
        WalCmdRestoreTableSnapshot restore_table_cmd(restore_table_txn_store->db_name_,
                                                     restore_table_txn_store->db_id_str_,
                                                     restore_table_txn_store->table_name_,
                                                     restore_table_txn_store->table_id_str_,
                                                     restore_table_txn_store->snapshot_name_,
                                                     restore_table_txn_store->table_def_,
                                                     restore_table_txn_store->segment_infos_,
                                                     restore_table_txn_store->index_cmds_,
                                                     restore_table_txn_store->files_);
        restore_table_wal_cmds.push_back(restore_table_cmd);
    }
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdRestoreDatabaseSnapshot>(db_name_, db_id_str_, db_comment_, restore_table_wal_cmds);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> RestoreDatabaseTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

String RenameTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, old_table: {}, table_id: {}, new_table_name: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       old_table_name_,
                       table_id_str_,
                       new_table_name_);
}

SharedPtr<WalEntry> RenameTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdRenameTableV2>(db_name_, db_id_str_, old_table_name_, table_id_str_, new_table_name_, old_table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> RenameTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(std::stoull(db_id_str_), old_table_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(std::stoull(db_id_str_), new_table_name_));
    return cache_items;
}

String CreateIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_id_str_);
}

SharedPtr<WalEntry> CreateIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdCreateIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_id_str_, index_base_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CreateIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(MakeShared<MetaEraseIndexCache>(db_id_, table_id_, *index_base_->index_name_));
    return cache_items;
}

String DropIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}, create_ts: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_name_,
                       index_id_str_,
                       create_ts_);
}

SharedPtr<WalEntry> DropIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdDropIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_name_, index_id_str_, create_ts_, index_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DropIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(MakeShared<MetaEraseIndexCache>(db_id_, table_id_, index_name_));
    return cache_items;
}

String OptimizeIndexTxnStore::ToString() const {
    std::string result;
    for (const auto &store_entry : entries_) {
        SizeT size = store_entry.new_chunk_infos_.size();
        std::vector<ChunkID> chunk_ids(size);
        for (SizeT i = 0; i < size; ++i) {
            chunk_ids[i] = store_entry.new_chunk_infos_[i].chunk_id_;
        }
        result += fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, table_key: {}, index: {}, index_id: {}, segment_id: {}, "
                              "new_chunk_infos: {}, deprecate_chunks: {}\n",
                              TransactionType2Str(type_),
                              store_entry.db_name_,
                              store_entry.db_id_str_,
                              store_entry.table_name_,
                              store_entry.table_id_str_,
                              store_entry.table_key_,
                              store_entry.index_name_,
                              store_entry.index_id_str_,
                              store_entry.segment_id_,
                              fmt::join(chunk_ids, " "),
                              fmt::join(store_entry.deprecate_chunks_, " "));
    }
    return result;
}

SharedPtr<WalEntry> OptimizeIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmdDumpIndexV2> dump_command;
    for (const auto &store_entry : entries_) {
        dump_command = MakeShared<WalCmdDumpIndexV2>(store_entry.db_name_,
                                                     store_entry.db_id_str_,
                                                     store_entry.table_name_,
                                                     store_entry.table_id_str_,
                                                     store_entry.index_name_,
                                                     store_entry.index_id_str_,
                                                     store_entry.segment_id_,
                                                     store_entry.new_chunk_infos_,
                                                     store_entry.deprecate_chunks_,
                                                     store_entry.table_key_);
        dump_command->dump_cause_ = DumpIndexCause::kOptimizeIndex;
        wal_entry->cmds_.push_back(static_pointer_cast<WalCmd>(dump_command));
    }
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> OptimizeIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.reserve(entries_.size());
    for (const auto &entry : entries_) {
        cache_items.push_back(MakeShared<MetaEraseDbCache>(entry.db_name_));
        cache_items.push_back(MakeShared<MetaEraseTableCache>(entry.db_id_, entry.table_name_));
        cache_items.push_back(MakeShared<MetaEraseIndexCache>(entry.db_id_, entry.table_id_, entry.index_name_));
    }
    return cache_items;
}

String AppendTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size());
}

SizeT AppendTxnStore::RowCount() const { return input_block_->row_count(); }

SharedPtr<WalEntry> AppendTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdAppendV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ranges_, input_block_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> AppendTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

void AppendTxnStore::ClearData() { input_block_ = nullptr; }

String ImportTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_name_, table_id_);
}

SharedPtr<WalEntry> ImportTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    for (SizeT i = 0; i < segment_infos_.size(); ++i) {
        SharedPtr<WalCmdImportV2> wal_command = MakeShared<WalCmdImportV2>(db_name_, db_id_str_, table_name_, table_id_str_, segment_infos_[i]);
        wal_entry->cmds_.push_back(wal_command);
    }

    SharedPtr<WalCmdDumpIndexV2> dump_command{};
    for (const SegmentID &segment_id : segment_ids_) {
        for (SizeT i = 0; i < index_names_.size(); ++i) {
            dump_command = MakeShared<WalCmdDumpIndexV2>(db_name_,
                                                         db_id_str_,
                                                         table_name_,
                                                         table_id_str_,
                                                         index_names_[i],
                                                         index_ids_str_[i],
                                                         segment_id,
                                                         chunk_infos_in_segments_.at(segment_id),
                                                         deprecate_ids_in_segments_.at(segment_id),
                                                         table_key_);
            dump_command->dump_cause_ = DumpIndexCause::kImport;
            wal_entry->cmds_.push_back(dump_command);
        }
    }

    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> ImportTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

void ImportTxnStore::ClearData() { input_blocks_in_imports_.clear(); }

SizeT ImportTxnStore::RowCount() const {
    SizeT row_count = 0;
    for (const auto &input_blocks : input_blocks_in_imports_) {
        row_count += input_blocks.second.at(0)->row_count();
    }
    return row_count;
}

SizeT ImportTxnStore::SegmentCount() const {
    SizeT row_count = RowCount();
    SizeT segment_count = row_count % DEFAULT_SEGMENT_CAPACITY == 0 ? row_count / DEFAULT_SEGMENT_CAPACITY : row_count / DEFAULT_SEGMENT_CAPACITY + 1;
    return segment_count;
}

String DumpMemIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_name_,
                       index_id_);
}

SharedPtr<WalEntry> DumpMemIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    for (SegmentID segment_id : segment_ids_) {
        SharedPtr<WalCmdDumpIndexV2> wal_command =
            MakeShared<WalCmdDumpIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_name_, index_id_str_, segment_id, table_key_);
        wal_command->dump_cause_ = DumpIndexCause::kDumpMemIndex;
        wal_command->chunk_infos_ = chunk_infos_in_segments_.at(segment_id);
        wal_entry->cmds_.push_back(wal_command);
    }

    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DumpMemIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(MakeShared<MetaEraseIndexCache>(db_id_, table_id_, index_name_));
    return cache_items;
}

String AddColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       column_defs_.size());
}

SharedPtr<WalEntry> AddColumnsTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdAddColumnsV2>(db_name_, db_id_str_, table_name_, table_id_str_, column_idx_list_, column_defs_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> AddColumnsTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String DropColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       column_names_.size());
}

SharedPtr<WalEntry> DropColumnsTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command =
        MakeShared<WalCmdDropColumnsV2>(db_name_, db_id_str_, table_name_, table_id_str_, column_names_, column_ids_, table_key_, column_keys_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DropColumnsTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String CompactTxnStore::ToString() const {

    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, new_segment_id: {}, deprecated_segment_ids: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       new_segment_id_,
                       fmt::join(deprecated_segment_ids_, " "));
}

SharedPtr<WalEntry> CompactTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmdCompactV2> wal_command = MakeShared<WalCmdCompactV2>(db_name_,
                                                                         db_id_str_,
                                                                         table_name_,
                                                                         table_id_str_,
                                                                         index_names_,
                                                                         index_ids_str_,
                                                                         segment_infos_,
                                                                         deprecated_segment_ids_);
    wal_entry->cmds_.push_back(wal_command);

    SharedPtr<WalCmdDumpIndexV2> dump_command{};
    for (const SegmentID &segment_id : segment_ids_) {
        for (SizeT i = 0; i < index_names_.size(); ++i) {
            dump_command = MakeShared<WalCmdDumpIndexV2>(db_name_,
                                                         db_id_str_,
                                                         table_name_,
                                                         table_id_str_,
                                                         index_names_[i],
                                                         index_ids_str_[i],
                                                         segment_id,
                                                         chunk_infos_in_segments_.at(segment_id),
                                                         deprecate_ids_in_segments_.at(segment_id),
                                                         table_key_);
            dump_command->dump_cause_ = DumpIndexCause::kCompact;
            wal_entry->cmds_.push_back(dump_command);
        }
    }

    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CompactTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    for (const auto &index_name : index_names_) {
        cache_items.push_back(MakeShared<MetaEraseIndexCache>(db_id_, table_id_, index_name));
    }
    return cache_items;
}

String DeleteTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ids_.size());
}

SharedPtr<WalEntry> DeleteTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdDeleteV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ids_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> DeleteTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

String UpdateTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size(),
                       row_ids_.size());
}

SharedPtr<WalEntry> UpdateTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command1 = MakeShared<WalCmdDeleteV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ids_);
    wal_entry->cmds_.push_back(wal_command1);
    if (!input_blocks_.empty()) {
        SharedPtr<DataBlock> input_block = nullptr;
        if (input_blocks_.size() == 1) {
            input_block = input_blocks_[0];
        } else {
            SizeT total_row_count = 0;
            for (SizeT i = 0; i < row_ranges_.size(); ++i) {
                total_row_count += row_ranges_[i].second;
            }
            input_block = MakeShared<DataBlock>();
            input_block->Init(input_blocks_[0]->types(), total_row_count);
            for (SizeT i = 0; i < input_blocks_.size(); ++i) {
                input_block->AppendWith(input_blocks_[i]);
            }
            input_block->Finalize();
        }
        SharedPtr<WalCmd> wal_command2 = MakeShared<WalCmdAppendV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ranges_, input_block);
        wal_entry->cmds_.push_back(wal_command2);
    }
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> UpdateTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    Vector<SharedPtr<EraseBaseCache>> cache_items;
    cache_items.push_back(MakeShared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(MakeShared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

void UpdateTxnStore::ClearData() { input_blocks_.clear(); }

SizeT UpdateTxnStore::RowCount() const {
    SizeT row_count = 0;
    for (const auto &input_block : input_blocks_) {
        row_count += input_block->row_count();
    }
    return row_count;
}

String CheckpointTxnStore::ToString() const { return fmt::format("{}: max_commit_ts_: {}", TransactionType2Str(type_), max_commit_ts_); }

SharedPtr<WalEntry> CheckpointTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCheckpointV2>(max_commit_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CheckpointTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

String CleanupTxnStore::ToString() const { return fmt::format("{}: timestamp: {}", TransactionType2Str(type_), timestamp_); }

SharedPtr<WalEntry> CleanupTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    SharedPtr<WalEntry> wal_entry = MakeShared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    SharedPtr<WalCmd> wal_command = MakeShared<WalCmdCleanup>(timestamp_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

Vector<SharedPtr<EraseBaseCache>> CleanupTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

} // namespace infinity
