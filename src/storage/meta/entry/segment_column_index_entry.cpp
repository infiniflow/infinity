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

import stl;
import base_entry;
// import segment_entry;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import logger;
import third_party;
import parser;
import infinity_exception;

import index_file_worker;
import annivfflat_index_file_worker;
import hnsw_file_worker;
import column_index_entry;
import table_collection_entry;
import segment_entry;

module segment_column_index_entry;

namespace infinity {
SegmentColumnIndexEntry::SegmentColumnIndexEntry(ColumnIndexEntry *column_index_entry, u32 segment_id, BufferObj *buffer)
    : BaseEntry(EntryType::kSegmentColumnIndex), column_index_entry_(column_index_entry), segment_id_(segment_id), buffer_(buffer){};

SharedPtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::NewIndexEntry(ColumnIndexEntry *column_index_entry,
                                                                          u32 segment_id,
                                                                          TxnTimeStamp create_ts,
                                                                          BufferManager *buffer_manager,
                                                                          CreateIndexParam *param) {
    // FIXME: estimate index size.
    UniquePtr<IndexFileWorker> file_worker = SegmentColumnIndexEntry::CreateFileWorker(column_index_entry, param, segment_id);
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
    UniquePtr<IndexFileWorker> file_worker = SegmentColumnIndexEntry::CreateFileWorker(column_index_entry, param, segment_id);
    auto buffer = buffer_manager->Get(Move(file_worker));
    return UniquePtr<SegmentColumnIndexEntry>(new SegmentColumnIndexEntry(column_index_entry, segment_id, buffer));
}

BufferHandle SegmentColumnIndexEntry::GetIndex(SegmentColumnIndexEntry *segment_column_index_entry, BufferManager *buffer_mgr) {
    return segment_column_index_entry->buffer_->Load();
}

void SegmentColumnIndexEntry::UpdateIndex(SegmentColumnIndexEntry *segment_column_index_entry,
                                          TxnTimeStamp commit_ts,
                                          FaissIndexPtr *index,
                                          BufferManager *buffer_mgr) {
    Error<NotImplementException>("Not implemented");
}

bool SegmentColumnIndexEntry::Flush(SegmentColumnIndexEntry *segment_column_index_entry, TxnTimeStamp checkpoint_ts) {
    String &index_name = *segment_column_index_entry->column_index_entry_->index_dir_;
    u64 segment_id = segment_column_index_entry->segment_id_;
    LOG_TRACE(Format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    if (segment_column_index_entry->max_ts_ <= segment_column_index_entry->checkpoint_ts_ || segment_column_index_entry->min_ts_ > checkpoint_ts) {
        LOG_TRACE(Format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                         segment_id,
                         index_name));
        return false;
    }
    if (segment_column_index_entry->buffer_ == nullptr) {
        LOG_WARN("Index entry is not initialized");
        return false;
    }
    if (segment_column_index_entry->buffer_->Save()) {
        segment_column_index_entry->buffer_->Sync();
        segment_column_index_entry->buffer_->CloseFile();
    }

    segment_column_index_entry->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(Format("Segment: {}, Index: {} is flushed", segment_id, index_name));
    return true;
}

Json SegmentColumnIndexEntry::Serialize(const SegmentColumnIndexEntry *segment_column_index_entry) {
    Json index_entry_json;
    index_entry_json["segment_id"] = segment_column_index_entry->segment_id_;
    index_entry_json["min_ts"] = segment_column_index_entry->min_ts_;
    index_entry_json["max_ts"] = segment_column_index_entry->max_ts_;
    index_entry_json["checkpoint_ts"] = segment_column_index_entry->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry
    return index_entry_json;
}

UniquePtr<SegmentColumnIndexEntry> SegmentColumnIndexEntry::Deserialize(const Json &index_entry_json,
                                                                        ColumnIndexEntry *column_index_entry,
                                                                        BufferManager *buffer_mgr,
                                                                        TableCollectionEntry *table_collection_entry) {
    u32 segment_id = index_entry_json["segment_id"];
    SegmentEntry *segment_entry = TableCollectionEntry::GetSegmentByID(table_collection_entry, segment_id);
    u64 column_id = column_index_entry->column_id_;
    UniquePtr<CreateIndexParam> create_index_param =
        SegmentEntry::GetCreateIndexParam(segment_entry, column_index_entry->index_base_.get(), table_collection_entry->columns_[column_id].get());
    // TODO: need to get create index param;
    //    UniquePtr<CreateIndexParam> create_index_param = SegmentEntry::GetCreateIndexParam(segment_entry, index_base, column_def.get());
    auto segment_column_index_entry = LoadIndexEntry(column_index_entry, segment_id, buffer_mgr, create_index_param.get());
    Assert<StorageException>(segment_column_index_entry.get() != nullptr, "Failed to load index entry");
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

UniquePtr<IndexFileWorker> SegmentColumnIndexEntry::CreateFileWorker(ColumnIndexEntry *column_index_entry, CreateIndexParam *param, u32 segment_id) {
    UniquePtr<IndexFileWorker> file_worker = nullptr;
    const auto *index_base = param->index_base_;
    const auto *column_def = param->column_def_;
    auto file_name = MakeShared<String>(SegmentColumnIndexEntry::IndexFileName(index_base->file_name_, segment_id));
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            auto create_annivfflat_param = static_cast<CreateAnnIVFFlatParam *>(param);
            auto elem_type = ((EmbeddingInfo *)(column_def->type()->type_info().get()))->Type();
            switch (elem_type) {
                case kElemFloat: {
                    file_worker = MakeUnique<AnnIVFFlatIndexFileWorker<f32>>(column_index_entry->index_dir_,
                                                                             file_name,
                                                                             index_base,
                                                                             column_def,
                                                                             create_annivfflat_param->row_count_);
                    break;
                }
                default: {
                    ExecutorException("Create IVF Flat index: unsupported element type.");
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto create_hnsw_param = static_cast<CreateHnswParam *>(param);
            file_worker =
                MakeUnique<HnswFileWorker>(column_index_entry->index_dir_, file_name, index_base, column_def, create_hnsw_param->max_element_);
            break;
        }
        case IndexType::kHnswLVQ: {
            UniquePtr<String> err_msg =
                    MakeUnique<String>(Format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
        case IndexType::kIRSFullText: {
            auto create_fulltext_param = static_cast<CreateFullTextParam *>(param);
            UniquePtr<String> err_msg =
                    MakeUnique<String>(Format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(Format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
    }
    if (file_worker.get() == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>("Failed to create index file worker");
        LOG_ERROR(*err_msg);
        Error<StorageException>(*err_msg);
    }
    return file_worker;
}

String SegmentColumnIndexEntry::IndexFileName(const String &index_name, u32 segment_id) { return Format("seg{}.idx", segment_id, index_name); }

} // namespace infinity
