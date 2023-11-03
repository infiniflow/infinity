//
// Created by jinhai on 23-8-18.
//

module;

import stl;
import std;
import parser;
import base_entry;
import table_collection_meta;
import table_collection_type;
import third_party;
import txn;
import buffer_manager;
import segment_entry;
import index_def;
import block_index;
import data_access_state;
import db_entry;
import logger;
import txn_store;
import infinity_assert;
import infinity_exception;
import ivfflat_index_def;
import index_def_meta;
import txn_manager;
import index_entry;

module table_collection_entry;

namespace infinity {

TableCollectionEntry::TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                           SharedPtr<String> table_collection_name,
                                           const Vector<SharedPtr<ColumnDef>> &columns,
                                           TableCollectionType table_collection_type,
                                           TableCollectionMeta *table_collection_meta,
                                           u64 txn_id,
                                           TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable), table_entry_dir_(MakeShared<String>(Format("{}/{}/txn_{}", *db_entry_dir, *table_collection_name, txn_id))),
      table_collection_name_(Move(table_collection_name)), columns_(columns), table_collection_type_(table_collection_type),
      table_collection_meta_(table_collection_meta) {
    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        name2id_[columns[idx]->name()] = idx;
    }

    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

EntryResult TableCollectionEntry::CreateIndex(TableCollectionEntry *table_entry,
                                              SharedPtr<IndexDef> index_def,
                                              ConflictType conflict_type,
                                              u64 txn_id,
                                              TxnTimeStamp begin_ts,
                                              TxnManager *txn_mgr) {
    table_entry->rw_locker_.lock_shared();
    IndexDefMeta *index_def_meta{nullptr};
    if (index_def->index_name_->empty()) {
        // set default index name
        int index_id = 0;
        String index_name_base = "idx";
        for (const auto &column_name : index_def->column_names_) {
            index_name_base += "_" + column_name;
        }
        while (true) {
            auto index_name = index_id > 0 ? (index_name_base + "_" + ToStr(index_id)) : index_name_base;
            if (auto iter = table_entry->indexes_.find(index_name); iter == table_entry->indexes_.end()) {
                index_def->index_name_ = MakeShared<String>(Move(index_name));
                break;
            }
            index_id++;
        }
    } else if (auto iter = table_entry->indexes_.find(*index_def->index_name_); iter != table_entry->indexes_.end()) {
        index_def_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();

    if (index_def_meta == nullptr) {
        LOG_TRACE(Format("Create new index: {}", *index_def->index_name_));
        auto new_index_def_meta = MakeUnique<IndexDefMeta>(index_def->index_name_, table_entry);
        index_def_meta = new_index_def_meta.get();

        table_entry->rw_locker_.lock();
        table_entry->indexes_[*index_def->index_name_] = Move(new_index_def_meta);
        table_entry->rw_locker_.unlock();

        LOG_TRACE(Format("Add new index entry for {} in new index meta of table_entry {} ", *index_def->index_name_, *table_entry->table_entry_dir_));
    } else {
        LOG_TRACE(
            Format("Add new index entry for {} in existed index meta of table_entry {}", *index_def->index_name_, *table_entry->table_entry_dir_));
    }
    IndexDef *index_def_ptr = index_def.get();
    EntryResult res = IndexDefMeta::CreateNewEntry(index_def_meta, Move(index_def), conflict_type, txn_id, begin_ts, txn_mgr);

    return res;
}

EntryResult TableCollectionEntry::DropIndex(TableCollectionEntry *table_entry,
                                            const String &index_name,
                                            ConflictType conflict_type,
                                            u64 txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr) {
    table_entry->rw_locker_.lock_shared();

    IndexDefMeta *index_meta{nullptr};
    if (auto iter = table_entry->indexes_.find(index_name); iter != table_entry->indexes_.end()) {
        index_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();
    if (index_meta == nullptr) {
        switch (conflict_type) {
            LOG_TRACE(Format("Attempt to drop not existed index entry {}", index_name));
            case ConflictType::kIgnore: {
                return {nullptr, nullptr};
            }
            case ConflictType::kError: {
                return {nullptr, MakeUnique<String>("Attempt to drop not existed index entry")};
            }
            default: {
                Error<StorageException>("Invalid conflict type.", __FILE_NAME__, __LINE__);
            }
        }
        Error<StorageException>("Should not reach here.", __FILE_NAME__, __LINE__);
    } else {
        LOG_TRACE(Format("Drop index entry {}", index_name));
        return IndexDefMeta::DropNewEntry(index_meta, conflict_type, txn_id, begin_ts, txn_mgr);
    }
}

EntryResult TableCollectionEntry::GetIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts) {
    Error<StorageException>("Not implemented.", __FILE_NAME__, __LINE__);
}

void TableCollectionEntry::RemoveIndexEntry(TableCollectionEntry *table_entry, const SharedPtr<String> &index_name, u64 txn_id, TxnManager *txn_mgr) {
    table_entry->rw_locker_.lock_shared();

    IndexDefMeta *index_def_meta{nullptr};
    if (auto iter = table_entry->indexes_.find(*index_name); iter != table_entry->indexes_.end()) {
        index_def_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();

    LOG_TRACE(Format("Remove index entry: {}", *index_name));
    IndexDefMeta::DeleteNewEntry(index_def_meta, txn_id, txn_mgr);
}

void TableCollectionEntry::Append(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store, BufferManager *buffer_mgr) {
    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    Txn *transaction_ptr = (Txn *)txn_ptr;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    if (append_state_ptr->Finished()) {
        LOG_TRACE("No append is done.");
        return;
    }

    while (!append_state_ptr->Finished()) {
        {
            UniqueLock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation
            if (table_entry->unsealed_segment_ == nullptr || SegmentEntry::Room(table_entry->unsealed_segment_) <= 0) {
                // unsealed_segment_ is unpopulated or full
                SharedPtr<SegmentEntry> new_segment =
                    SegmentEntry::MakeNewSegmentEntry(table_entry, TableCollectionEntry::GetNextSegmentID(table_entry), buffer_mgr);
                table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
                table_entry->unsealed_segment_ = new_segment.get();
                LOG_TRACE(Format("Created a new segment {}", new_segment->segment_id_));
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        int actual_appended = SegmentEntry::AppendData(table_entry->unsealed_segment_, txn_ptr, append_state_ptr, buffer_mgr);
        LOG_TRACE(Format("Segment {} is appended with {} rows", int(table_entry->unsealed_segment_->segment_id_), actual_appended));
    }
}

void TableCollectionEntry::CreateIndexFile(TableCollectionEntry *table_entry,
                                           void *txn_store,
                                           const IndexDef &index_def,
                                           TxnTimeStamp begin_ts,
                                           BufferManager *buffer_mgr) {
    auto txn_store_ptr = static_cast<TxnTableStore *>(txn_store);
    switch (index_def.method_type_) {
        case IndexMethod::kIVFFlat: {
            // check whether the column def is valid
            if (index_def.column_names_.size() != 1) {
                StorageException("IVFFlat index should created on one column");
            }
            const String &column_name = index_def.column_names_[0];
            u64 column_id = table_entry->GetColumnIdByName(column_name);
            ColumnDef &column_def = *table_entry->columns_[column_id];
            if (column_def.type()->type() != LogicalType::kEmbedding) {
                StorageException("IVFFlat index should created on Embedding type column");
            }
            auto type_info = column_def.type()->type_info().get();
            auto embedding_info = (EmbeddingInfo *)type_info;
            if (embedding_info->Type() != EmbeddingDataType::kElemFloat) {
                StorageException("IVFFlat index should created on float embedding type column");
            }

            for (const auto &[_segment_id, segment_entry] : table_entry->segments_) {
                SegmentEntry::CreateIndexEmbedding(segment_entry.get(),
                                                   index_def,
                                                   column_id,
                                                   embedding_info->Dimension(),
                                                   begin_ts,
                                                   buffer_mgr,
                                                   txn_store_ptr);
            }
            break;
        }
        case IndexMethod::kInvalid: {
            StorageException("Invalid index method type.");
        }
        default: {
            NotImplementException("Not implemented.");
        }
    }
}

UniquePtr<String> TableCollectionEntry::Delete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &delete_state) {
    for(const auto& to_delete_seg_rows: delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment = TableCollectionEntry::GetSegmentByID(table_entry, segment_id);
        if(segment == nullptr) {
            Error<ExecutorException>(Format("Going to delete data in non-exist segment: {}", segment_id), __FILE_NAME__, __LINE__);
        }
        const HashMap<u16, Vector<RowID>>& block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::DeleteData(segment, txn_ptr, block_row_hashmap);
    }
    return nullptr;
}

UniquePtr<String> TableCollectionEntry::InitScan(TableCollectionEntry *table_entry, Txn *txn_ptr, ScanState &scan_state, BufferManager *buffer_mgr) {
    Error<NotImplementException>("TableCollectionEntry::InitScan", __FILE_NAME__, __LINE__);
    return nullptr;
}

UniquePtr<String>
TableCollectionEntry::Scan(TableCollectionEntry *table_entry, Txn *txn_ptr, const ScanState &scan_state, BufferManager *buffer_mgr) {
    Error<NotImplementException>("TableCollectionEntry::Scan", __FILE_NAME__, __LINE__);
    return nullptr;
}

void TableCollectionEntry::CommitAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, const AppendState *append_state_ptr) {
    for (const auto &range : append_state_ptr->append_ranges_) {
        LOG_TRACE(
            Format("Commit, segment: {}, block: {} start: {}, count: {}", range.segment_id_, range.block_id_, range.start_id_, range.row_count_));
        SegmentEntry *segment_ptr = table_entry->segments_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_ptr, range.block_id_, range.start_id_, range.row_count_);
    }
}

void TableCollectionEntry::CommitCreateIndex(TableCollectionEntry *table_entry, const HashMap<u64, SharedPtr<IndexEntry>> &uncommitted_indexes) {
    // FIXME: One index_entry is created for one segment_entry.
    for (const auto &[segment_id, index_entry] : uncommitted_indexes) {
        SegmentEntry::CommitCreateIndex(table_entry->segments_[segment_id].get(), index_entry);
    }
}

void TableCollectionEntry::RollbackAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store) {
    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    Error<NotImplementException>("TableCollectionEntry::RollbackAppend", __FILE_NAME__, __LINE__);
}

