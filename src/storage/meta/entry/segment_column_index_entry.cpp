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

module catalog;

import stl;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import logger;
import third_party;
import infinity_exception;

import index_file_worker;
import status;
import index_base;
import index_hnsw;
import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;

namespace infinity {
SegmentColumnIndexEntry::SegmentColumnIndexEntry(ColumnIndexEntry *column_index_entry, u32 segment_id, BufferObj *buffer)
    : BaseEntry(EntryType::kSegmentColumnIndex), column_index_entry_(column_index_entry), segment_id_(segment_id), buffer_(buffer){};

SharedPtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::NewIndexEntry(ColumnIndexEntry *column_index_entry,
                                                                          u32 segment_id,
                                                                          TxnTimeStamp create_ts,
                                                                          BufferManager *buffer_manager,
                                                                          CreateIndexParam *param) {
    // FIXME: estimate index size.
    UniquePtr<IndexFileWorker> file_worker = column_index_entry->CreateFileWorker(param, segment_id);
    auto buffer = buffer_manager->Allocate(Move(file_worker));
    auto segment_column_index_entry = SharedPtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, buffer));
    segment_column_index_entry->min_ts_ = create_ts;
    segment_column_index_entry->max_ts_ = create_ts;
    return segment_column_index_entry;
}

UniquePtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::LoadIndexEntry(ColumnIndexEntry *column_index_entry,
                                                                           u32 segment_id,
                                                                           BufferManager *buffer_manager,
                                                                           CreateIndexParam *param) {
    UniquePtr<IndexFileWorker> file_worker = column_index_entry->CreateFileWorker(param, segment_id);
    auto buffer = buffer_manager->Get(Move(file_worker));
    return UniquePtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, buffer));
}

BufferHandle SegmentColumnIndexEntry::GetIndex() { return buffer_->Load(); }

Status SegmentColumnIndexEntry::CreateIndexDo(IndexBase *index_base, const ColumnDef *column_def, atomic_u64 &create_index_idx) {
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            auto InsertHnswDo = [&](auto *hnsw_index, atomic_u64 &create_index_idx) {
                SizeT vertex_n = hnsw_index->GetVertexNum();
                while (true) {
                    SizeT idx = create_index_idx.fetch_add(1);
                    if (idx % 10000 == 0) {
                        LOG_INFO(Format("Insert index: {}", idx));
                    }
                    if (idx >= vertex_n) {
                        break;
                    }
                    hnsw_index->Build(idx);
                }
            };
            auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                Error<StorageException>("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BufferHandle buffer_handle = GetIndex();

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    switch (index_hnsw->encode_type_) {
                        case HnswEncodeType::kPlain: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto *hnsw_index =
                                        static_cast<KnnHnsw<float, u64, PlainStore<float>, PlainIPDist<float>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index =
                                        static_cast<KnnHnsw<float, u64, PlainStore<float>, PlainL2Dist<float>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
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
                                    auto *hnsw_index =
                                        static_cast<KnnHnsw<float, u64, LVQStore<float, i8, LVQIPCache<float, i8>>, LVQIPDist<float, i8>> *>(
                                            buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index =
                                        static_cast<KnnHnsw<float, u64, LVQStore<float, i8, LVQL2Cache<float, i8>>, LVQL2Dist<float, i8>> *>(
                                            buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
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
        default: {
            break;
        }
    }
    return Status::OK();
}

void SegmentColumnIndexEntry::UpdateIndex(TxnTimeStamp, FaissIndexPtr *, BufferManager *) { Error<NotImplementException>("Not implemented"); }

bool SegmentColumnIndexEntry::Flush(TxnTimeStamp checkpoint_ts) {
    String &index_name = *this->column_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    LOG_TRACE(Format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    if (this->max_ts_ <= this->checkpoint_ts_ || this->min_ts_ > checkpoint_ts) {
        LOG_TRACE(Format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                         segment_id,
                         index_name));
        return false;
    }
    if (this->buffer_ == nullptr) {
        LOG_WARN("Index entry is not initialized");
        return false;
    }
    if (this->buffer_->Save()) {
        this->buffer_->Sync();
        this->buffer_->CloseFile();
    }

    this->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(Format("Segment: {}, Index: {} is flushed", segment_id, index_name));
    return true;
}

Json SegmentColumnIndexEntry::Serialize() {
    if (this->deleted_) {
        Error<StorageException>("Segment Column index entry can't be deleted.");
    }

    Json index_entry_json;
    {
        SharedLock<RWMutex> lck(this->rw_locker_);
        index_entry_json["segment_id"] = this->segment_id_;
        index_entry_json["min_ts"] = this->min_ts_;
        index_entry_json["max_ts"] = this->max_ts_;
        index_entry_json["checkpoint_ts"] = this->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry
    }

    return index_entry_json;
}

UniquePtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::Deserialize(const Json &index_entry_json,
                                                                        ColumnIndexEntry *column_index_entry,
                                                                        BufferManager *buffer_mgr,
                                                                        TableEntry *table_entry) {
    u32 segment_id = index_entry_json["segment_id"];
    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);

    if (!status.ok()) {
        Error<StorageException>(status.message());
        return nullptr;
    }
    u64 column_id = column_index_entry->column_id();
    UniquePtr<CreateIndexParam> create_index_param =
        SegmentEntry::GetCreateIndexParam(segment_row_count, column_index_entry->index_base_ptr(), table_entry->GetColumnDefByID(column_id));
    // TODO: need to get create index param;
    //    UniquePtr<CreateIndexParam> create_index_param = SegmentEntry::GetCreateIndexParam(segment_entry, index_base, column_def.get());
    auto segment_column_index_entry = LoadIndexEntry(column_index_entry, segment_id, buffer_mgr, create_index_param.get());
    if (segment_column_index_entry.get() == nullptr) {
        Error<StorageException>("Failed to load index entry");
    }
    segment_column_index_entry->min_ts_ = index_entry_json["min_ts"];
    segment_column_index_entry->max_ts_ = index_entry_json["max_ts"];
    segment_column_index_entry->checkpoint_ts_ = index_entry_json["checkpoint_ts"]; // TODO shenyushi:: use fields in BaseEntry
    return segment_column_index_entry;
}

void SegmentColumnIndexEntry::MergeFrom(BaseEntry &other) {
    auto &other_index_entry = dynamic_cast<SegmentColumnIndexEntry &>(other);
    if (other_index_entry.min_ts_ < min_ts_) {
        min_ts_ = other_index_entry.min_ts_;
    }
    if (other_index_entry.max_ts_ > max_ts_) {
        max_ts_ = other_index_entry.max_ts_;
    }
    if (other_index_entry.checkpoint_ts_ > checkpoint_ts_) {
        checkpoint_ts_ = other_index_entry.checkpoint_ts_;
    }
}

} // namespace infinity
