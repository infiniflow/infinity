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

#include <ctime>
#include <string>
#include <vector>

module catalog;

import stl;
import third_party;
import buffer_manager;
import default_values;
import data_block;
import txn;
import data_access_state;
import index_def;

import infinity_exception;
import defer_op;
import index_ivfflat;
import index_hnsw;
import buffer_handle;
import annivfflat_index_data;
import annivfflat_index_file_worker;
import index_file_worker;
import hnsw_file_worker;
import logger;
import local_file_system;
import random;
import parser;
import txn_store;
import index_base;

import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;
import segment_iter;

namespace infinity {

SegmentEntry::SegmentEntry(const TableEntry *table_entry) : BaseEntry(EntryType::kSegment), table_entry_(table_entry) {}

SharedPtr<SegmentEntry> SegmentEntry::MakeNewSegmentEntry(const TableEntry *table_entry, u32 segment_id, BufferManager *buffer_mgr) {
    SharedPtr<SegmentEntry> new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_count_ = 0;
    new_entry->row_capacity_ = DEFAULT_SEGMENT_CAPACITY;
    new_entry->segment_id_ = segment_id;
    new_entry->min_row_ts_ = 0;
    new_entry->max_row_ts_ = 0;

    const auto *table_ptr = (const TableEntry *)table_entry;
    new_entry->column_count_ = table_ptr->ColumnCount();

    new_entry->segment_dir_ = SegmentEntry::DetermineSegmentDir(*table_entry->TableEntryDir(), segment_id);
    if (new_entry->block_entries_.empty()) {
        new_entry->block_entries_.emplace_back(
            MakeUnique<BlockEntry>(new_entry.get(), new_entry->block_entries_.size(), 0, new_entry->column_count_, buffer_mgr));
    }
    return new_entry;
}

SharedPtr<SegmentEntry>
SegmentEntry::MakeReplaySegmentEntry(const TableEntry *table_entry, u32 segment_id, SharedPtr<String> segment_dir, TxnTimeStamp commit_ts) {

    auto new_entry = MakeShared<SegmentEntry>(table_entry);
    new_entry->row_capacity_ = DEFAULT_SEGMENT_CAPACITY;
    new_entry->segment_id_ = segment_id;
    new_entry->min_row_ts_ = commit_ts;
    new_entry->max_row_ts_ = commit_ts;

    const auto *table_ptr = (const TableEntry *)table_entry;
    new_entry->column_count_ = table_ptr->ColumnCount();
    new_entry->segment_dir_ = Move(segment_dir);
    return new_entry;
}

int SegmentEntry::Room() {
    SharedLock<RWMutex> lck(rw_locker_);
    return this->row_capacity_ - this->row_count_;
}

u64 SegmentEntry::AppendData(SegmentEntry *segment_entry, u64 txn_id, AppendState *append_state_ptr, BufferManager *buffer_mgr) {
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);
    if (segment_entry->row_capacity_ - segment_entry->row_count_ <= 0)
        return 0;
    //    SizeT start_row = segment_entry->row_count_;
    SizeT append_block_count = append_state_ptr->blocks_.size();
    u64 total_copied{0};

