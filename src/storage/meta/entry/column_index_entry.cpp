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

module column_index_entry;

import stl;
import index_base;
import third_party;
import local_file_system;
import default_values;
import random;
import buffer_manager;
import infinity_exception;
import index_file_worker;

import catalog_delta_entry;
import annivfflat_index_file_worker;
import hnsw_file_worker;
import secondary_index_file_worker;
import logger;
import embedding_info;
import create_index_info;
import column_def;

namespace infinity {

ColumnIndexEntry::ColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                   TableIndexEntry *table_index_entry,
                                   u64 column_id,
                                   SharedPtr<String> col_index_dir,
                                   TransactionID txn_id,
                                   TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kColumnIndex), table_index_entry_(table_index_entry), column_id_(column_id), col_index_dir_(col_index_dir),
      index_base_(index_base) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

SharedPtr<ColumnIndexEntry> ColumnIndexEntry::NewColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                                                  ColumnID column_id,
                                                                  TableIndexEntry *table_index_entry,
                                                                  Txn *txn,
                                                                  TransactionID txn_id,
                                                                  SharedPtr<String> col_index_dir,
                                                                  TxnTimeStamp begin_ts) {
    auto column_index_entry = MakeShared<ColumnIndexEntry>(index_base, table_index_entry, column_id, col_index_dir, txn_id, begin_ts);
    {
        if (txn != nullptr) {
            UniquePtr<AddColumnIndexEntryOp> operation = MakeUnique<AddColumnIndexEntryOp>(column_index_entry);
            txn->AddCatalogDeltaOperation(std::move(operation));
        }
    }
    return column_index_entry;
}

SharedPtr<ColumnIndexEntry> ColumnIndexEntry::NewReplayColumnIndexEntry(TableIndexEntry *table_index_entry,
                                                                        SharedPtr<IndexBase> index_base,
                                                                        ColumnID column_id,
                                                                        SharedPtr<String> col_index_dir,
                                                                        TransactionID txn_id,
                                                                        TxnTimeStamp begin_ts,
                                                                        TxnTimeStamp commit_ts,
                                                                        bool is_delete) {
    auto column_index_entry = MakeShared<ColumnIndexEntry>(index_base, table_index_entry, column_id, col_index_dir, txn_id, begin_ts);
    return column_index_entry;
}

void ColumnIndexEntry::CommitCreatedIndex(u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry) {
    std::unique_lock<std::shared_mutex> w_locker(this->rw_locker_);
    this->index_by_segment_.emplace(segment_id, std::move(index_entry));
}

nlohmann::json ColumnIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    if (this->deleted_) {
        UnrecoverableError("Column index entry can't be deleted.");
    }

    nlohmann::json json;
    Vector<SegmentColumnIndexEntry *> segment_column_index_entry_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);

        json["txn_id"] = this->txn_id_.load();
        json["begin_ts"] = this->begin_ts_;
        json["commit_ts"] = this->commit_ts_.load();
        json["deleted"] = this->deleted_;
        json["column_id"] = this->column_id_;
        json["col_index_dir"] = *this->col_index_dir();
        json["index_base"] = this->index_base_->Serialize();

        for (const auto &[segment_id, index_entry] : this->index_by_segment_) {
            segment_column_index_entry_candidates.emplace_back((SegmentColumnIndexEntry *)index_entry.get());
        }
    }

    for (const auto &segment_column_index_entry : segment_column_index_entry_candidates) {
        json["index_by_segment"].push_back(segment_column_index_entry->Serialize());
    }

    return json;
}

SharedPtr<ColumnIndexEntry> ColumnIndexEntry::Deserialize(const nlohmann::json &column_index_entry_json,
                                                          TableIndexEntry *table_index_entry,
                                                          BufferManager *buffer_mgr,
                                                          TableEntry *table_entry) {
    bool deleted = column_index_entry_json["deleted"];
    if (deleted) {
        UnrecoverableError("Column index entry can't be deleted.");
    }

    TransactionID txn_id = column_index_entry_json["txn_id"];
    TxnTimeStamp begin_ts = column_index_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = column_index_entry_json["commit_ts"];
    u64 column_id = column_index_entry_json["column_id"];
    auto col_index_dir = MakeShared<String>(column_index_entry_json["col_index_dir"]);
    SharedPtr<IndexBase> index_base = IndexBase::Deserialize(column_index_entry_json["index_base"]);

    auto column_index_entry = MakeShared<ColumnIndexEntry>(index_base, table_index_entry, column_id, col_index_dir, txn_id, begin_ts);
    column_index_entry->commit_ts_.store(commit_ts);
    column_index_entry->deleted_ = deleted;

    if (column_index_entry_json.contains("index_by_segment")) {
        for (const auto &index_by_segment_json : column_index_entry_json["index_by_segment"]) {
            UniquePtr<SegmentColumnIndexEntry> segment_column_index_entry =
                SegmentColumnIndexEntry::Deserialize(index_by_segment_json, column_index_entry.get(), buffer_mgr, table_entry);
            column_index_entry->index_by_segment_.emplace(segment_column_index_entry->segment_id(), std::move(segment_column_index_entry));
        }
    }

    return column_index_entry;
}

