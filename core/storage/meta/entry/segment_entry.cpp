//
// Created by jinhai on 23-8-11.
//

module;

import stl;
import third_party;
import block_entry;
import buffer_manager;
import default_values;
import data_block;
import txn;
import data_access_state;
import index_def;
import table_collection_entry;
import base_entry;

module segment_entry;

namespace infinity {

SegmentEntry::SegmentEntry(const TableCollectionEntry *table_entry) : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

SharedPtr<SegmentEntry> SegmentEntry::MakeNewSegmentEntry(const TableCollectionEntry *table_entry,
                                                          u64 txn_id,
                                                          u64 segment_id,
                                                          BufferManager *buffer_mgr,
                                                          SizeT segment_row) {

    SharedPtr<SegmentEntry> new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_capacity_ = segment_row;
    new_entry->current_row_ = 0;
    new_entry->segment_id_ = segment_id;
    new_entry->status_ = DataSegmentStatus::kSegmentOpen;
    //    new_entry->segment_version_ = MakeUnique<SegmentVersion>(segment_row);
    new_entry->min_row_ts_ = MAX_TXN_ID;
    new_entry->max_row_ts_ = MAX_TXN_ID;

    const auto *table_ptr = (const TableCollectionEntry *)table_entry;
    new_entry->column_count_ = table_ptr->columns_.size();

    new_entry->segment_dir_ = SegmentEntry::DetermineSegFilename(*table_entry->table_entry_dir_, segment_id);
    if (new_entry->block_entries_.empty()) {
        new_entry->block_entries_.emplace_back(
            MakeUnique<BlockEntry>(new_entry.get(), new_entry->block_entries_.size(), new_entry->column_count_, 0, buffer_mgr));
    }
    // new_entry->finish_shuffle = false;

    //    const Vector<SharedPtr<ColumnDef>>& columns = table_ptr->columns_;
    //    new_entry->columns_.reserve(columns.size());
    //    for(const auto& column_def: columns) {
    //        new_entry->columns_.emplace_back(SegmentColumnEntry::MakeNewColumnDataEntry(new_entry.get(),
    //                                                                                    column_def->id(),
    //                                                                                    segment_row,
    //                                                                                    column_def->type(),
    //                                                                                    buffer_mgr
    //        ));
    //    }
    return new_entry;
}

void SegmentEntry::AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr) {
#if 0
    if (segment_entry->status_ != DataSegmentStatus::kSegmentOpen) {
        StorageError("Attempt to append data into Non-Open status data segment");
    }

    segment_entry->rw_locker_.lock();
    DeferFn defer_fn([&]() { segment_entry->rw_locker_.unlock(); });

    SizeT start_row = segment_entry->current_row_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    SizeT total_copied{0};

    bool segment_full = false;
    while (append_state_ptr->current_block_ < append_block_count) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        i16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0) {
            // Append to_append_rows into block
            BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
            ;
            if (BlockEntry::IsFull(last_block_entry)) {
                segment_entry->block_entries_.emplace_back(
                    MakeUnique<BlockEntry>(segment_entry, segment_entry->block_entries_.size(), segment_entry->column_count_, start_row, buffer_mgr));
                last_block_entry = segment_entry->block_entries_.back().get();
            }

            u64 range_segment_id = segment_entry->segment_id_;
            i16 range_block_id = last_block_entry->block_id_;
            i16 range_block_start_row = last_block_entry->start_row_;

            i16 actual_appended =
                BlockEntry::AppendData(last_block_entry, txn_ptr, input_block, append_state_ptr->current_block_offset_, to_append_rows);

            i16 range_block_row_count = actual_appended;
            append_state_ptr->append_ranges_.emplace_back(range_segment_id, range_block_id, range_block_start_row, range_block_row_count);

            to_append_rows -= actual_appended;
            segment_entry->current_row_ += actual_appended;
            append_state_ptr->current_count_ += actual_appended;

            if (segment_entry->current_row_ == segment_entry->row_capacity_) {
                // segment is full
                append_state_ptr->current_block_offset_ += actual_appended;
                return;
            }
        }

        append_state_ptr->current_block_++;
        append_state_ptr->current_block_offset_ = 0;
    }
#endif
}

void SegmentEntry::CreateIndexScalar(SegmentEntry *segment_entry, Txn *txn_ptr, const IndexDef &index_def, u64 column_id) {
#if 0
    NotImplementError("Not implemented")
#endif
}

