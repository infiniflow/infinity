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

#include <vector>

module table_index_entry;

import third_party;
import local_file_system;
import default_values;
import random;
import index_def;
import index_base;
import segment_iter;

import infinity_exception;
import index_full_text;
import catalog_delta_entry;
import base_table_ref;
import iresearch_datastore;
import create_index_info;
import base_entry;

namespace infinity {

TableIndexEntry::TableIndexEntry() : BaseEntry(EntryType::kDummy) {}

TableIndexEntry::TableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                 TableIndexMeta *table_index_meta,
                                 SharedPtr<String> index_dir,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts,
                                 bool is_replay)
    : BaseEntry(EntryType::kTableIndex), table_index_meta_(table_index_meta), index_def_(std::move(index_def)), index_dir_(std::move(index_dir)) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

TableIndexEntry::TableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTableIndex), table_index_meta_(table_index_meta) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewTableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                                               TableIndexMeta *table_index_meta,
                                                               Txn *txn,
                                                               TransactionID txn_id,
                                                               TxnTimeStamp begin_ts,
                                                               bool is_replay,
                                                               String replay_table_index_dir) {

    if (is_replay) {
        auto table_index_entry =
            MakeShared<TableIndexEntry>(index_def, table_index_meta, MakeShared<String>(replay_table_index_dir), txn_id, begin_ts);
        SizeT index_count = index_def->index_array_.size();
        table_index_entry->column_index_map_.reserve(index_count);

        return table_index_entry;
    } else {
        SharedPtr<String> index_dir = DetermineIndexDir(*table_index_meta->GetTableEntry()->TableEntryDir(), *index_def->index_name_);
        auto table_index_entry = MakeShared<TableIndexEntry>(index_def, table_index_meta, index_dir, txn_id, begin_ts);
        TableIndexEntry *table_index_entry_ptr = table_index_entry.get();

        {
            auto operation = MakeUnique<AddTableIndexEntryOp>(table_index_entry);
            txn->AddCatalogDeltaOperation(std::move(operation));
        }

        SizeT index_count = index_def->index_array_.size();
        table_index_entry->column_index_map_.reserve(index_count);

        HashMap<u64, SharedPtr<IndexFullText>> index_info_map;
        for (SizeT idx = 0; idx < index_def->index_array_.size(); ++idx) {
            SharedPtr<IndexBase> &index_base = index_def->index_array_[idx];

            // Get column info
            if (index_base->column_names_.size() != 1) {
                RecoverableError(Status::SyntaxError("Currently, composite index doesn't supported."));
            }
            ColumnID column_id = table_index_meta->GetTableEntry()->GetColumnIdByName(index_base->column_names_[0]);
            if (index_base->index_type_ == IndexType::kFullText) {
                SharedPtr<IndexFullText> index_fulltext = std::static_pointer_cast<IndexFullText>(index_base);
                index_info_map.emplace(column_id, index_fulltext);
            } else {
                SharedPtr<String> column_index_dir =
                    MakeShared<String>(fmt::format("{}/{}", *table_index_entry->index_dir_, index_base->column_names_[0]));
                SharedPtr<ColumnIndexEntry> column_index_entry =
                    ColumnIndexEntry::NewColumnIndexEntry(index_base, column_id, table_index_entry.get(), txn, txn_id, column_index_dir, begin_ts);
                table_index_entry->column_index_map_[column_id] = std::move(column_index_entry);
            }
        }

        if (!index_info_map.empty()) {
            table_index_entry->fulltext_index_entry_ =
                FulltextIndexEntry::NewFulltextIndexEntry(table_index_entry_ptr, txn, txn_id, table_index_entry->index_dir_, begin_ts);
        }

        return table_index_entry;
    }
}

