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
import catalog_delta_entry;
import column_vector;
import annivfflat_index_data;
import segment_iter;
import secondary_index_data;

namespace infinity {

SegmentColumnIndexEntry::SegmentColumnIndexEntry(ColumnIndexEntry *column_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer)
    : BaseEntry(EntryType::kSegmentColumnIndex), column_index_entry_(column_index_entry), segment_id_(segment_id),
      vector_buffer_(std::move(vector_buffer)){};

SharedPtr<SegmentColumnIndexEntry>
SegmentColumnIndexEntry::NewIndexEntry(ColumnIndexEntry *column_index_entry, SegmentID segment_id, Txn *txn, CreateIndexParam *param) {
    auto *buffer_mgr = txn->GetBufferMgr();

    // FIXME: estimate index size.
    auto vector_file_worker = column_index_entry->CreateFileWorker(param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_mgr->Allocate(std::move(vector_file_worker[i]));
    };
    auto segment_column_index_entry =
        SharedPtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, std::move(vector_buffer)));
    auto begin_ts = txn->BeginTS();
    segment_column_index_entry->min_ts_ = begin_ts;
    segment_column_index_entry->max_ts_ = begin_ts;
    segment_column_index_entry->begin_ts_ = begin_ts;

    {
        if (txn != nullptr) {
            auto operation = MakeUnique<AddSegmentColumnIndexEntryOp>(segment_column_index_entry);
            txn->AddCatalogDeltaOperation(std::move(operation));
        }
    }
    return segment_column_index_entry;
}

SharedPtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::NewReplaySegmentIndexEntry(ColumnIndexEntry *column_index_entry,
                                                                                       TableEntry *table_entry,
                                                                                       SegmentID segment_id,
                                                                                       BufferManager *buffer_manager,
                                                                                       TxnTimeStamp min_ts,
                                                                                       TxnTimeStamp max_ts,
                                                                                       TransactionID txn_id,
                                                                                       TxnTimeStamp begin_ts,
                                                                                       TxnTimeStamp commit_ts,
                                                                                       bool is_delete) {

    auto [segment_row_count, actual_segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    ColumnID column_id = column_index_entry->column_id();
    auto create_index_param =
        column_index_entry->GetCreateIndexParam(segment_row_count, actual_segment_row_count, table_entry->GetColumnDefByID(column_id));
    auto vector_file_worker = column_index_entry->CreateFileWorker(create_index_param.get(), segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->Get(std::move(vector_file_worker[i]));
    };
    auto segment_column_index_entry =
        SharedPtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, std::move(vector_buffer)));
    if (segment_column_index_entry.get() == nullptr) {
        UnrecoverableError("Failed to load index entry");
    }
    segment_column_index_entry->min_ts_ = min_ts;
    segment_column_index_entry->max_ts_ = max_ts;
    segment_column_index_entry->commit_ts_.store(commit_ts);
    return segment_column_index_entry;
}

UniquePtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::LoadIndexEntry(ColumnIndexEntry *column_index_entry,
                                                                           u32 segment_id,
                                                                           BufferManager *buffer_manager,
                                                                           CreateIndexParam *param) {
    auto vector_file_worker = column_index_entry->CreateFileWorker(param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->Get(std::move(vector_file_worker[i]));
    }
    return UniquePtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, std::move(vector_buffer)));
}

BufferHandle SegmentColumnIndexEntry::GetIndex() { return vector_buffer_[0]->Load(); }

BufferHandle SegmentColumnIndexEntry::GetIndexPartAt(u32 idx) { return vector_buffer_[idx + 1]->Load(); }

