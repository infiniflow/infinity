//
// Created by jinhai on 23-8-11.
//

#include "storage/meta/entry/segment_entry.h"
#include "common/utility/defer_op.h"
#include "common/utility/random.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "parser/definition/ivfflat_index_def.h"
#include "storage/buffer/faiss_index_ptr.h"
#include "storage/io/local_file_system.h"
#include "storage/meta/entry/data_access_state.h"
#include "storage/meta/entry/index_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "storage/txn/txn.h"
#include "storage/txn/txn_store.h"

namespace infinity {

nlohmann::json SegmentVersion::Serialize(SegmentVersion *segment_version){NotImplementError("Segment version serialize")}

UniquePtr<SegmentVersion> SegmentVersion::Deserialize(const nlohmann::json &table_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr){
    NotImplementError("Segment version deserialize")}

SharedPtr<SegmentEntry> SegmentEntry::MakeNewSegmentEntry(const TableCollectionEntry *table_entry, u64 segment_id, BufferManager *buffer_mgr) {

    SharedPtr<SegmentEntry> new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_count_ = 0;
    new_entry->row_capacity_ = DEFAULT_SEGMENT_CAPACITY;
    new_entry->segment_id_ = segment_id;
    new_entry->min_row_ts_ = 0;
    new_entry->max_row_ts_ = 0;

    const auto *table_ptr = (const TableCollectionEntry *)table_entry;
    new_entry->column_count_ = table_ptr->columns_.size();

    new_entry->segment_dir_ = SegmentEntry::DetermineSegFilename(*table_entry->table_entry_dir_, segment_id);
    if (new_entry->block_entries_.empty()) {
        new_entry->block_entries_.emplace_back(
            MakeUnique<BlockEntry>(new_entry.get(), new_entry->block_entries_.size(), 0, new_entry->column_count_, buffer_mgr));
    }
    return new_entry;
}

int SegmentEntry::Room(SegmentEntry *segment_entry) {
    std::shared_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
    return segment_entry->row_capacity_ - segment_entry->row_count_;
}

int SegmentEntry::AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr) {
    std::unique_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
    if (segment_entry->row_capacity_ - segment_entry->row_count_ <= 0)
        return 0;
    SizeT start_row = segment_entry->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    int total_copied{0};

    while (append_state_ptr->current_block_ < append_block_count && segment_entry->row_count_ < segment_entry->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        i16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0 && segment_entry->row_count_ < segment_entry->row_capacity_) {
            // Append to_append_rows into block
            BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
            if (BlockEntry::Room(last_block_entry) <= 0) {
                segment_entry->block_entries_.emplace_back(
                    MakeUnique<BlockEntry>(segment_entry, segment_entry->block_entries_.size(), 0, segment_entry->column_count_, buffer_mgr));
                last_block_entry = segment_entry->block_entries_.back().get();
            }

            u64 range_segment_id = segment_entry->segment_id_;
            i16 range_block_id = last_block_entry->block_id_;
            i16 range_block_start_row = last_block_entry->row_count_;

            i16 actual_appended =
                BlockEntry::AppendData(last_block_entry, txn_ptr, input_block, append_state_ptr->current_block_offset_, to_append_rows);

            append_state_ptr->append_ranges_.emplace_back(range_segment_id, range_block_id, range_block_start_row, actual_appended);

            total_copied += actual_appended;
            to_append_rows -= actual_appended;
            append_state_ptr->current_count_ += actual_appended;
            segment_entry->row_count_ += actual_appended;
        }
        if (to_append_rows <= 0) {
            append_state_ptr->current_block_++;
            append_state_ptr->current_block_offset_ = 0;
        }
    }
    return total_copied;
}

void SegmentEntry::DeleteData(SegmentEntry *segment_entry, Txn *txn_ptr, const Vector<RowID> &rows, BufferManager *buffer_mgr) {
    std::unique_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
    for (RowID row_id : rows) {
        StorageAssert(row_id.segment_id_ == segment_entry->segment_id_, "The segment doesn't contain the given rows.");
        BlockEntry *block_entry = SegmentEntry::GetBlockEntryByID(segment_entry, row_id.block_id_);
        StorageAssert(block_entry != nullptr, "The segment doesn't contain the given block.");
        BlockEntry::DeleteData(block_entry, txn_ptr, row_id.block_offset_);
    }
}

