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

module infinity_core:base_txn_store.impl;

import :base_txn_store;
import :wal_entry;
import :data_block;
import :default_values;
import :meta_cache;

import std;
import third_party;

namespace infinity {

void BaseTxnStore::ClearData() { return; }

std::string DummyTxnStore::ToString() const { return fmt::format("{}: dummy", TransactionType2Str(type_)); }

std::shared_ptr<WalEntry> DummyTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdDummy>();
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DummyTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

std::string CreateDBTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id:{}, comment: {}", TransactionType2Str(type_), db_name_, db_id_, *comment_ptr_);
}

std::shared_ptr<WalEntry> CreateDBTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdCreateDatabaseV2>(db_name_, db_id_str_, *comment_ptr_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CreateDBTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

std::string DropDBTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, create_ts: {}", TransactionType2Str(type_), db_name_, db_id_, create_ts_);
}

std::shared_ptr<WalEntry> DropDBTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdDropDatabaseV2>(db_name_, db_id_str_, create_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DropDBTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

std::string CreateTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_id_);
}

std::shared_ptr<WalEntry> CreateTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdCreateTableV2>(db_name_, db_id_str_, table_id_str_, table_def_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CreateTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string CreateTableSnapshotTxnStore::ToString() const {
    return fmt::format("{}: database: {}, table: {}, snapshot: {}, max_commit_ts: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       table_name_,
                       snapshot_name_,
                       max_commit_ts_);
}

// check if we need it
std::shared_ptr<WalEntry> CreateTableSnapshotTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdCreateTableSnapshot>(db_name_, table_name_, snapshot_name_, max_commit_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CreateTableSnapshotTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

std::string DropTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, table: {}, table_id: {}, create_ts: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       table_name_,
                       table_id_str_,
                       create_ts_);
}

std::shared_ptr<WalEntry> DropTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdDropTableV2>(db_name_, db_id_str_, table_name_, table_id_str_, create_ts_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DropTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string RestoreTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_name_, table_id_);
}

std::shared_ptr<WalEntry> RestoreTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;

    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdRestoreTableSnapshot>(db_name_,
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

std::vector<std::shared_ptr<EraseBaseCache>> RestoreTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string RestoreDatabaseTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}", TransactionType2Str(type_), db_name_, db_id_str_);
}

std::shared_ptr<WalEntry> RestoreDatabaseTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::vector<WalCmdRestoreTableSnapshot> restore_table_wal_cmds;
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
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdRestoreDatabaseSnapshot>(db_name_, db_id_str_, db_comment_, restore_table_wal_cmds);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> RestoreDatabaseTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    return cache_items;
}

std::string RenameTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, old_table: {}, table_id: {}, new_table_name: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       old_table_name_,
                       table_id_str_,
                       new_table_name_);
}

std::shared_ptr<WalEntry> RenameTableTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdRenameTableV2>(db_name_, db_id_str_, old_table_name_, table_id_str_, new_table_name_, old_table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> RenameTableTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(std::stoull(db_id_str_), old_table_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(std::stoull(db_id_str_), new_table_name_));
    return cache_items;
}

std::string CreateIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_id_str_);
}

std::shared_ptr<WalEntry> CreateIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdCreateIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_id_str_, index_base_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CreateIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(std::make_shared<MetaEraseIndexCache>(db_id_, table_id_, *index_base_->index_name_));
    return cache_items;
}

std::string DropIndexTxnStore::ToString() const {
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

std::shared_ptr<WalEntry> DropIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdDropIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_name_, index_id_str_, create_ts_, index_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DropIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(std::make_shared<MetaEraseIndexCache>(db_id_, table_id_, index_name_));
    return cache_items;
}