void SegmentEntry::CreateIndexEmbedding(SegmentEntry *segment_entry, Txn *txn_ptr, const IndexDef &index_def, u64 column_id, int dimension) {
#if 0
    switch (index_def.method_type()) {
        case IndexMethod::kIVFFlat: {
            auto ivfflat_index_def = static_cast<const IVFFlatIndexDef &>(index_def);
            faiss::IndexFlat *quantizer_ptr = nullptr;
            switch (ivfflat_index_def.metric_type()) {
                case MetricType::kMerticL2: {
                    quantizer_ptr = new faiss::IndexFlatL2(dimension);
                    break;
                }
                case MetricType::kMerticInnerProduct: {
                    quantizer_ptr = new faiss::IndexFlatIP(dimension);
                    break;
                }
                case MetricType::kInvalid: {
                    StorageException("Metric type is not supported");
                }
                default: {
                    NotImplementException("Not implemented");
                }
            }
            // faiss::IndexFlatL2 quantizer(dimension);
            auto quantizer = UniquePtr<faiss::IndexFlat>(quantizer_ptr);
            auto index = MakeUnique<faiss::IndexIVFFlat>(quantizer.get(), dimension, ivfflat_index_def.centroids_count());
            for (const auto &block_entry : segment_entry->block_entries_) {
                auto block_column_entry = block_entry->columns_[column_id].get();
                CommonObjectHandle object_handle(block_column_entry->buffer_handle_);
                auto block_data_ptr = (float *)object_handle.GetData();
                SizeT block_row_cnt = block_entry->row_count_;
                try {
                    index->train(block_row_cnt, block_data_ptr);
                } catch (std::exception &e) {
                    StorageException("Train index failed: {}", e.what());
                }
            }

            auto index_filename = DetermineIndexFilename(*segment_entry->segment_dir_, ivfflat_index_def.index_name(), segment_entry->segment_id_);
            // TODO shenyushi: change meta data
            // segment_entry->index_name_map_.emplace(column_id, index_filename);
            faiss::write_index(index.get(), index_filename.get()->data());
            return;
        }
        default: {
            NotImplementException("Index method type is not supported");
        }
    }
#endif
}

void SegmentEntry::CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, i16 block_id, i16 start_pos, i16 row_count) {
#if 0
    u64 committing_txn_id = txn_ptr->TxnID();

    if (segment_entry->min_row_ts_ == MAX_TXN_ID) {
        segment_entry->min_row_ts_ = committing_txn_id;
    }

    if (committing_txn_id < segment_entry->min_row_ts_) {
        segment_entry->min_row_ts_ = committing_txn_id;
    }

    if (segment_entry->max_row_ts_ == MAX_TXN_ID) {
        segment_entry->max_row_ts_ = committing_txn_id;
    }

    if (committing_txn_id > segment_entry->max_row_ts_) {
        segment_entry->max_row_ts_ = committing_txn_id;
    }

    BlockEntry::CommitAppend(segment_entry->block_entries_[block_id].get(), committing_txn_id);
#endif
}

void SegmentEntry::CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, u64 start_pos, u64 row_count) {
}

bool SegmentEntry::PrepareFlush(SegmentEntry *segment_entry) {
#if 0
    DataSegmentStatus expected = DataSegmentStatus::kSegmentOpen;
    return segment_entry->status_.compare_exchange_strong(expected, DataSegmentStatus::kSegmentFlushing, std::memory_order_seq_cst);
#endif
}

bool SegmentEntry::Flush(SegmentEntry *segment_entry) {
#if 0
    LOG_TRACE("DataSegment: {} is being flushed", segment_entry->segment_id_);
    for (const auto &block_entry : segment_entry->block_entries_) {

        bool flushed = BlockEntry::Flush(block_entry.get());
        if (!flushed) {
            LOG_WARN("Block is expected as flushing status");
            // TODO(xuanwei): delta checkpoint
            continue;
        }
        LOG_TRACE("Segment: {}, Block: {} is flushed", segment_entry->segment_id_, block_entry->block_id_);
    }

    auto expected = DataSegmentStatus::kSegmentFlushing;
    if (!segment_entry->status_.compare_exchange_strong(expected, DataSegmentStatus::kSegmentClosed, std::memory_order_seq_cst)) {
        LOG_WARN("Data segment is expected as flushing status");
        return false;
    }
    LOG_TRACE("DataSegment: {} is being flushed", segment_entry->segment_id_);

    return true;
#endif
}