Status SegmentColumnIndexEntry::CreateIndexPrepare(const IndexBase *index_base,
                                                   ColumnID column_id,
                                                   const ColumnDef *column_def,
                                                   const SegmentEntry *segment_entry,
                                                   Txn *txn,
                                                   bool prepare,
                                                   bool check_ts) {
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto *buffer_mgr = txn->GetBufferMgr();
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("AnnIVFFlat only supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);
            SizeT dimension = embedding_info->Dimension();
            BufferHandle buffer_handle = GetIndex();
            switch (embedding_info->Type()) {
                case kElemFloat: {
                    auto annivfflat_index = reinterpret_cast<AnnIVFFlatIndexData<f32> *>(buffer_handle.GetDataMut());
                    // TODO: How to select training data?
                    Vector<f32> segment_column_data;
                    segment_column_data.reserve(segment_entry->row_count() * dimension);
                    for (const auto &block_entry : segment_entry->block_entries()) {
                        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);

                        ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                        auto *data_ptr = reinterpret_cast<float *>(column_vector.data());
                        SizeT block_row_cnt = block_entry->row_count();
                        segment_column_data.insert(segment_column_data.end(), data_ptr, data_ptr + block_row_cnt * dimension);
                    }
                    SizeT total_row_cnt = segment_column_data.size() / dimension;
                    annivfflat_index->train_centroids(dimension, total_row_cnt, segment_column_data.data());
                    annivfflat_index->insert_data(dimension, total_row_cnt, segment_column_data.data());
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not support data type for index ivf."));
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BufferHandle buffer_handle = GetIndex();

            auto InsertHnsw = [&](auto &hnsw_index) {
                auto InsertHnswInner = [&](auto &iter) {
                    if (!prepare) {
                        // Single thread insert
                        hnsw_index->InsertVecs(iter, segment_entry->row_count()); // estimate insert count
                    } else {
                        // Multi thread insert data, write file in the physical create index finish stage.
                        hnsw_index->StoreData(iter, segment_entry->row_count());
                    }
                };
                if (check_ts) {
                    OneColumnIterator<float> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    InsertHnswInner(iter);
                } else {
                    OneColumnIterator<float, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    InsertHnswInner(iter);
                }
            };

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    switch (index_hnsw->encode_type_) {
                        case HnswEncodeType::kPlain: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainIPDist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainL2Dist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Invalid metric type.");
                                }
                            }
                            break;
                        }
                        case HnswEncodeType::kLVQ: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    // too long type. fix it.
                                    auto hnsw_index = static_cast<KnnHnsw<float,
                                                                          SegmentOffset,
                                                                          LVQStore<float, SegmentOffset, i8, LVQIPCache<float, i8>>,
                                                                          LVQIPDist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index = static_cast<KnnHnsw<float,
                                                                          SegmentOffset,
                                                                          LVQStore<float, SegmentOffset, i8, LVQL2Cache<float, i8>>,
                                                                          LVQL2Dist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Invalid metric type.");
                                }
                            }
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid HNSW encode type.");
                        }
                    }
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not support data type for index hnsw."));
                }
            }
            break;
        }
        case IndexType::kIRSFullText: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
        case IndexType::kSecondary: {
            auto &data_type = column_def->type();
            if (!(data_type->CanBuildSecondaryIndex())) {
                UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
            }
            // 1. build secondary index by merge sort
            u32 part_capacity = DEFAULT_BLOCK_CAPACITY;
            // fetch the actual_row_count from segment_entry
            auto secondary_index_builder =
                GetSecondaryIndexDataBuilder(data_type, segment_entry->row_count(), segment_entry->actual_row_count(), part_capacity);
            secondary_index_builder->LoadSegmentData(segment_entry, buffer_mgr, column_id, begin_ts, check_ts);
            secondary_index_builder->StartOutput();
            // 2. output into SecondaryIndexDataPart
            {
                u32 part_num = GetIndexPartNum();
                for (u32 part_id = 0; part_id < part_num; ++part_id) {
                    BufferHandle buffer_handle_part = GetIndexPartAt(part_id);
                    auto secondary_index_part = static_cast<SecondaryIndexDataPart *>(buffer_handle_part.GetDataMut());
                    secondary_index_builder->OutputToPart(secondary_index_part);
                }
            }
            // 3. output into SecondaryIndexDataHead
            {
                BufferHandle buffer_handle_head = GetIndex();
                auto secondary_index_head = static_cast<SecondaryIndexDataHead *>(buffer_handle_head.GetDataMut());
                secondary_index_builder->OutputToHeader(secondary_index_head);
            }
            secondary_index_builder->EndOutput();
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    return Status::OK();
}