void TableCollectionEntry::CommitDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, const DeleteState &delete_state) {
    for(const auto& to_delete_seg_rows: delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment = TableCollectionEntry::GetSegmentByID(table_entry, segment_id);
        if(segment == nullptr) {
            Error<ExecutorException>(Format("Going to commit delete data in non-exist segment: {}", segment_id), __FILE_NAME__, __LINE__);
        }
        const HashMap<u16, Vector<RowID>>& block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::CommitDelete(segment, txn_ptr, block_row_hashmap);
    }
}

UniquePtr<String>
TableCollectionEntry::RollbackDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr) {
    Error<NotImplementException>("TableCollectionEntry::RollbackDelete", __FILE_NAME__, __LINE__);
    return nullptr;
}

UniquePtr<String> TableCollectionEntry::ImportSegment(TableCollectionEntry *table_entry, Txn *txn_ptr, SharedPtr<SegmentEntry> segment) {
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    segment->min_row_ts_ = commit_ts;
    segment->max_row_ts_ = commit_ts;
    for (auto &block_entry : segment->block_entries_) {
        block_entry->min_row_ts_ = commit_ts;
        block_entry->max_row_ts_ = commit_ts;
        // ATTENTION: Do not modify the block_entry checkpoint_ts_
        block_entry->block_version_->created_.emplace_back(commit_ts, block_entry->row_count_);
    }

    UniqueLock<RWMutex> rw_locker(table_entry->rw_locker_);
    table_entry->segments_.emplace(segment->segment_id_, Move(segment));
    return nullptr;
}