bool TableIndexEntry::IsFulltextIndexHomebrewed() const {
    bool homebrewed = false;
    for (SizeT idx = 0; idx < index_def_->index_array_.size(); ++idx) {
        SharedPtr<IndexBase> &index_base = index_def_->index_array_[idx];
        if (index_base->index_type_ == IndexType::kFullText) {
            SharedPtr<IndexFullText> index_fulltext = std::static_pointer_cast<IndexFullText>(index_base);
            homebrewed = index_fulltext->homebrewed_;
            break;
        }
    }
    return homebrewed;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                                     const SharedPtr<IndexDef> &index_def,
                                                                     const SharedPtr<String> &index_dir,
                                                                     TransactionID txn_id,
                                                                     TxnTimeStamp begin_ts,
                                                                     TxnTimeStamp commit_ts,
                                                                     bool is_delete) {
    auto table_index_entry = MakeShared<TableIndexEntry>(index_def, table_index_meta, index_dir, txn_id, begin_ts);
    SizeT index_count = index_def->index_array_.size();
    table_index_entry->column_index_map_.reserve(index_count);
    table_index_entry->commit_ts_.store(commit_ts);
    table_index_entry->deleted_ = is_delete;
    return table_index_entry;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewDropTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
    return MakeShared<TableIndexEntry>(table_index_meta, txn_id, begin_ts);
}

// For segment_column_index_entry
void TableIndexEntry::CommitCreateIndex(u64 column_id,
                                        u32 segment_id,
                                        SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry,
                                        bool is_replay) {
    if (!is_replay) {
        // Save index file.
        segment_column_index_entry->SaveIndexFile();
    }
    {
        std::unique_lock<std::shared_mutex> w_locker(this->rw_locker_);
        ColumnIndexEntry *column_index_entry = this->column_index_map_[column_id].get();
        column_index_entry->index_by_segment_.emplace(segment_id, segment_column_index_entry);
    }
}

// For fulltext_index_entry
void TableIndexEntry::CommitCreateIndex(const SharedPtr<FulltextIndexEntry> &fulltext_index_entry) {
    this->fulltext_index_entry_ = fulltext_index_entry;
}

nlohmann::json TableIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json;

    Vector<ColumnIndexEntry *> column_index_entry_candidates;
    FulltextIndexEntry *fulltext_index_entry_candidate_{};
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json["txn_id"] = this->txn_id_.load();
        json["begin_ts"] = this->begin_ts_;
        json["commit_ts"] = this->commit_ts_.load();
        json["deleted"] = this->deleted_;
        if (this->deleted_) {
            return json;
        }

        json["index_dir"] = *this->index_dir_;
        json["index_def"] = this->index_def_->Serialize();

        for (const auto &[index_name, column_index_entry] : this->column_index_map_) {
            column_index_entry_candidates.emplace_back((ColumnIndexEntry *)column_index_entry.get());
        }

        fulltext_index_entry_candidate_ = this->fulltext_index_entry_.get();
    }

    for (const auto &column_index_entry : column_index_entry_candidates) {
        json["column_indexes"].emplace_back(column_index_entry->Serialize(max_commit_ts));
    }

    if (fulltext_index_entry_candidate_ != nullptr) {
        json["fulltext_index_entry"] = fulltext_index_entry_candidate_->Serialize(max_commit_ts);
    }

    return json;
}