std::string OptimizeIndexTxnStore::ToString() const {
    std::string result;
    for (const auto &store_entry : entries_) {
        size_t size = store_entry.new_chunk_infos_.size();
        std::vector<ChunkID> chunk_ids(size);
        for (size_t i = 0; i < size; ++i) {
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

std::shared_ptr<WalEntry> OptimizeIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmdDumpIndexV2> dump_command;
    for (const auto &store_entry : entries_) {
        dump_command = std::make_shared<WalCmdDumpIndexV2>(store_entry.db_name_,
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

std::vector<std::shared_ptr<EraseBaseCache>> OptimizeIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.reserve(entries_.size());
    for (const auto &entry : entries_) {
        cache_items.push_back(std::make_shared<MetaEraseDbCache>(entry.db_name_));
        cache_items.push_back(std::make_shared<MetaEraseTableCache>(entry.db_id_, entry.table_name_));
        cache_items.push_back(std::make_shared<MetaEraseIndexCache>(entry.db_id_, entry.table_id_, entry.index_name_));
    }
    return cache_items;
}

std::string AppendTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size());
}

size_t AppendTxnStore::RowCount() const { return input_block_->row_count(); }

std::shared_ptr<WalEntry> AppendTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdAppendV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ranges_, input_block_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> AppendTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

void AppendTxnStore::ClearData() { input_block_ = nullptr; }

std::string ImportTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_name_, table_id_);
}

std::shared_ptr<WalEntry> ImportTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    for (size_t i = 0; i < segment_infos_.size(); ++i) {
        std::shared_ptr<WalCmdImportV2> wal_command =
            std::make_shared<WalCmdImportV2>(db_name_, db_id_str_, table_name_, table_id_str_, segment_infos_[i]);
        wal_entry->cmds_.push_back(wal_command);
    }

    std::shared_ptr<WalCmdDumpIndexV2> dump_command{};
    for (const SegmentID &segment_id : segment_ids_) {
        for (size_t i = 0; i < index_names_.size(); ++i) {
            dump_command = std::make_shared<WalCmdDumpIndexV2>(db_name_,
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

std::vector<std::shared_ptr<EraseBaseCache>> ImportTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

size_t ImportTxnStore::SegmentCount() const {
    size_t segment_count =
        row_count_ % DEFAULT_SEGMENT_CAPACITY == 0 ? row_count_ / DEFAULT_SEGMENT_CAPACITY : row_count_ / DEFAULT_SEGMENT_CAPACITY + 1;
    return segment_count;
}

std::string DumpMemIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_name_,
                       index_id_);
}

std::shared_ptr<WalEntry> DumpMemIndexTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    for (SegmentID segment_id : segment_ids_) {
        std::shared_ptr<WalCmdDumpIndexV2> wal_command =
            std::make_shared<WalCmdDumpIndexV2>(db_name_, db_id_str_, table_name_, table_id_str_, index_name_, index_id_str_, segment_id, table_key_);
        wal_command->dump_cause_ = DumpIndexCause::kDumpMemIndex;
        wal_command->chunk_infos_ = chunk_infos_in_segments_.at(segment_id);
        wal_entry->cmds_.push_back(wal_command);
    }

    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DumpMemIndexTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    cache_items.push_back(std::make_shared<MetaEraseIndexCache>(db_id_, table_id_, index_name_));
    return cache_items;
}

std::string AddColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       column_defs_.size());
}

std::shared_ptr<WalEntry> AddColumnsTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdAddColumnsV2>(db_name_, db_id_str_, table_name_, table_id_str_, column_idx_list_, column_defs_, table_key_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> AddColumnsTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string DropColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       column_names_.size());
}

std::shared_ptr<WalEntry> DropColumnsTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command =
        std::make_shared<WalCmdDropColumnsV2>(db_name_, db_id_str_, table_name_, table_id_str_, column_names_, column_ids_, table_key_, column_keys_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DropColumnsTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string CompactTxnStore::ToString() const {

    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, new_segment_id: {}, deprecated_segment_ids: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       new_segment_id_,
                       fmt::join(deprecated_segment_ids_, " "));
}