void ColumnIndexEntry::Cleanup() {
    for (auto &[segment_id, segment_column_index_entry] : index_by_segment_) {
        segment_column_index_entry->Cleanup();
    }
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

Vector<UniquePtr<IndexFileWorker>> ColumnIndexEntry::CreateFileWorker(CreateIndexParam *param, u32 segment_id) {
    Vector<UniquePtr<IndexFileWorker>> vector_file_worker(1);
    // reference file_worker will be invalidated when vector_file_worker is resized
    auto &file_worker = vector_file_worker[0];
    const auto *index_base = param->index_base_;
    const auto *column_def = param->column_def_;
    auto file_name = MakeShared<String>(ColumnIndexEntry::IndexFileName(index_base->file_name_, segment_id));
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            auto create_annivfflat_param = static_cast<CreateAnnIVFFlatParam *>(param);
            auto elem_type = ((EmbeddingInfo *)(column_def->type()->type_info().get()))->Type();
            switch (elem_type) {
                case kElemFloat: {
                    file_worker = MakeUnique<AnnIVFFlatIndexFileWorker<f32>>(this->col_index_dir(),
                                                                             file_name,
                                                                             index_base,
                                                                             column_def,
                                                                             create_annivfflat_param->row_count_);
                    break;
                }
                default: {
                    UnrecoverableError("Create IVF Flat index: Unsupported element type.");
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto create_hnsw_param = static_cast<CreateHnswParam *>(param);
            file_worker = MakeUnique<HnswFileWorker>(this->col_index_dir(), file_name, index_base, column_def, create_hnsw_param->max_element_);
            break;
        }
        case IndexType::kFullText: {
            //            auto create_fulltext_param = static_cast<CreateFullTextParam *>(param);
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
            break;
        }
        case IndexType::kSecondary: {
            auto create_secondary_param = static_cast<CreateSecondaryIndexParam *>(param);
            auto const row_count = create_secondary_param->row_count_;
            auto const actual_row_count = create_secondary_param->actual_row_count_;
            auto const part_capacity = create_secondary_param->part_capacity_;
            // now we can only use row_count to calculate the part_num
            // because the actual_row_count will reduce when we delete rows
            // which will cause the index file worker count to be inconsistent when we read the index file
            u32 part_num = (row_count + part_capacity - 1) / part_capacity;
            vector_file_worker.resize(part_num + 1);
            // cannot use invalid file_worker
            vector_file_worker[0] = MakeUnique<SecondaryIndexFileWorker>(this->col_index_dir(),
                                                                         file_name,
                                                                         index_base,
                                                                         column_def,
                                                                         0,
                                                                         row_count,
                                                                         actual_row_count,
                                                                         part_capacity);
            for (u32 i = 1; i <= part_num; ++i) {
                auto part_file_name = MakeShared<String>(fmt::format("{}_part{}", *file_name, i));
                vector_file_worker[i] = MakeUnique<SecondaryIndexFileWorker>(this->col_index_dir(),
                                                                             part_file_name,
                                                                             index_base,
                                                                             column_def,
                                                                             i,
                                                                             row_count,
                                                                             actual_row_count,
                                                                             part_capacity);
            }
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    // cannot use invalid file_worker
    if (vector_file_worker[0].get() == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>("Failed to create index file worker");
        LOG_ERROR(*err_msg);
        UnrecoverableError(*err_msg);
    }
    return vector_file_worker;
}

String ColumnIndexEntry::IndexFileName(const String &index_name, u32 segment_id) { return fmt::format("seg{}.idx", segment_id, index_name); }

Status ColumnIndexEntry::CreateIndexPrepare(TableEntry *table_entry,
                                            BlockIndex *block_index,
                                            ColumnID column_id,
                                            Txn *txn,
                                            bool prepare,
                                            bool is_replay,
                                            bool check_ts) {
    const auto *column_def = table_entry->GetColumnDefByID(column_id);
    auto *txn_store = txn->GetTxnTableStore(table_entry);

    for (const auto *segment_entry : block_index->segments_) {
        // use actual_row_count to exclude the deleted rows
        auto create_index_param = GetCreateIndexParam(segment_entry->row_count(), segment_entry->actual_row_count(), column_def);
        SegmentID segment_id = segment_entry->segment_id();
        SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry =
            SegmentColumnIndexEntry::NewIndexEntry(this, segment_id, txn, create_index_param.get());
        if (!is_replay) {
            segment_column_index_entry->CreateIndexPrepare(index_base_.get(), column_id, column_def, segment_entry, txn, prepare, check_ts);
        }
        txn_store->CreateIndexFile(table_index_entry_, column_id, segment_id, segment_column_index_entry);
        index_by_segment_.emplace(segment_id, segment_column_index_entry);
    }
    return Status::OK();
}

Status ColumnIndexEntry::CreateIndexDo(const ColumnDef *column_def, HashMap<u32, atomic_u64> &create_index_idxes) {
    for (auto &[segment_id, segment_column_index_entry] : index_by_segment_) {
        atomic_u64 &create_index_idx = create_index_idxes.at(segment_id);
        auto status = segment_column_index_entry->CreateIndexDo(index_base_.get(), column_def, create_index_idx);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

UniquePtr<CreateIndexParam> ColumnIndexEntry::GetCreateIndexParam(SizeT seg_row_count, SizeT seg_actual_row_count, const ColumnDef *column_def) {
    switch (index_base_->index_type_) {
        case IndexType::kIVFFlat: {
            return MakeUnique<CreateAnnIVFFlatParam>(index_base_.get(), column_def, seg_row_count);
        }
        case IndexType::kHnsw: {
            SizeT max_element = seg_row_count;
            return MakeUnique<CreateHnswParam>(index_base_.get(), column_def, max_element);
        }
        case IndexType::kFullText: {
            return MakeUnique<CreateFullTextParam>(index_base_.get(), column_def);
        }
        case IndexType::kSecondary: {
            u32 part_capacity = DEFAULT_BLOCK_CAPACITY;
            return MakeUnique<CreateSecondaryIndexParam>(index_base_.get(), column_def, seg_row_count, seg_actual_row_count, part_capacity);
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base_->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    return nullptr;
}

} // namespace infinity