SharedPtr<TableIndexEntry> TableIndexEntry::Deserialize(const nlohmann::json &index_def_entry_json,
                                                        TableIndexMeta *table_index_meta,
                                                        BufferManager *buffer_mgr,
                                                        TableEntry *table_entry) {
    TransactionID txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    bool deleted = index_def_entry_json["deleted"];

    if (deleted) {
        auto table_index_entry = NewDropTableIndexEntry(table_index_meta, txn_id, begin_ts);
        table_index_entry->deleted_ = true;
        table_index_entry->commit_ts_.store(commit_ts);
        table_index_entry->begin_ts_ = begin_ts;
        return table_index_entry;
    }

    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);
    auto index_def = IndexDef::Deserialize(index_def_entry_json["index_def"]);

    SharedPtr<TableIndexEntry> table_index_entry = MakeShared<TableIndexEntry>(index_def, table_index_meta, index_dir, txn_id, begin_ts, true);
    table_index_entry->commit_ts_.store(commit_ts);
    table_index_entry->begin_ts_ = begin_ts;

    if (index_def_entry_json.contains("column_indexes")) {
        for (const auto &column_index_entry_json : index_def_entry_json["column_indexes"]) {
            SharedPtr<ColumnIndexEntry> column_index_entry =
                ColumnIndexEntry::Deserialize(column_index_entry_json, table_index_entry.get(), buffer_mgr, table_entry);
            u64 column_id = column_index_entry->column_id_;
            table_index_entry->column_index_map_.emplace(column_id, std::move(column_index_entry));
        }
    }

    if (index_def_entry_json.contains("fulltext_index_entry")) {
        table_index_entry->fulltext_index_entry_ =
            FulltextIndexEntry::Deserialize(index_def_entry_json["fulltext_index_entry"], table_index_entry.get(), buffer_mgr);
    }
    return table_index_entry;
}

SharedPtr<String> TableIndexEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    LocalFileSystem fs;
    SharedPtr<String> index_dir;
    do {
        u32 seed = std::time(nullptr);
        index_dir = MakeShared<String>(fmt::format("{}/{}_index_{}", parent_dir, RandomString(DEFAULT_RANDOM_NAME_LEN, seed), index_name));
    } while (!fs.CreateDirectoryNoExp(*index_dir));
    return index_dir;
}

Status TableIndexEntry::CreateIndexPrepare(TableEntry *table_entry, BlockIndex *block_index, Txn *txn, bool prepare, bool is_replay, bool check_ts) {
    FulltextIndexEntry *fulltext_index_entry = this->fulltext_index_entry_.get();
    if (fulltext_index_entry != nullptr) {
        if (fulltext_index_entry->irs_index_.get() != nullptr) {
            auto *buffer_mgr = txn->GetBufferMgr();
            for (const auto *segment_entry : block_index->segments_) {
                fulltext_index_entry->irs_index_->BatchInsert(table_entry, index_def_.get(), segment_entry, buffer_mgr);
            }
            fulltext_index_entry->irs_index_->Commit();
            fulltext_index_entry->irs_index_->StopSchedule();
        } else {
            auto *buffer_mgr = txn->GetBufferMgr();
            for (const auto *segment_entry : block_index->segments_) {
                auto block_entry_iter = BlockEntryIter(segment_entry);
                for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                    fulltext_index_entry->indexer_->BatchInsert(block_entry, 0, block_entry->row_count(), buffer_mgr);
                }
                fulltext_index_entry->indexer_->Commit();
                fulltext_index_entry->indexer_->Dump();
            }
        }
    }
    for (const auto &[column_id, column_index_entry] : column_index_map_) {
        column_index_entry->CreateIndexPrepare(table_entry, block_index, column_id, txn, prepare, is_replay, check_ts);
    }
    return Status::OK();
}

Status TableIndexEntry::CreateIndexDo(const TableEntry *table_entry, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    if (column_index_map_.size() != 1) {
        // TODO
        UnrecoverableError("Not implemented");
    }
    const auto &[column_id, column_index_entry] = *column_index_map_.begin();

    const auto *column_def = table_entry->GetColumnDefByID(column_id);
    return column_index_entry->CreateIndexDo(column_def, create_index_idxes);
}

void TableIndexEntry::Cleanup() && {
    for (auto &[column_id, column_index_entry] : column_index_map_) {
        std::move(*column_index_entry).Cleanup();
    }
    // FIXME: to cleanup fulltext_index_entry_
}

bool TableIndexEntry::PickCleanup(CleanupScanner *scanner) {
    if (this->Cleanupable(scanner->visible_ts())) {
        return true;
    }
    std::unique_lock lock(rw_locker_);
    // TODO(sys)

    return false;
}

} // namespace infinity
