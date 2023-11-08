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

#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/index_io.h"

import stl;
import third_party;
import block_entry;
import buffer_manager;
import default_values;
import data_block;
import txn;
import data_access_state;
import index_def;
import index_data;
import table_collection_entry;
import base_entry;

import infinity_exception;
import defer_op;
import ivfflat_index_def;
import buffer_handle;
import faiss_index_file_worker;
import ann_ivf_index_file_worker;
import ann_ivf_flat;
import logger;
import local_file_system;
import random;
import parser;
import index_entry;
import txn_store;

module segment_entry;

namespace infinity {

SegmentEntry::SegmentEntry(const TableCollectionEntry *table_entry) : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

SharedPtr<SegmentEntry> SegmentEntry::MakeNewSegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, BufferManager *buffer_mgr) {
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

SharedPtr<SegmentEntry>
SegmentEntry::MakeReplaySegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, SharedPtr<String> segment_dir, TxnTimeStamp commit_ts) {

    auto new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_capacity_ = DEFAULT_SEGMENT_CAPACITY;
    new_entry->segment_id_ = segment_id;
    new_entry->min_row_ts_ = commit_ts;
    new_entry->max_row_ts_ = commit_ts;

    const auto *table_ptr = (const TableCollectionEntry *)table_entry;
    new_entry->column_count_ = table_ptr->columns_.size();
    new_entry->segment_dir_ = Move(segment_dir);
    return new_entry;
}

int SegmentEntry::Room(SegmentEntry *segment_entry) {
    SharedLock<RWMutex> lck(segment_entry->rw_locker_);
    return segment_entry->row_capacity_ - segment_entry->row_count_;
}

u64 SegmentEntry::AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr) {
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);
    if (segment_entry->row_capacity_ - segment_entry->row_count_ <= 0)
        return 0;
    SizeT start_row = segment_entry->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};

    while (append_state_ptr->current_block_ < append_block_count && segment_entry->row_count_ < segment_entry->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        u16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0 && segment_entry->row_count_ < segment_entry->row_capacity_) {
            // Append to_append_rows into block
            BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
            if (BlockEntry::Room(last_block_entry) <= 0) {
                segment_entry->block_entries_.emplace_back(
                    MakeUnique<BlockEntry>(segment_entry, segment_entry->block_entries_.size(), 0, segment_entry->column_count_, buffer_mgr));
                last_block_entry = segment_entry->block_entries_.back().get();
            }

            u32 range_segment_id = segment_entry->segment_id_;
            u16 range_block_id = last_block_entry->block_id_;
            u16 range_block_start_row = last_block_entry->row_count_;

            u16 actual_appended =
                BlockEntry::AppendData(last_block_entry, txn_ptr, input_block, append_state_ptr->current_block_offset_, to_append_rows, buffer_mgr);
            if (to_append_rows < actual_appended) {
                Error<StorageException>(Format("Attempt to append rows: {}, but rows: {} are appended", to_append_rows, actual_appended));
            }

            append_state_ptr->append_ranges_.emplace_back(range_segment_id, range_block_id, range_block_start_row, actual_appended);

            total_copied += actual_appended;
            to_append_rows -= actual_appended;
            append_state_ptr->current_count_ += actual_appended;
            segment_entry->row_count_ += actual_appended;
        }
        if (to_append_rows == 0) {
            append_state_ptr->current_block_++;
            append_state_ptr->current_block_offset_ = 0;
        }
    }
    return total_copied;
}

void SegmentEntry::DeleteData(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>> &block_row_hashmap) {
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);

    for (const auto &row_hash_map : block_row_hashmap) {
        u16 block_id = row_hash_map.first;
        BlockEntry *block_entry = SegmentEntry::GetBlockEntryByID(segment_entry, block_id);
        if (block_entry == nullptr) {
            Error<StorageException>(Format("The segment doesn't contain the given block: {}.", block_id));
        }

        const Vector<RowID> &rows = row_hash_map.second;
        BlockEntry::DeleteData(block_entry, txn_ptr, rows);
    }
}