void SegmentEntry::CreateIndexScalar(SegmentEntry *segment_entry,
                                     Txn *txn_ptr,
                                     const IndexDef &index_def,
                                     u64 column_id,
                                     BufferManager *buffer_mgr,
                                     TxnTableStore *txn_store){NotImplementError("Not implemented")}

SharedPtr<IndexEntry> SegmentEntry::CreateIndexEmbedding(SegmentEntry *segment_entry,
                                                         const IndexDef &index_def,
                                                         u64 column_id,
                                                         int dimension,
                                                         TxnTimeStamp create_ts,
                                                         BufferManager *buffer_mgr,
                                                         TxnTableStore *txn_store) {
    Vector<SharedPtr<IndexEntry>> index_entries;
    switch (index_def.method_type_) {
        case IndexMethod::kIVFFlat: {
            auto ivfflat_index_def = static_cast<const IVFFlatIndexDef &>(index_def);
            faiss::IndexFlat *quantizer = nullptr;
            faiss::MetricType metric = faiss::MetricType::METRIC_L2;
            switch (ivfflat_index_def.metric_type_) {
                case MetricType::kMerticL2: {
                    quantizer = new faiss::IndexFlatL2(dimension);
                    metric = faiss::MetricType::METRIC_L2;
                    break;
                }
                case MetricType::kMerticInnerProduct: {
                    quantizer = new faiss::IndexFlatIP(dimension);
                    metric = faiss::MetricType::METRIC_INNER_PRODUCT;
                    break;
                }
                case MetricType::kInvalid: {
                    StorageError("Metric type is invalid");
                }
                default: {
                    NotImplementError("Not implemented");
                }
            }
            auto index = new faiss::IndexIVFFlat(quantizer, dimension, ivfflat_index_def.centroids_count_, metric);
            for (const auto &block_entry : segment_entry->block_entries_) {
                auto block_column_entry = block_entry->columns_[column_id].get();
                ObjectHandle object_handle(block_column_entry->buffer_handle_);
                auto block_data_ptr = reinterpret_cast<float *>(object_handle.GetData());
                SizeT block_row_cnt = block_entry->row_count_;
                try {
                    index->train(block_row_cnt, block_data_ptr);
                } catch (std::exception &e) {
                    StorageException("Train index failed: {}", e.what());
                }
            }

            auto index_entry =
                IndexEntry::NewIndexEntry(segment_entry, index_def.index_name_, create_ts, buffer_mgr, new FaissIndexPtr(index, quantizer));

            txn_store->CreateIndexFile(segment_entry->segment_id_, std::move(index_entry));
            return index_entry;
        }
        case IndexMethod::kInvalid: {
            StorageError("Index method type is invalid");
        }
        default: {
            NotImplementError("Not implemented");
        }
    }
}

void SegmentEntry::CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, i16 block_id, i16 start_pos, i16 row_count) {
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    SharedPtr<BlockEntry> block_entry;
    {
        std::unique_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
        if (segment_entry->min_row_ts_ == 0) {
            segment_entry->min_row_ts_ = commit_ts;
        }
        segment_entry->max_row_ts_ = std::max(segment_entry->max_row_ts_, commit_ts);
        block_entry = segment_entry->block_entries_[block_id];
    }
    BlockEntry::CommitAppend(block_entry.get(), txn_ptr);
}

void SegmentEntry::CommitCreateIndex(SegmentEntry *segment_entry, SharedPtr<IndexEntry> index_entry) {
    segment_entry->index_entry_map_.emplace(*index_entry->index_name_, std::move(index_entry));
}

