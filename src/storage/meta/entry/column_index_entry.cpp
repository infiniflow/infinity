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

module catalog;

import stl;
import index_base;
import third_party;
import local_file_system;
import default_values;
import random;
import buffer_manager;
import infinity_exception;
import index_file_worker;
import parser;
import annivfflat_index_file_worker;
import hnsw_file_worker;
import logger;

namespace infinity {

ColumnIndexEntry::ColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                   TableIndexEntry *table_index_entry,
                                   u64 column_id,
                                   SharedPtr<String> index_dir,
                                   u64 txn_id,
                                   TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kColumnIndex), table_index_entry_(table_index_entry), column_id_(column_id), index_dir_(index_dir),
      index_base_(index_base) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

UniquePtr<ColumnIndexEntry> ColumnIndexEntry::NewColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                                                  u64 column_id,
                                                                  TableIndexEntry *table_index_entry,
                                                                  u64 txn_id,
                                                                  SharedPtr<String> index_dir,
                                                                  TxnTimeStamp begin_ts) {
    //    SharedPtr<String> index_dir =
    //        DetermineIndexDir(*TableIndexMeta::GetTableEntry(table_index_meta)->table_entry_dir_, index_base->file_name_);
    return MakeUnique<ColumnIndexEntry>(index_base, table_index_entry, column_id, index_dir, txn_id, begin_ts);
}

void ColumnIndexEntry::CommitCreatedIndex(u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry) {
    UniqueLock<RWMutex> w_locker(this->rw_locker_);
    this->index_by_segment_.emplace(segment_id, Move(index_entry));
}

Json ColumnIndexEntry::Serialize(ColumnIndexEntry *column_index_entry, TxnTimeStamp max_commit_ts) {
    if (column_index_entry->deleted_) {
        Error<StorageException>("Column index entry can't be deleted.");
    }

    Json json;
    Vector<SegmentColumnIndexEntry *> segment_column_index_entry_candidates;
    {
        SharedLock<RWMutex> lck(column_index_entry->rw_locker_);

        json["txn_id"] = column_index_entry->txn_id_.load();
        json["begin_ts"] = column_index_entry->begin_ts_;
        json["commit_ts"] = column_index_entry->commit_ts_.load();
        json["deleted"] = column_index_entry->deleted_;
        json["column_id"] = column_index_entry->column_id_;
        json["index_dir"] = *column_index_entry->index_dir();
        json["index_base"] = column_index_entry->index_base_->Serialize();

        for (const auto &[segment_id, index_entry] : column_index_entry->index_by_segment_) {
            segment_column_index_entry_candidates.emplace_back((SegmentColumnIndexEntry *)index_entry.get());
        }
    }

    for (const auto &segment_column_index_entry : segment_column_index_entry_candidates) {
        segment_column_index_entry->Flush(max_commit_ts);
        json["index_by_segment"].push_back(SegmentColumnIndexEntry::Serialize(segment_column_index_entry));
    }

    return json;
}

UniquePtr<ColumnIndexEntry> ColumnIndexEntry::Deserialize(const Json &column_index_entry_json,
                                                          TableIndexEntry *table_index_entry,
                                                          BufferManager *buffer_mgr,
                                                          TableEntry *table_entry) {
    bool deleted = column_index_entry_json["deleted"];
    if (deleted) {
        Error<StorageException>("Column index entry can't be deleted.");
    }

    u64 txn_id = column_index_entry_json["txn_id"];
    TxnTimeStamp begin_ts = column_index_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = column_index_entry_json["commit_ts"];
    u64 column_id = column_index_entry_json["column_id"];
    auto index_dir = MakeShared<String>(column_index_entry_json["index_dir"]);
    SharedPtr<IndexBase> index_base = IndexBase::Deserialize(column_index_entry_json["index_base"]);

    auto column_index_entry = MakeUnique<ColumnIndexEntry>(index_base, table_index_entry, column_id, index_dir, txn_id, begin_ts);
    column_index_entry->commit_ts_.store(commit_ts);
    column_index_entry->deleted_ = deleted;

    if (column_index_entry_json.contains("index_by_segment")) {
        for (const auto &index_by_segment_json : column_index_entry_json["index_by_segment"]) {
            UniquePtr<SegmentColumnIndexEntry> segment_column_index_entry =
                SegmentColumnIndexEntry::Deserialize(index_by_segment_json, column_index_entry.get(), buffer_mgr, table_entry);
            column_index_entry->index_by_segment_.emplace(segment_column_index_entry->segment_id(), Move(segment_column_index_entry));
        }
    }

    return column_index_entry;
}

SharedPtr<String> ColumnIndexEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    LocalFileSystem fs;
    SharedPtr<String> index_dir;
    do {
        u32 seed = time(nullptr);
        index_dir = MakeShared<String>(parent_dir + "/" + RandomString(DEFAULT_RANDOM_NAME_LEN, seed) + "_index_" + index_name);
    } while (!fs.CreateDirectoryNoExp(*index_dir));
    return index_dir;
}

UniquePtr<IndexFileWorker> ColumnIndexEntry::CreateFileWorker(CreateIndexParam *param, u32 segment_id) {
    UniquePtr<IndexFileWorker> file_worker = nullptr;
    const auto *index_base = param->index_base_;
    const auto *column_def = param->column_def_;
    auto file_name = MakeShared<String>(ColumnIndexEntry::IndexFileName(index_base->file_name_, segment_id));
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            auto create_annivfflat_param = static_cast<CreateAnnIVFFlatParam *>(param);
            auto elem_type = ((EmbeddingInfo *)(column_def->type()->type_info().get()))->Type();
            switch (elem_type) {
                case kElemFloat: {
                    file_worker = MakeUnique<AnnIVFFlatIndexFileWorker<f32>>(this->index_dir(),
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
            file_worker = MakeUnique<HnswFileWorker>(this->index_dir(), file_name, index_base, column_def, create_hnsw_param->max_element_);
            break;
        }
        case IndexType::kIRSFullText: {
            //            auto create_fulltext_param = static_cast<CreateFullTextParam *>(param);
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

String ColumnIndexEntry::IndexFileName(const String &index_name, u32 segment_id) { return Format("seg{}.idx", segment_id, index_name); }

} // namespace infinity