void SegmentEntry::CreateIndexScalar(SegmentEntry *segment_entry,
                                     Txn *txn_ptr,
                                     const IndexDef &index_def,
                                     u64 column_id,
                                     BufferManager *buffer_mgr,
                                     TxnTableStore *txn_store) {
    Error<NotImplementException>("Not implemented");
}

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
            auto metric = ivfflat_index_def.metric_type_;
            switch (metric) {
                case MetricType::kMerticL2:
                    // case MetricType::kMerticInnerProduct:
                    break;
                case MetricType::kInvalid:
                    Error<StorageException>("Metric type is invalid");
                default:
                    Error<NotImplementException>("Not implemented");
            }
            // TODO: assume that this index is created on float column?
            Vector<f32> all_data;
            all_data.reserve((segment_entry->row_count_) * dimension);
            for (const auto &block_entry : segment_entry->block_entries_) {
                auto block_column_entry = block_entry->columns_[column_id].get();
                BufferHandle buffer_handle = block_column_entry->buffer_->Load();
                auto block_data_ptr = reinterpret_cast<const float *>(buffer_handle.GetData());
                SizeT block_row_cnt = block_entry->row_count_;
                all_data.insert(all_data.end(), block_data_ptr, block_data_ptr + block_row_cnt * dimension);
            }
            auto ann_index_data =
                AnnIVFFlatL2<f32>::CreateIndex(dimension, segment_entry->row_count_, all_data.data(), ivfflat_index_def.centroids_count_);

            auto index_entry = IndexEntry::NewAnnIVFFlatIndexEntry(segment_entry,
                                                                   index_def.index_name_,
                                                                   create_ts,
                                                                   buffer_mgr,
                                                                   new AnnIVFFlatIndexPtr(ann_index_data.release()));

            txn_store->CreateIndexFile(segment_entry->segment_id_, Move(index_entry));
            return index_entry;
        }
        case IndexMethod::kIVFFlatFaiss: {
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
                    Error<StorageException>("Metric type is invalid");
                }
                default: {
                    Error<NotImplementException>("Not implemented");
                }
            }
            auto index = new faiss::IndexIVFFlat(quantizer, dimension, ivfflat_index_def.centroids_count_, metric);
            for (const auto &block_entry : segment_entry->block_entries_) {
                auto block_column_entry = block_entry->columns_[column_id].get();
                BufferHandle buffer_handle = block_column_entry->buffer_->Load();
                auto block_data_ptr = reinterpret_cast<const float *>(buffer_handle.GetData());
                SizeT block_row_cnt = block_entry->row_count_;
                index->train(block_row_cnt, block_data_ptr);
            }

            auto index_entry =
                IndexEntry::NewIndexEntry(segment_entry, index_def.index_name_, create_ts, buffer_mgr, new FaissIndexPtr(index, quantizer));

            txn_store->CreateIndexFile(segment_entry->segment_id_, Move(index_entry));
            return index_entry;
        }
        case IndexMethod::kInvalid: {
            Error<StorageException>("Index method type is invalid");
        }
        default: {
            Error<NotImplementException>("Not implemented");
        }
    }
}

void SegmentEntry::CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, u16 block_id, u16 start_pos, u16 row_count) {
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    SharedPtr<BlockEntry> block_entry;
    {
        UniqueLock<RWMutex> lck(segment_entry->rw_locker_);
        if (segment_entry->min_row_ts_ == 0) {
            segment_entry->min_row_ts_ = commit_ts;
        }
        segment_entry->max_row_ts_ = Max(segment_entry->max_row_ts_, commit_ts);
        block_entry = segment_entry->block_entries_[block_id];
    }
    BlockEntry::CommitAppend(block_entry.get(), txn_ptr);
}

void SegmentEntry::CommitCreateIndex(SegmentEntry *segment_entry, SharedPtr<IndexEntry> index_entry) {
    segment_entry->index_entry_map_.emplace(*index_entry->index_name_, Move(index_entry));
}

void SegmentEntry::CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>> &block_row_hashmap) {
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);

    for (const auto &row_hash_map : block_row_hashmap) {
        u16 block_id = row_hash_map.first;
        // TODO: block_id is u16, GetBlockEntryByID need to be modified accordingly.
        BlockEntry *block_entry = SegmentEntry::GetBlockEntryByID(segment_entry, block_id);
        if (block_entry == nullptr) {
            Error<StorageException>(Format("The segment doesn't contain the given block: {}.", block_id));
        }

        BlockEntry::CommitDelete(block_entry, txn_ptr);
        segment_entry->max_row_ts_ = Max(segment_entry->max_row_ts_, commit_ts);
    }
}

u16 SegmentEntry::GetMaxBlockID(const SegmentEntry *segment_entry) { return segment_entry->block_entries_.size(); }

BlockEntry *SegmentEntry::GetBlockEntryByID(const SegmentEntry *segment_entry, u16 block_id) {
    if (block_id < segment_entry->block_entries_.size()) {
        return segment_entry->block_entries_[block_id].get();
    } else {
        return nullptr;
    }
}