    while (append_state_ptr->current_block_ < append_block_count && segment_entry->row_count_ < segment_entry->row_capacity_) {
        DataBlock *input_block = append_state_ptr->blocks_[append_state_ptr->current_block_].get();

        u16 to_append_rows = input_block->row_count();
        while (to_append_rows > 0 && segment_entry->row_count_ < segment_entry->row_capacity_) {
            // Append to_append_rows into block
            BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
            if (last_block_entry->GetAvailableCapacity() <= 0) {
                segment_entry->block_entries_.emplace_back(
                    MakeUnique<BlockEntry>(segment_entry, segment_entry->block_entries_.size(), 0, segment_entry->column_count_, buffer_mgr));
                last_block_entry = segment_entry->block_entries_.back().get();
            }

            u32 range_segment_id = segment_entry->segment_id_;
            u16 range_block_id = last_block_entry->block_id();
            u16 range_block_start_row = last_block_entry->row_count();

            u16 actual_appended =
                last_block_entry->AppendData(txn_id, input_block, append_state_ptr->current_block_offset_, to_append_rows, buffer_mgr);
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

void SegmentEntry::DeleteData(SegmentEntry *segment_entry, u64 txn_id, TxnTimeStamp commit_ts, const HashMap<u16, Vector<RowID>> &block_row_hashmap) {
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);

    for (const auto &row_hash_map : block_row_hashmap) {
        u16 block_id = row_hash_map.first;
        BlockEntry *block_entry = segment_entry->GetBlockEntryByID(block_id);
        if (block_entry == nullptr) {
            Error<StorageException>(Format("The segment doesn't contain the given block: {}.", block_id));
        }

        const Vector<RowID> &rows = row_hash_map.second;
        block_entry->DeleteData(txn_id, commit_ts, rows);
    }
}

template <typename DataType>
class OneColumnIterator {
public:
    OneColumnIterator(const SegmentEntry *entry, SizeT column_id) : segment_iter_(entry, MakeShared<Vector<SizeT>>(Vector<SizeT>{column_id})) {}

    Optional<const DataType *> Next() {
        if (auto ret = segment_iter_.Next(); ret) {
            return reinterpret_cast<const DataType *>((*ret)[0]);
        }
        return None;
    }

private:
    SegmentIter segment_iter_;
};

SharedPtr<SegmentColumnIndexEntry> SegmentEntry::CreateIndexFile(SegmentEntry *segment_entry,
                                                                 ColumnIndexEntry *column_index_entry,
                                                                 SharedPtr<ColumnDef> column_def,
                                                                 TxnTimeStamp create_ts,
                                                                 BufferManager *buffer_mgr,
                                                                 TxnTableStore *txn_store) {
    u64 column_id = column_def->id();
    //    SharedPtr<IndexDef> index_def = index_def_entry->index_def_;
    const IndexBase *index_base = column_index_entry->index_base_ptr();
    //    UniquePtr<CreateIndexParam> create_index_param = MakeUnique<CreateIndexParam>(index_base, column_def.get());
    UniquePtr<CreateIndexParam> create_index_param = SegmentEntry::GetCreateIndexParam(segment_entry->row_count_, index_base, column_def.get());
    SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry =
        SegmentColumnIndexEntry::NewIndexEntry(column_index_entry, segment_entry->segment_id_, create_ts, buffer_mgr, create_index_param.get());
    switch (index_base->index_type_) {

        case IndexType::kIVFFlat: {
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                Error<StorageException>("AnnIVFFlat supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);
            SizeT dimension = embedding_info->Dimension();
            BufferHandle buffer_handle = segment_column_index_entry->GetIndex();
            switch (embedding_info->Type()) {
                case kElemFloat: {
                    auto annivfflat_index = static_cast<AnnIVFFlatIndexData<f32> *>(buffer_handle.GetDataMut());
                    // TODO: How to select training data?
                    Vector<f32> segment_column_data;
                    segment_column_data.reserve(segment_entry->row_count_ * dimension);
                    for (const auto &block_entry : segment_entry->block_entries_) {
                        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                        BufferHandle block_column_buffer_handle = block_column_entry->buffer()->Load();
                        auto block_column_data_ptr = reinterpret_cast<const float *>(block_column_buffer_handle.GetData());
                        SizeT block_row_cnt = block_entry->row_count();
                        segment_column_data.insert(segment_column_data.end(),
                                                   block_column_data_ptr,
                                                   block_column_data_ptr + block_row_cnt * dimension);
                    }
                    SizeT total_row_cnt = segment_column_data.size() / dimension;
                    annivfflat_index->train_centroids(dimension, total_row_cnt, segment_column_data.data());
                    annivfflat_index->insert_data(dimension, total_row_cnt, segment_column_data.data());
                    break;
                }
                default: {
                    Error<StorageException>("Not implemented");
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                Error<StorageException>("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BufferHandle buffer_handle = segment_column_index_entry->GetIndex();
            auto InsertHnsw = [&](auto &hnsw_index) {
                u32 segment_offset = 0;
                Vector<u64> row_ids;
                for (const auto &block_entry : segment_entry->block_entries_) {
                    SizeT block_row_cnt = block_entry->row_count();

                    for (SizeT block_offset = 0; block_offset < block_row_cnt; ++block_offset) {
                        RowID row_id(segment_entry->segment_id_, segment_offset + block_offset);
                        row_ids.push_back(row_id.ToUint64());
                    }
                    segment_offset += DEFAULT_BLOCK_CAPACITY;
                }
                OneColumnIterator<float> one_column_iter(segment_entry, column_id);
                hnsw_index->InsertVecs(one_column_iter, row_ids.data(), row_ids.size());
            };
            switch (embedding_info->Type()) {
                case kElemFloat: {
                    switch (index_hnsw->encode_type_) {
                        case HnswEncodeType::kPlain: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto hnsw_index =
                                        static_cast<KnnHnsw<float, u64, PlainStore<float>, PlainIPDist<float>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index =
                                        static_cast<KnnHnsw<float, u64, PlainStore<float>, PlainL2Dist<float>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    Error<StorageException>("Not implemented");
                                }
                            }
                            break;
                        }
                        case HnswEncodeType::kLVQ: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    // too long type. fix it.
                                    auto hnsw_index =
                                        static_cast<KnnHnsw<float, u64, LVQStore<float, i8, LVQIPCache<float, i8>>, LVQIPDist<float, i8>> *>(
                                            buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index =
                                        static_cast<KnnHnsw<float, u64, LVQStore<float, i8, LVQL2Cache<float, i8>>, LVQL2Dist<float, i8>> *>(
                                            buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    Error<StorageException>("Not implemented");
                                }
                            }
                            break;
                        }
                        default: {
                            Error<StorageException>("Not implemented");
                        }
                    }
                    break;
                }
                default: {
                    Error<StorageException>("Not implemented");
                }
            }
            break;
        }
        case IndexType::kIRSFullText: {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
        default: {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
    }
    txn_store->CreateIndexFile(column_index_entry->table_index_entry(), column_id, segment_entry->segment_id_, segment_column_index_entry);
    return segment_column_index_entry;
}

void SegmentEntry::CommitAppend(SegmentEntry *segment_entry, u64 txn_id, TxnTimeStamp commit_ts, u16 block_id, u16, u16) {
    SharedPtr<BlockEntry> block_entry;
    {
        UniqueLock<RWMutex> lck(segment_entry->rw_locker_);
        if (segment_entry->min_row_ts_ == 0) {
            segment_entry->min_row_ts_ = commit_ts;
        }
        segment_entry->max_row_ts_ = Max(segment_entry->max_row_ts_, commit_ts);
        block_entry = segment_entry->block_entries_[block_id];
    }
    block_entry->CommitAppend(txn_id, commit_ts);
}

void SegmentEntry::CommitDelete(SegmentEntry *segment_entry,
                                u64 txn_id,
                                TxnTimeStamp commit_ts,
                                const HashMap<u16, Vector<RowID>> &block_row_hashmap) {
    UniqueLock<RWMutex> lck(segment_entry->rw_locker_);

    for (const auto &row_hash_map : block_row_hashmap) {
        u16 block_id = row_hash_map.first;
        // TODO: block_id is u16, GetBlockEntryByID need to be modified accordingly.
        BlockEntry *block_entry = segment_entry->GetBlockEntryByID(block_id);
        if (block_entry == nullptr) {
            Error<StorageException>(Format("The segment doesn't contain the given block: {}.", block_id));
        }

        block_entry->CommitDelete(txn_id, commit_ts);
        segment_entry->max_row_ts_ = Max(segment_entry->max_row_ts_, commit_ts);
    }
}

BlockEntry *SegmentEntry::GetBlockEntryByID(u16 block_id) const {
    if (block_id < block_entries_.size()) {
        return block_entries_[block_id].get();
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
            if (is_full_checkpoint || max_commit_ts > block_entry->checkpoint_ts()) {
                block_entries.push_back((BlockEntry *)block_entry.get());
            }
        }
    }
    for (BlockEntry *block_entry : block_entries) {
        LOG_TRACE(Format("Before Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                         block_entry->checkpoint_ts(),
                         block_entry->min_row_ts(),
                         block_entry->max_row_ts(),
                         max_commit_ts));
        block_entry->Flush(max_commit_ts);
        LOG_TRACE(Format("Finish Flush: block_entry checkpoint ts: {}, min_row_ts: {}, max_row_ts: {} || max_commit_ts: {}",
                         block_entry->checkpoint_ts(),
                         block_entry->min_row_ts(),
                         block_entry->max_row_ts(),
                         max_commit_ts));
        // WARNING: this operation may influence data visibility
        //        if (!is_full_checkpoint && block_entry->checkpoint_ts_ != max_commit_ts) {
        //            continue;
        //        }
        json_res["block_entries"].emplace_back(BlockEntry::Serialize(block_entry, max_commit_ts));
    }
    return json_res;
}

SharedPtr<SegmentEntry> SegmentEntry::Deserialize(const Json &segment_entry_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
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
            segment_entry->block_entries_.resize(Max(block_entries_size, static_cast<SizeT>(block_entry->block_id() + 1)));
            segment_entry->block_entries_[block_entry->block_id()] = Move(block_entry);
        }
    }
    LOG_TRACE(Format("Segment: {}, Block count: {}", segment_entry->segment_id_, segment_entry->block_entries_.size()));

    return segment_entry;
}

SharedPtr<String> SegmentEntry::DetermineSegmentDir(const String &parent_dir, u32 seg_id) {
    LocalFileSystem fs;
    SharedPtr<String> segment_dir;
    do {
        u32 seed = time(nullptr);
        segment_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_NAME_LEN, seed) + "_seg_" + ToStr(seg_id));
    } while (!fs.CreateDirectoryNoExp(*segment_dir));
    return segment_dir;
}