SegmentEntry *TableCollectionEntry::GetSegmentByID(const TableCollectionEntry *table_entry, u64 id) {
    auto iter = table_entry->segments_.find(id);
    if (iter != table_entry->segments_.end()) {
        return iter->second.get();
    } else {
        return nullptr;
    }
}

DBEntry *TableCollectionEntry::GetDBEntry(const TableCollectionEntry *table_entry) {
    TableCollectionMeta *table_meta = (TableCollectionMeta *)table_entry->table_collection_meta_;
    return (DBEntry *)table_meta->db_entry_;
}

SharedPtr<BlockIndex> TableCollectionEntry::GetBlockIndex(TableCollectionEntry *table_collection_entry, u64 txn_id, TxnTimeStamp begin_ts) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    SharedLock<RWMutex> rw_locker(table_collection_entry->rw_locker_);
    result->Reserve(table_collection_entry->segments_.size());

    for (const auto &segment_pair : table_collection_entry->segments_) {
        result->Insert(segment_pair.second.get(), begin_ts);
    }

    return result;
}

Json TableCollectionEntry::Serialize(TableCollectionEntry *table_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<SharedPtr<SegmentEntry>> segments;
    Vector<SharedPtr<IndexDef>> indexes{};

    {
        SharedLock<RWMutex> lck(table_entry->rw_locker_);
        json_res["table_entry_dir"] = *table_entry->table_entry_dir_;
        json_res["table_name"] = *table_entry->table_collection_name_;
        json_res["table_entry_type"] = table_entry->table_collection_type_;
        json_res["row_count"] = table_entry->row_count_;
        json_res["begin_ts"] = table_entry->begin_ts_;
        json_res["commit_ts"] = table_entry->commit_ts_.load();
        json_res["txn_id"] = table_entry->txn_id_.load();
        json_res["deleted"] = table_entry->deleted_;
        for (const auto &column_def : table_entry->columns_) {
            Json column_def_json;
            column_def_json["column_type"] = column_def->type()->Serialize();
            column_def_json["column_id"] = column_def->id();
            column_def_json["column_name"] = column_def->name();

            for (const auto &column_constraint : column_def->constraints_) {
                column_def_json["constraints"].emplace_back(column_constraint);
            }

            json_res["column_definition"].emplace_back(column_def_json);
        }
        u64 next_segment_id = table_entry->next_segment_id_;
        json_res["next_segment_id"] = next_segment_id;

        for (const auto &segment_pair : table_entry->segments_) {
            segments.push_back(segment_pair.second);
        }

        for (const auto &[index_name, index_def_meta] : table_entry->indexes_) {
            json_res["indexes"].emplace_back(IndexDefMeta::Serialize(index_def_meta.get()));
        }
    }

    for (const auto &segment : segments) {
        json_res["segments"].emplace_back(SegmentEntry::Serialize(segment.get(), max_commit_ts, is_full_checkpoint));
    }

    for (const auto &index : indexes) {
        json_res["indexes"].emplace_back(index->Serialize());
    }

    return json_res;
}