std::shared_ptr<WalEntry> CompactTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmdCompactV2> wal_command = std::make_shared<WalCmdCompactV2>(db_name_,
                                                                                     db_id_str_,
                                                                                     table_name_,
                                                                                     table_id_str_,
                                                                                     index_names_,
                                                                                     index_ids_str_,
                                                                                     segment_infos_,
                                                                                     deprecated_segment_ids_);
    wal_entry->cmds_.push_back(wal_command);

    std::shared_ptr<WalCmdDumpIndexV2> dump_command{};
    for (const SegmentID &segment_id : segment_ids_) {
        for (size_t i = 0; i < index_names_.size(); ++i) {
            dump_command = std::make_shared<WalCmdDumpIndexV2>(db_name_,
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

std::vector<std::shared_ptr<EraseBaseCache>> CompactTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    for (const auto &index_name : index_names_) {
        cache_items.push_back(std::make_shared<MetaEraseIndexCache>(db_id_, table_id_, index_name));
    }
    return cache_items;
}

std::string DeleteTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ids_.size());
}

std::shared_ptr<WalEntry> DeleteTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdDeleteV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ids_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> DeleteTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

std::string UpdateTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size(),
                       row_ids_.size());
}

std::shared_ptr<WalEntry> UpdateTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command1 = std::make_shared<WalCmdDeleteV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ids_);
    wal_entry->cmds_.push_back(wal_command1);
    if (!input_blocks_.empty()) {
        std::shared_ptr<DataBlock> input_block = nullptr;
        if (input_blocks_.size() == 1) {
            input_block = input_blocks_[0];
        } else {
            size_t total_row_count = 0;
            for (size_t i = 0; i < row_ranges_.size(); ++i) {
                total_row_count += row_ranges_[i].second;
            }
            input_block = std::make_shared<DataBlock>();
            input_block->Init(input_blocks_[0]->types(), total_row_count);
            for (size_t i = 0; i < input_blocks_.size(); ++i) {
                input_block->AppendWith(input_blocks_[i]);
            }
            input_block->Finalize();
        }
        std::shared_ptr<WalCmd> wal_command2 =
            std::make_shared<WalCmdAppendV2>(db_name_, db_id_str_, table_name_, table_id_str_, row_ranges_, input_block);
        wal_entry->cmds_.push_back(wal_command2);
    }
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> UpdateTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const {
    std::vector<std::shared_ptr<EraseBaseCache>> cache_items;
    cache_items.push_back(std::make_shared<MetaEraseDbCache>(db_name_));
    cache_items.push_back(std::make_shared<MetaEraseTableCache>(db_id_, table_name_));
    return cache_items;
}

void UpdateTxnStore::ClearData() { input_blocks_.clear(); }

size_t UpdateTxnStore::RowCount() const {
    size_t row_count = 0;
    for (const auto &input_block : input_blocks_) {
        row_count += input_block->row_count();
    }
    return row_count;
}

std::string CheckpointTxnStore::ToString() const { return fmt::format("{}: max_commit_ts_: {}", TransactionType2Str(type_), max_commit_ts_); }

std::shared_ptr<WalEntry> CheckpointTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdCheckpointV2>(max_commit_ts_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CheckpointTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

std::string CleanupTxnStore::ToString() const { return fmt::format("{}: timestamp: {}", TransactionType2Str(type_), timestamp_); }

std::shared_ptr<WalEntry> CleanupTxnStore::ToWalEntry(TxnTimeStamp commit_ts) const {
    std::shared_ptr<WalEntry> wal_entry = std::make_shared<WalEntry>();
    wal_entry->commit_ts_ = commit_ts;
    std::shared_ptr<WalCmd> wal_command = std::make_shared<WalCmdCleanup>(timestamp_);
    wal_entry->cmds_.push_back(wal_command);
    return wal_entry;
}

std::vector<std::shared_ptr<EraseBaseCache>> CleanupTxnStore::ToCachedMeta(TxnTimeStamp commit_ts) const { return {}; }

} // namespace infinity
