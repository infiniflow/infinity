//
// Created by jinhai on 23-8-18.
//

#include "table_entry.h"
#include "storage/txn/txn_store.h"
#include "storage/txn/txn.h"


namespace infinity {

void
TableEntry::Append(TableEntry* table_entry, Txn* txn_ptr, void* txn_store, BufferManager* buffer_mgr) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    Txn* transaction_ptr = (Txn*)txn_ptr;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    if(table_entry->unsealed_segment_ == nullptr) {
        // No segment at all.
        std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation

        // Need double-check
        if(table_entry->unsealed_segment_ == nullptr) {
            u64 next_segment_id = TableEntry::GetNextSegmentID(table_entry);
            SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry,
                                                                                    table_entry->txn_id_,
                                                                                    next_segment_id,
                                                                                    buffer_mgr);
            table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
            table_entry->unsealed_segment_ = new_segment.get();
//            table_entry->unsealed_segment_->Init(this->definition_ptr_->columns(), dir_, buffer_mgr_);
            LOG_TRACE("Add a new segment");
        }
    }

    while(!append_state_ptr->Finished()) {
        SizeT current_row = append_state_ptr->current_count_;

        if(table_entry->unsealed_segment_->AvailableCapacity() == 0) {
            // uncommitted_segment is full
            std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation
            // Need double-check
            if(table_entry->unsealed_segment_->AvailableCapacity() == 0) {
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(table_entry,
                                                                                        table_entry->txn_id_,
                                                                                        TableEntry::GetNextSegmentID(table_entry),
                                                                                        buffer_mgr);
                table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
                table_entry->unsealed_segment_ = new_segment.get();
                LOG_TRACE("Add a new segment");
            }
        }

        SegmentEntry::AppendData(table_entry->unsealed_segment_, txn_ptr, append_state_ptr, buffer_mgr);
        LOG_TRACE("Segment is appended with {} rows", append_state_ptr->current_count_ - current_row);
    }
}

UniquePtr<String>
TableEntry::Delete(TableEntry* table_entry, Txn* txn_ptr, DeleteState& delete_state, BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::InitScan(TableEntry* table_entry, Txn* txn_ptr, ScanState& scan_state, BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::Scan(TableEntry* table_entry, Txn* txn_ptr, ScanState scan_state, BufferManager* buffer_mgr) {

}

void
TableEntry::CommitAppend(TableEntry* table_entry,
                         Txn* txn_ptr,
                         const AppendState* append_state_ptr,
                         BufferManager* buffer_mgr) {
    HashSet<u64> new_segments;
    for(const auto& range: append_state_ptr->append_ranges_) {
        LOG_TRACE("Commit, segment: {}, start: {}, count: {}", range.segment_id_, range.start_pos_, range.row_count_);
        SegmentEntry* segment_ptr = table_entry->segments_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_ptr, range.start_pos_, range.row_count_);
        new_segments.insert(range.segment_id_);
    }

    // FIXME: now all commit will trigger flush
    for(u64 segment_id: new_segments) {
        SegmentEntry::PrepareFlush(table_entry->segments_[segment_id].get());
        SegmentEntry::Flush(table_entry->segments_[segment_id].get());
    }
}

void
TableEntry::RollbackAppend(TableEntry* table_entry, Txn* txn_ptr, void* txn_store) {
    TxnTableStore* txn_store_ptr = (TxnTableStore*)txn_store;
    AppendState* append_state_ptr = txn_store_ptr->append_state_.get();
    NotImplementError("Not implemented");
}

UniquePtr<String>
TableEntry::CommitDelete(TableEntry* table_entry,
                         Txn* txn_ptr,
                         DeleteState& append_state,
                         BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::RollbackDelete(TableEntry* table_entry,
                           Txn* txn_ptr,
                           DeleteState& append_state,
                           BufferManager* buffer_mgr) {

}

UniquePtr<String>
TableEntry::ImportAppendSegment(TableEntry* table_entry,
                                Txn* txn_ptr,
                                SharedPtr<SegmentEntry> segment,
                                AppendState& append_state,
                                BufferManager* buffer_mgr) {
    append_state.append_ranges_.emplace_back(segment->segment_id_, 0, segment->current_row_);
    std::unique_lock<RWMutex> rw_locker(table_entry->rw_locker_);
    table_entry->segments_.emplace(segment->segment_id_, std::move(segment));
    return nullptr;
}

DBEntry*
TableEntry::GetDBEntry(const TableEntry* table_entry) {
    TableMeta* table_meta = (TableMeta*)table_entry->table_meta_;
    return (DBEntry*)table_meta->db_entry_;
}

nlohmann::json
TableEntry::Serialize(const TableEntry* table_entry) {
    nlohmann::json json_res;

    json_res["base_dir"] = *table_entry->base_dir_;
    json_res["table_name"] = *table_entry->table_name_;
    json_res["row_count"] = table_entry->row_count_;
    json_res["begin_ts"] = table_entry->begin_ts_;
    json_res["commit_ts"] = table_entry->commit_ts_.load();
    json_res["txn_id"] = table_entry->txn_id_.load();
    json_res["deleted"] = table_entry->deleted_;

    for(const auto& column_def: table_entry->columns_) {
        nlohmann::json column_def_json;
        column_def_json["column_type"] = column_def->type()->Serialize();
        column_def_json["column_id"] = column_def->id();
        column_def_json["column_name"] = column_def->name();

        for(const auto& column_constraint: column_def->constraints_) {
            column_def_json["constraints"].emplace_back(column_constraint);
        }

        json_res["column_definition"].emplace_back(column_def_json);
    }

    for(const auto& segment_pair: table_entry->segments_) {
        json_res["segments"].emplace_back(SegmentEntry::Serialize(segment_pair.second.get()));
    }
    u64 next_segment_id = table_entry->next_segment_id_;
    json_res["next_segment_id"] = next_segment_id;

    return json_res;
}

UniquePtr<TableEntry>
TableEntry::Deserialize(const nlohmann::json& table_entry_json, TableMeta* table_meta, BufferManager* buffer_mgr) {
    nlohmann::json json_res;

    SharedPtr<String> base_dir = MakeShared<String>(table_entry_json["base_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_entry_json["table_name"]);
    u64 row_count = table_entry_json["row_count"];

    Vector<SharedPtr<ColumnDef>> columns;
    for(const auto& column_def_json: table_entry_json["column_definition"]) {
        HashSet<ConstraintType> constraints;
        for(const auto& column_constraint: column_def_json["constraints"]) {
//            ConstraintType constraint = column_constraint;
            constraints.emplace(column_constraint);
        }

        SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
        i64 column_id = column_def_json["column_id"];
        String column_name = column_def_json["column_name"];

        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints);
        columns.emplace_back(column_def);
    }

    u64 txn_id = table_entry_json["txn_id"];
    TxnTimeStamp begin_ts = table_entry_json["begin_ts"];

    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(base_dir, table_name, columns, table_meta, txn_id, begin_ts);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];
    for(const auto& segment_json: table_entry_json["segments"]) {
        SharedPtr<SegmentEntry> segment_entry
            = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
        table_entry->segments_.emplace(segment_entry->segment_id_, segment_entry);
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = table_entry_json["deleted"];

    return table_entry;
}

}