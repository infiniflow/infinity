//
// Created by jinhai on 23-8-18.
//

#include "storage/meta/entry/table_collection_entry.h"
#include "parser/definition/index_def.h"
#include "storage/common/block_index.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/meta/index_def_meta.h"
#include "storage/meta/table_collection_meta.h"
#include "storage/txn/txn_store.h"

namespace infinity {

TableCollectionEntry::TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                           SharedPtr<String> table_collection_name,
                                           const Vector<SharedPtr<ColumnDef>> &columns,
                                           TableCollectionType table_collection_type,
                                           TableCollectionMeta *table_collection_meta,
                                           u64 txn_id,
                                           TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable),
      table_entry_dir_(MakeShared<String>(*db_entry_dir + "/" + *table_collection_name + "/txn_" + std::to_string(txn_id))),
      table_collection_name_(std::move(table_collection_name)), columns_(columns), table_collection_type_(table_collection_type),
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
                                              TxnManager *txn_mgr,
                                              BufferManager *buffer_mgr) {
    table_entry->rw_locker_.lock_shared();
    IndexDefMeta *index_def_meta{nullptr};
    if (table_entry->indexes_.find(index_def->index_name_) != table_entry->indexes_.end()) {
        index_def_meta = table_entry->indexes_[index_def->index_name_].get();
    }
    table_entry->rw_locker_.unlock_shared();

    if (index_def_meta == nullptr) {
        LOG_TRACE("Create new index: {}", *index_def->index_name_);
        auto new_index_def_meta = MakeUnique<IndexDefMeta>(index_def->index_name_, table_entry);
        index_def_meta = new_index_def_meta.get();

        table_entry->rw_locker_.lock();
        table_entry->indexes_[index_def->index_name_] = std::move(new_index_def_meta);
        table_entry->rw_locker_.unlock();

        LOG_TRACE("Add new index entry for {} in new index meta of table_entry {} ", *index_def->index_name_, *table_entry->table_entry_dir_);
    } else {
        LOG_TRACE("Add new index entry for {} in existed index meta of table_entry {}", *index_def->index_name_, *table_entry->table_entry_dir_);
    }
    IndexDef *index_def_ptr = index_def.get();
    EntryResult res = IndexDefMeta::CreateNewEntry(index_def_meta, std::move(index_def), txn_id, begin_ts, txn_mgr);

    return res;
}

EntryResult
TableCollectionEntry::DropIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    NotImplementError("Not implemented.");
    // TODO
}

EntryResult TableCollectionEntry::GetIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts) {
    NotImplementError("Not implemented.");
    // TODO
}

void TableCollectionEntry::RemoveIndexEntry(TableCollectionEntry *table_entry, const SharedPtr<String> &index_name, u64 txn_id, TxnManager *txn_mgr) {
    table_entry->rw_locker_.lock_shared();

    IndexDefMeta *index_def_meta{nullptr};
    if (table_entry->indexes_.find(index_name) != table_entry->indexes_.end()) {
        index_def_meta = table_entry->indexes_[index_name].get();
    }
    table_entry->rw_locker_.unlock_shared();

    LOG_TRACE("Remove index entry: {}", *index_name);
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
    {
        std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation

        // No segment, or unsealed_segment_ is already closed(flushed to disk).
        if (table_entry->unsealed_segment_ == nullptr || table_entry->unsealed_segment_->status_ != DataSegmentStatus::kSegmentOpen) {
            u64 next_segment_id = TableCollectionEntry::GetNextSegmentID(table_entry);
            SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry, table_entry->txn_id_, next_segment_id, buffer_mgr);
            table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
            table_entry->unsealed_segment_ = new_segment.get();
            //            table_entry->unsealed_segment_->Init(this->definition_ptr_->columns(), dir_, buffer_mgr_);
            LOG_TRACE("Add a new segment");
        }
    }

    while (!append_state_ptr->Finished()) {
        SizeT current_row = append_state_ptr->current_count_;

        if (table_entry->unsealed_segment_->AvailableCapacity() == 0 && table_entry->unsealed_segment_->row_capacity_ > 0) {
            // uncommitted_segment is full
            std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation
            // Need double-check
            if (table_entry->unsealed_segment_->AvailableCapacity() == 0) {
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry,
                                                                                        table_entry->txn_id_,
                                                                                        TableCollectionEntry::GetNextSegmentID(table_entry),
                                                                                        buffer_mgr);
                table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
                table_entry->unsealed_segment_ = new_segment.get();
                LOG_TRACE("Add a new segment");
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        SegmentEntry::AppendData(table_entry->unsealed_segment_, txn_ptr, append_state_ptr, buffer_mgr);
        LOG_TRACE("Segment is appended with {} rows", append_state_ptr->current_count_ - current_row);
    }
}

void TableCollectionEntry::CreateIndexFile(TableCollectionEntry *table_entry, void *txn_store, const IndexDef &index_def, BufferManager *buffer_mgr) {
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
                SegmentEntry::CreateIndexEmbedding(segment_entry.get(), index_def, column_id, embedding_info->Dimension(), buffer_mgr, txn_store_ptr);
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

UniquePtr<String>
TableCollectionEntry::Delete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &delete_state, BufferManager *buffer_mgr) {
    NotImplementError("TableCollectionEntry::Delete");
    return nullptr;
}