Status SegmentColumnIndexEntry::CreateIndexDo(const IndexBase *index_base, const ColumnDef *column_def, atomic_u64 &create_index_idx) {
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            auto InsertHnswDo = [&](auto *hnsw_index, atomic_u64 &create_index_idx) {
                SizeT vertex_n = hnsw_index->GetVertexNum();
                while (true) {
                    SizeT idx = create_index_idx.fetch_add(1);
                    if (idx % 10000 == 0) {
                        LOG_INFO(fmt::format("Insert index: {}", idx));
                    }
                    if (idx >= vertex_n) {
                        break;
                    }
                    hnsw_index->Build(idx);
                }
            };
            auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
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
                                    auto *hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainIPDist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainL2Dist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Not implemented");
                                }
                            }
                            break;
                        }
                        case HnswEncodeType::kLVQ: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto *hnsw_index = static_cast<KnnHnsw<float,
                                                                           SegmentOffset,
                                                                           LVQStore<float, SegmentOffset, i8, LVQIPCache<float, i8>>,
                                                                           LVQIPDist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index = static_cast<KnnHnsw<float,
                                                                           SegmentOffset,
                                                                           LVQStore<float, SegmentOffset, i8, LVQL2Cache<float, i8>>,
                                                                           LVQL2Dist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Not implemented");
                                }
                            }
                            break;
                        }
                        default: {
                            UnrecoverableError("Not implemented");
                        }
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Not implemented");
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

void SegmentColumnIndexEntry::UpdateIndex(TxnTimeStamp, FaissIndexPtr *, BufferManager *) { UnrecoverableError("Not implemented"); }

bool SegmentColumnIndexEntry::Flush(TxnTimeStamp checkpoint_ts) {
    String &index_name = *this->column_index_entry_->col_index_dir();
    u64 segment_id = this->segment_id_;
    if (this->max_ts_ <= this->checkpoint_ts_ || this->min_ts_ > checkpoint_ts) {
        LOG_TRACE(fmt::format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                              segment_id,
                              index_name));
        return false;
    }

    this->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} checkpoint is change to {}", segment_id, index_name, this->checkpoint_ts_));
    return true;
}

void SegmentColumnIndexEntry::SaveIndexFile() {
    String &index_name = *this->column_index_entry_->col_index_dir();
    u64 segment_id = this->segment_id_;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    for (auto &buffer_ptr : vector_buffer_) {
        if (buffer_ptr->Save()) {
            buffer_ptr->Sync();
            buffer_ptr->CloseFile();
        }
    }
}

nlohmann::json SegmentColumnIndexEntry::Serialize() {
    if (this->deleted_) {
        UnrecoverableError("Segment Column index entry can't be deleted.");
    }

    nlohmann::json index_entry_json;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        index_entry_json["segment_id"] = this->segment_id_;
        index_entry_json["min_ts"] = this->min_ts_;
        index_entry_json["max_ts"] = this->max_ts_;
        index_entry_json["checkpoint_ts"] = this->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry
    }

    return index_entry_json;
}

UniquePtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::Deserialize(const nlohmann::json &index_entry_json,
                                                                        ColumnIndexEntry *column_index_entry,
                                                                        BufferManager *buffer_mgr,
                                                                        TableEntry *table_entry) {
    u32 segment_id = index_entry_json["segment_id"];
    auto [segment_row_count, actual_segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);

    if (!status.ok()) {
        UnrecoverableError(status.message());
        return nullptr;
    }
    u64 column_id = column_index_entry->column_id();
    UniquePtr<CreateIndexParam> create_index_param =
        column_index_entry->GetCreateIndexParam(segment_row_count, actual_segment_row_count, table_entry->GetColumnDefByID(column_id));
    // TODO: need to get create index param;
    //    UniquePtr<CreateIndexParam> create_index_param = SegmentEntry::GetCreateIndexParam(segment_entry, index_base, column_def.get());
    auto segment_column_index_entry = LoadIndexEntry(column_index_entry, segment_id, buffer_mgr, create_index_param.get());
    if (segment_column_index_entry.get() == nullptr) {
        UnrecoverableError("Failed to load index entry");
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