Json SegmentEntry::Serialize(SegmentEntry *segment_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<BlockEntry *> block_entries;
    {
        SharedLock<RWMutex> lck(segment_entry->rw_locker_);
        json_res["segment_dir"] = *segment_entry->segment_dir_;
        json_res["row_capacity"] = segment_entry->row_capacity_;
        json_res["segment_id"] = segment_entry->segment_id_;
        json_res["column_count"] = segment_entry->column_count_;
        json_res["min_row_ts"] = segment_entry->min_row_ts_;
        json_res["max_row_ts"] = segment_entry->max_row_ts_;
        json_res["deleted"] = segment_entry->deleted_;
        json_res["row_count"] = segment_entry->row_count_;
        for (auto &block_entry : segment_entry->block_entries_) {
            if (is_full_checkpoint || max_commit_ts > block_entry->checkpoint_ts_) {
                block_entries.push_back((BlockEntry *)block_entry.get());
            }
        }
    }
    for (BlockEntry *block_entry : block_entries) {
        LOG_TRACE(Format("Before Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                         block_entry->checkpoint_ts_,
                         block_entry->min_row_ts_,
                         block_entry->max_row_ts_,
                         max_commit_ts));
        BlockEntry::Flush(block_entry, max_commit_ts);
        LOG_TRACE(Format("Finish Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                         block_entry->checkpoint_ts_,
                         block_entry->min_row_ts_,
                         block_entry->max_row_ts_,
                         max_commit_ts));
        // WARNING: this operation may influence data visibility
        if (!is_full_checkpoint && block_entry->checkpoint_ts_ != max_commit_ts) {
            continue;
        }
        json_res["block_entries"].emplace_back(BlockEntry::Serialize(block_entry, max_commit_ts));
    }
    for (const auto &[_index_name, index_entry] : segment_entry->index_entry_map_) {
        IndexEntry::Flush(index_entry.get(), max_commit_ts);
        json_res["index_entries"].emplace_back(IndexEntry::Serialize(index_entry.get()));
    }
    return json_res;
}

SharedPtr<SegmentEntry> SegmentEntry::Deserialize(const Json &segment_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr) {
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
        for (const auto &block_json : segment_entry_json["block_entries"]) {
            UniquePtr<BlockEntry> block_entry = BlockEntry::Deserialize(block_json, segment_entry.get(), buffer_mgr);
            auto block_entries_size = segment_entry->block_entries_.size();
            segment_entry->block_entries_.resize(Max(block_entries_size, static_cast<SizeT>(block_entry->block_id_ + 1)));
            segment_entry->block_entries_[block_entry->block_id_] = Move(block_entry);
        }
    }
    LOG_TRACE(Format("Segment: {}, Block count: {}", segment_entry->segment_id_, segment_entry->block_entries_.size()));

    if (segment_entry_json.contains("index_entries")) {
        for (const auto &index_json : segment_entry_json["index_entries"]) {
            SharedPtr<IndexEntry> index_entry = IndexEntry::Deserialize(index_json, segment_entry.get(), buffer_mgr);
            segment_entry->index_entry_map_.emplace(*index_entry->index_name_, Move(index_entry));
        }
    }

    return segment_entry;
}

SharedPtr<String> SegmentEntry::DetermineSegFilename(const String &parent_dir, u32 seg_id) {
    u32 seed = time(nullptr);
    LocalFileSystem fs;
    SharedPtr<String> segment_dir;
    do {
        segment_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_SEGMENT_NAME_LEN, seed) + "_seg_" + ToStr(seg_id));
    } while (!fs.CreateDirectoryNoExp(*segment_dir));
    return segment_dir;
}

void SegmentEntry::MergeFrom(BaseEntry &other) {
    auto segment_entry2 = dynamic_cast<SegmentEntry *>(&other);
    Assert<StorageException>(segment_entry2 != nullptr, "MergeFrom requires the same type of BaseEntry");
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(*this->segment_dir_ == *segment_entry2->segment_dir_, "SegmentEntry::MergeFrom requires segment_dir_ match");
    Assert<StorageException>(this->segment_id_ == segment_entry2->segment_id_, "SegmentEntry::MergeFrom requires segment_id_ match");
    Assert<StorageException>(this->column_count_ == segment_entry2->column_count_, "SegmentEntry::MergeFrom requires column_count_ match");
    Assert<StorageException>(this->row_capacity_ == segment_entry2->row_capacity_, "SegmentEntry::MergeFrom requires row_capacity_ match");
    Assert<StorageException>(this->min_row_ts_ == segment_entry2->min_row_ts_, "SegmentEntry::MergeFrom requires min_row_ts_ match");

    this->row_count_ = Max(this->row_count_, segment_entry2->row_count_);
    this->max_row_ts_ = Max(this->max_row_ts_, segment_entry2->max_row_ts_);
    this->row_capacity_ = Max(this->row_capacity_, segment_entry2->row_capacity_);

    int block_count = Min(this->block_entries_.size(), segment_entry2->block_entries_.size());
    for (int i = 0; i < block_count; i++) {
        auto &block_entry1 = this->block_entries_[i];
        auto &block_entry2 = this->block_entries_[i];
        if (block_entry1 == nullptr)
            block_entry1 = block_entry2;
        else if (block_entry2 != nullptr)
            block_entry1->MergeFrom(*block_entry2);
    }

    for (const auto &[index_name, index_entry] : segment_entry2->index_entry_map_) {
        if (this->index_entry_map_.find(index_name) == this->index_entry_map_.end()) {
            this->index_entry_map_.emplace(index_name, index_entry);
        } else {
            this->index_entry_map_[index_name]->MergeFrom(*index_entry);
        }
    }
}

} // namespace infinity