u64 SegmentEntry::GetBlockIDByRowID(SizeT row_id) {
    if (row_id == 0) {
        return 0;
    }

    if (row_id % DEFAULT_VECTOR_SIZE == 0) {
        return row_id / DEFAULT_VECTOR_SIZE - 1;
    } else {
        return row_id / DEFAULT_VECTOR_SIZE;
    }
}

i16 SegmentEntry::GetMaxBlockID(const SegmentEntry *segment_entry) { return segment_entry->block_entries_.size(); }

BlockEntry *SegmentEntry::GetBlockEntryByID(const SegmentEntry *segment_entry, u64 block_id) {
    if (block_id < segment_entry->block_entries_.size()) {
        return segment_entry->block_entries_[block_id].get();
    } else {
        return nullptr;
    }
}

Json SegmentEntry::Serialize(const SegmentEntry *segment_entry) {
#if 0
    Json json_res;

    json_res["segment_dir"] = *segment_entry->segment_dir_;
    json_res["row_capacity"] = segment_entry->row_capacity_;
    i64 status_value = segment_entry->status_;
    json_res["status"] = status_value;
    json_res["segment_id"] = segment_entry->segment_id_;
    json_res["column_count"] = segment_entry->column_count_;

    json_res["min_row_ts"] = segment_entry->min_row_ts_.load();
    json_res["max_row_ts"] = segment_entry->max_row_ts_.load();

    json_res["begin_ts"] = segment_entry->begin_ts_;
    json_res["commit_ts"] = segment_entry->commit_ts_.load();
    json_res["txn_id"] = segment_entry->txn_id_.load();
    json_res["deleted"] = segment_entry->deleted_;
    json_res["current_row"] = segment_entry->current_row_;

    for (const auto &block_entry : segment_entry->block_entries_) {
        json_res["block_entries"].emplace_back(BlockEntry::Serialize(block_entry.get()));
    }
    return json_res;
#endif
}

SharedPtr<SegmentEntry>
SegmentEntry::Deserialize(const Json &segment_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry);

    segment_entry->segment_dir_ = MakeShared<String>(segment_entry_json["segment_dir"]);
    segment_entry->row_capacity_ = segment_entry_json["row_capacity"];

    i64 status_value = segment_entry_json["status"];
    segment_entry->status_ = static_cast<DataSegmentStatus>(status_value);
    segment_entry->segment_id_ = segment_entry_json["segment_id"];
    segment_entry->column_count_ = segment_entry_json["column_count"];

    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->max_row_ts_ = segment_entry_json["max_row_ts"];
    segment_entry->begin_ts_ = segment_entry_json["begin_ts"];
    segment_entry->commit_ts_ = segment_entry_json["commit_ts"];
    segment_entry->txn_id_ = segment_entry_json["txn_id"];
    segment_entry->deleted_ = segment_entry_json["deleted"];
    segment_entry->current_row_ = segment_entry_json["current_row"];

    if (segment_entry_json.contains("block_entries")) {
        SizeT block_count = segment_entry_json["block_entries"].size();
        segment_entry->block_entries_.reserve(block_count);
        for (const auto &block_json : segment_entry_json["block_entries"]) {
            UniquePtr<BlockEntry> block_entry = BlockEntry::Deserialize(block_json, segment_entry.get(), buffer_mgr);

            segment_entry->block_entries_.emplace_back(Move(block_entry));
        }
    }

    return segment_entry;
}

SharedPtr<String> SegmentEntry::DetermineSegFilename(const String &parent_dir, u64 seg_id) {
#if 0
    u32 seed = time(nullptr);
    LocalFileSystem fs;
    SharedPtr<String> segment_dir;
    do {
        segment_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_SEGMENT_NAME_LEN, seed) + "_seg_" + ToStr(seg_id));
    } while (!fs.CreateDirectoryNoExp(*segment_dir));
    return segment_dir;
#endif
}

SharedPtr<String> SegmentEntry::DetermineIndexFilename(const String &parent_dir, const String &index_name, u64 seg_id) {
#if 0
    // TODO shenyushi 3
    return MakeShared<String>(parent_dir + '/' + index_name + '_' + ToStr(seg_id));
#endif
}
} // namespace infinity