UniquePtr<TableCollectionEntry>
TableCollectionEntry::Deserialize(const Json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr) {
    SharedPtr<String> table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_entry_json["table_name"]);
    TableCollectionType table_entry_type = table_entry_json["table_entry_type"];
    u64 row_count = table_entry_json["row_count"];

    bool deleted = table_entry_json["deleted"];

    Vector<SharedPtr<ColumnDef>> columns;
    if (!deleted) {
        for (const auto &column_def_json : table_entry_json["column_definition"]) {
            SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
            i64 column_id = column_def_json["column_id"];
            String column_name = column_def_json["column_name"];

            HashSet<ConstraintType> constraints;
            if (column_def_json.contains("constraints")) {
                for (const auto &column_constraint : column_def_json["constraints"]) {
                    ConstraintType constraint = column_constraint;
                    constraints.emplace(constraint);
                }
            }

            SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints);
            columns.emplace_back(column_def);
        }
    }

    u64 txn_id = table_entry_json["txn_id"];
    TxnTimeStamp begin_ts = table_entry_json["begin_ts"];

    UniquePtr<TableCollectionEntry> table_entry =
        MakeUnique<TableCollectionEntry>(table_entry_dir, table_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];
    table_entry->table_entry_dir_ = table_entry_dir;
    if (table_entry_json.contains("segments")) {
        i32 max_segment_id = 0;
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segments_.emplace(segment_entry->segment_id_, segment_entry);
            max_segment_id = Max(max_segment_id, segment_entry->segment_id_);
        }
        table_entry->unsealed_segment_ = table_entry->segments_[max_segment_id].get();
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = deleted;

    if (table_entry_json.contains("indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["indexes"]) {
            UniquePtr<IndexDefMeta> index_def_meta = IndexDefMeta::Deserialize(index_def_meta_json, table_entry.get());
            table_entry->indexes_.emplace(*index_def_meta->index_name_, Move(index_def_meta));
        }
    }

    return table_entry;
}

u64 TableCollectionEntry::GetColumnIdByName(const String &column_name) {
    auto it = name2id_.find(column_name);
    if (it == name2id_.end()) {
        Error<NotImplementException>(Format("No column name: {}", column_name), __FILE_NAME__, __LINE__);
    }
    return it->second;
}

void TableCollectionEntry::MergeFrom(BaseEntry &other) {
    auto table_entry2 = dynamic_cast<TableCollectionEntry *>(&other);
    Assert<StorageException>(table_entry2 != nullptr, "MergeFrom requires the same type of BaseEntry", __FILE_NAME__, __LINE__);
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(*this->table_collection_name_ == *table_entry2->table_collection_name_,
                             "DBEntry::MergeFrom requires table_collection_name_ match",
                             __FILE_NAME__,
                             __LINE__);
    Assert<StorageException>(*this->table_entry_dir_ == *table_entry2->table_entry_dir_,
                             "DBEntry::MergeFrom requires table_entry_dir_ match",
                             __FILE_NAME__,
                             __LINE__);
    Assert<StorageException>(this->table_collection_type_ == table_entry2->table_collection_type_,
                             "DBEntry::MergeFrom requires table_entry_dir_ match",
                             __FILE_NAME__,
                             __LINE__);

    this->next_segment_id_.store(Max(this->next_segment_id_, table_entry2->next_segment_id_));
    u64 max_segment_id = 0;
    for (auto &[seg_id, sgement_entry2] : table_entry2->segments_) {
        max_segment_id = Max(max_segment_id, seg_id);
        auto it = this->segments_.find(seg_id);
        if (it == this->segments_.end()) {
            this->segments_.emplace(seg_id, sgement_entry2);
        } else {
            it->second->MergeFrom(*sgement_entry2.get());
        }
    }
    if (this->unsealed_segment_ == nullptr) {
        this->unsealed_segment_ = this->segments_[max_segment_id].get();
    }
}

} // namespace infinity