UniquePtr<String> TableCollectionEntry::InitScan(TableCollectionEntry *table_entry, Txn *txn_ptr, ScanState &scan_state, BufferManager *buffer_mgr) {
    NotImplementError("TableCollectionEntry::InitScan");
    return nullptr;
}

UniquePtr<String>
TableCollectionEntry::Scan(TableCollectionEntry *table_entry, Txn *txn_ptr, const ScanState &scan_state, BufferManager *buffer_mgr) {
    NotImplementError("TableCollectionEntry::Scan");
    return nullptr;
}

void TableCollectionEntry::CommitAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, const AppendState *append_state_ptr) {

    for (const auto &range : append_state_ptr->append_ranges_) {
        LOG_TRACE("Commit, segment: {}, block: {} start: {}, count: {}", range.segment_id_, range.block_id_, range.start_id_, range.row_count_);
        SegmentEntry *segment_ptr = table_entry->segments_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_ptr, range.block_id_, range.start_id_, range.row_count_);
    }
}

void TableCollectionEntry::CommitCreateIndexFile(TableCollectionEntry *table_entry,
                                                 Txn *txn_ptr,
                                                 const HashMap<u64, SharedPtr<IndexEntry>> &uncommitted_indexes) {
    for (const auto &[segment_id, index_entry] : uncommitted_indexes) {
        SegmentEntry::CommitCreateIndexFile(table_entry->segments_[segment_id].get(), txn_ptr, index_entry);
    }
}

void TableCollectionEntry::RollbackAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store) {
    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    NotImplementError("Not implemented");
}

UniquePtr<String>
TableCollectionEntry::CommitDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr) {
    NotImplementError("TableCollectionEntry::CommitDelete");
    return nullptr;
}

UniquePtr<String>
TableCollectionEntry::RollbackDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr) {
    NotImplementError("TableCollectionEntry::RollbackDelete");
    return nullptr;
}

UniquePtr<String> TableCollectionEntry::ImportAppendSegment(TableCollectionEntry *table_entry,
                                                            Txn *txn_ptr,
                                                            SharedPtr<SegmentEntry> segment,
                                                            AppendState &append_state) {
    for (const auto &block_entry : segment->block_entries_) {
        append_state.append_ranges_.emplace_back(segment->segment_id_, block_entry->block_id_, 0, block_entry->row_count_);
    }

    std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_);
    table_entry->segments_.emplace(segment->segment_id_, std::move(segment));
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
    std::shared_lock<RWMutex> rw_locker(table_collection_entry->rw_locker_);
    result->Reserve(table_collection_entry->segments_.size());

    for (const auto &segment_pair : table_collection_entry->segments_) {
        result->Insert(segment_pair.second.get(), txn_id);
    }

    return result;
}

nlohmann::json TableCollectionEntry::Serialize(const TableCollectionEntry *table_entry) {
    nlohmann::json json_res;

    json_res["table_entry_dir"] = *table_entry->table_entry_dir_;
    json_res["table_name"] = *table_entry->table_collection_name_;
    json_res["table_entry_type"] = table_entry->table_collection_type_;
    json_res["row_count"] = table_entry->row_count_;
    json_res["begin_ts"] = table_entry->begin_ts_;
    json_res["commit_ts"] = table_entry->commit_ts_.load();
    json_res["txn_id"] = table_entry->txn_id_.load();
    json_res["deleted"] = table_entry->deleted_;

    for (const auto &column_def : table_entry->columns_) {
        nlohmann::json column_def_json;
        column_def_json["column_type"] = column_def->type()->Serialize();
        column_def_json["column_id"] = column_def->id();
        column_def_json["column_name"] = column_def->name();

        for (const auto &column_constraint : column_def->constraints_) {
            column_def_json["constraints"].emplace_back(column_constraint);
        }

        json_res["column_definition"].emplace_back(column_def_json);
    }

    for (const auto &segment_pair : table_entry->segments_) {
        json_res["segments"].emplace_back(SegmentEntry::Serialize(segment_pair.second.get()));
    }
    u64 next_segment_id = table_entry->next_segment_id_;
    json_res["next_segment_id"] = next_segment_id;

    for (const auto &[index_name, index_def_meta] : table_entry->indexes_) {
        json_res["indexes"].emplace_back(IndexDefMeta::Serialize(index_def_meta.get()));
    }

    return json_res;
}

UniquePtr<TableCollectionEntry>
TableCollectionEntry::Deserialize(const nlohmann::json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr) {
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
    if (table_entry_json.contains("segments")) {
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segments_.emplace(segment_entry->segment_id_, segment_entry);
        }
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = deleted;

    if (table_entry_json.contains("indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["indexes"]) {
            UniquePtr<IndexDefMeta> index_def_meta = IndexDefMeta::Deserialize(index_def_meta_json, table_entry.get());
            table_entry->indexes_.emplace(index_def_meta->index_name_, std::move(index_def_meta));
        }
    }

    return table_entry;
}

u64 TableCollectionEntry::GetColumnIdByName(const String &column_name) {
    auto it = name2id_.find(column_name);
    if (it == name2id_.end()) {
        StorageError(fmt::format("No column name: {}", column_name))
    }
    return it->second;
}

} // namespace infinity