void SegmentEntry::MergeFrom(BaseEntry &other) {
    auto segment_entry2 = dynamic_cast<SegmentEntry *>(&other);
    if (segment_entry2 == nullptr) {
        Error<StorageException>("MergeFrom requires the same type of BaseEntry");
    }
    if (*this->segment_dir_ != *segment_entry2->segment_dir_) {
        Error<StorageException>("SegmentEntry::MergeFrom requires segment_dir_ match");
    }
    if (this->segment_id_ != segment_entry2->segment_id_) {
        Error<StorageException>("SegmentEntry::MergeFrom requires segment_id_ match");
    }
    if (this->column_count_ != segment_entry2->column_count_) {
        Error<StorageException>("SegmentEntry::MergeFrom requires column_count_ match");
    }
    if (this->row_capacity_ != segment_entry2->row_capacity_) {
        Error<StorageException>("SegmentEntry::MergeFrom requires row_capacity_ match");
    }
    if (this->min_row_ts_ != segment_entry2->min_row_ts_) {
        Error<StorageException>("SegmentEntry::MergeFrom requires min_row_ts_ match");
    }
    if (this->block_entries_.size() > segment_entry2->block_entries_.size()) {
        Error<StorageException>("SegmentEntry::MergeFrom requires source segment entry blocks not more than segment entry blocks");
    }

    this->row_count_ = Max(this->row_count_, segment_entry2->row_count_);
    this->max_row_ts_ = Max(this->max_row_ts_, segment_entry2->max_row_ts_);
    this->row_capacity_ = Max(this->row_capacity_, segment_entry2->row_capacity_);

    SizeT block_count = this->block_entries_.size();
    SizeT idx = 0;
    for (; idx < block_count; ++idx) {
        auto &block_entry1 = this->block_entries_[idx];
        auto &block_entry2 = segment_entry2->block_entries_[idx];
        if (block_entry1.get() == nullptr) {
            block_entry1 = block_entry2;
        } else if (block_entry2.get() != nullptr) {
            block_entry1->MergeFrom(*block_entry2);
        }
    }

    SizeT segment2_block_count = segment_entry2->block_entries_.size();
    for (; idx < segment2_block_count; ++idx) {
        this->block_entries_.emplace_back(segment_entry2->block_entries_[idx]);
    }

    // for (const auto &[index_name, index_entry] : segment_entry2->index_entry_map_) {
    //     if (this->index_entry_map_.find(index_name) == this->index_entry_map_.end()) {
    //         this->index_entry_map_.emplace(index_name, index_entry);
    //     } else {
    //         this->index_entry_map_[index_name]->MergeFrom(*index_entry);
    //     }
    // }
}

UniquePtr<CreateIndexParam> SegmentEntry::GetCreateIndexParam(SizeT seg_row_count, const IndexBase *index_base, const ColumnDef *column_def) {
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            return MakeUnique<CreateAnnIVFFlatParam>(index_base, column_def, seg_row_count);
        }
        case IndexType::kHnsw: {
            SizeT max_element = seg_row_count;
            return MakeUnique<CreateHnswParam>(index_base, column_def, max_element);
        }
        case IndexType::kIRSFullText: {
            return MakeUnique<CreateFullTextParam>(index_base, column_def);
        }
        default: {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
    }
    return nullptr;
}

} // namespace infinity