void SegmentEntry::CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, u64 start_pos, u64 row_count) {
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    std::unique_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
    if (segment_entry->min_row_ts_ == 0) {
        segment_entry->min_row_ts_ = commit_ts;
        // for (const auto &[index_name, index_entry] : segment_entry->index_entry_map_) {
        //     IndexEntry::Flush(index_entry.get(), commit_ts);
        // }
    }
    segment_entry->max_row_ts_ = std::max(segment_entry->max_row_ts_, commit_ts);
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

nlohmann::json SegmentEntry::Serialize(SegmentEntry *segment_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<BlockEntry *> block_entries;
    {
        std::shared_lock<std::shared_mutex> lck(segment_entry->rw_locker_);
        json_res["segment_dir"] = *segment_entry->segment_dir_;
        json_res["row_capacity"] = segment_entry->row_capacity_;
        json_res["segment_id"] = segment_entry->segment_id_;
        json_res["column_count"] = segment_entry->column_count_;
        json_res["min_row_ts"] = segment_entry->min_row_ts_;
        json_res["max_row_ts"] = segment_entry->max_row_ts_;
        json_res["deleted"] = segment_entry->deleted_;
        json_res["row_count"] = segment_entry->row_count_;
        for (auto &block_entry : segment_entry->block_entries_) {
            if (max_commit_ts > block_entry->checkpoint_ts_)
                block_entries.push_back((BlockEntry *)block_entry.get());
        }
    }
    for (BlockEntry *block_entry : block_entries) {
        BlockEntry::Flush(block_entry, max_commit_ts);
        if (!is_full_checkpoint && block_entry->checkpoint_ts_ != max_commit_ts)
            continue;
        json_res["block_entries"].emplace_back(BlockEntry::Serialize(block_entry, max_commit_ts));
    }
    for (const auto &[_index_name, index_entry] : segment_entry->index_entry_map_) {
        IndexEntry::Flush(index_entry.get(), max_commit_ts);
        json_res["index_entries"].emplace_back(IndexEntry::Serialize(index_entry.get()));
    }
    return json_res;
}

SharedPtr<SegmentEntry>
SegmentEntry::Deserialize(const nlohmann::json &segment_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr) {
    SharedPtr<SegmentEntry> segment_entry = MakeShared<SegmentEntry>(table_entry);

    segment_entry->segment_dir_ = MakeShared<String>(segment_entry_json["segment_dir"]);
    segment_entry->row_capacity_ = segment_entry_json["row_capacity"];

    segment_entry->segment_id_ = segment_entry_json["segment_id"];
    segment_entry->column_count_ = segment_entry_json["column_count"];

    segment_entry->min_row_ts_ = segment_entry_json["min_row_ts"];
    segment_entry->max_row_ts_ = segment_entry_json["max_row_ts"];
    segment_entry->deleted_ = segment_entry_json["deleted"];
    segment_entry->row_count_ = segment_entry_json["row_count"];

    if (segment_entry_json.contains("block_entries")) {
        SizeT block_count = segment_entry_json["block_entries"].size();
        segment_entry->block_entries_.reserve(block_count);
        for (const auto &block_json : segment_entry_json["block_entries"]) {
            UniquePtr<BlockEntry> block_entry = BlockEntry::Deserialize(block_json, segment_entry.get(), buffer_mgr);
            segment_entry->block_entries_.emplace_back(std::move(block_entry));
        }
    }
    if (segment_entry_json.contains("index_entries")) {
        for (const auto &index_json : segment_entry_json["index_entries"]) {
            SharedPtr<IndexEntry> index_entry = IndexEntry::Deserialize(index_json, segment_entry.get(), buffer_mgr);
            segment_entry->index_entry_map_.emplace(*index_entry->index_name_, std::move(index_entry));
        }
    }

    return segment_entry;
}

SharedPtr<String> SegmentEntry::DetermineSegFilename(const String &parent_dir, u64 seg_id) {
    u32 seed = time(nullptr);
    LocalFileSystem fs;
    SharedPtr<String> segment_dir;
    do {
        segment_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_SEGMENT_NAME_LEN, seed) + "_seg_" + std::to_string(seg_id));
    } while (!fs.CreateDirectoryNoExp(*segment_dir));
    return segment_dir;
}

} // namespace infinity
