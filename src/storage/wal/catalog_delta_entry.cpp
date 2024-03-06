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

#include <fstream>

module catalog_delta_entry;

import crc;
import serialize;
import data_block;
import table_def;
import index_base;
import infinity_exception;
import internal_types;
import stl;
import data_type;

import third_party;
import logger;

namespace infinity {

UniquePtr<CatalogDeltaOperation> CatalogDeltaOperation::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    UniquePtr<CatalogDeltaOperation> operation{nullptr};
    auto operation_type = ReadBufAdv<CatalogDeltaOpType>(ptr);
    auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
    switch (operation_type) {
        case CatalogDeltaOpType::ADD_DATABASE_META: {
            String db_name = ReadBufAdv<String>(ptr);
            String data_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddDBMetaOp>(begin_ts, is_delete, txn_id, commit_ts, db_name, data_dir);
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_META: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String db_entry_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddTableMetaOp>(begin_ts, is_delete, txn_id, commit_ts, std::move(db_name), table_name, db_entry_dir);
            break;
        }
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_entry_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddDBEntryOp>(begin_ts, is_delete, txn_id, commit_ts, db_name, db_entry_dir);
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_entry_dir = ReadBufAdv<String>(ptr);

            i32 columns_size = ReadBufAdv<i32>(ptr);
            Vector<SharedPtr<ColumnDef>> columns;
            for (i32 i = 0; i < columns_size; i++) {
                i64 id = ReadBufAdv<i64>(ptr);
                max_bytes = ptr_end - ptr;
                if (max_bytes <= 0) {
                    UnrecoverableError("ptr goes out of range when reading TableDef");
                }
                SharedPtr<DataType> column_type = DataType::ReadAdv(ptr, max_bytes);
                String column_name = ReadBufAdv<String>(ptr);
                i32 constraints_size = ReadBufAdv<i32>(ptr);
                HashSet<ConstraintType> constraints;
                for (i32 j = 0; j < constraints_size; j++) {
                    ConstraintType ct = ReadBufAdv<ConstraintType>(ptr);
                    constraints.insert(ct);
                }
                SharedPtr<ColumnDef> cd = MakeShared<ColumnDef>(id, column_type, column_name, constraints);
                columns.push_back(cd);
            }

            SizeT row_count = ReadBufAdv<SizeT>(ptr);
            operation = MakeUnique<AddTableEntryOp>(begin_ts,
                                                    is_delete,
                                                    txn_id,
                                                    commit_ts,
                                                    std::move(db_name),
                                                    table_name,
                                                    table_entry_dir,
                                                    columns,
                                                    row_count);
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_dir = ReadBufAdv<String>(ptr);
            u64 column_count = ReadBufAdv<u64>(ptr);
            SizeT row_count = ReadBufAdv<SizeT>(ptr);
            SizeT actual_row_count = ReadBufAdv<SizeT>(ptr);
            SizeT row_capacity = ReadBufAdv<SizeT>(ptr);
            TxnTimeStamp min_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation = MakeUnique<AddSegmentEntryOp>(begin_ts,
                                                      is_delete,
                                                      txn_id,
                                                      commit_ts,
                                                      std::move(db_name),
                                                      std::move(table_name),
                                                      segment_id,
                                                      segment_dir,
                                                      column_count,
                                                      row_count,
                                                      actual_row_count,
                                                      row_capacity,
                                                      min_row_ts,
                                                      max_row_ts);
            break;
        }
        case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            String block_dir = ReadBufAdv<String>(ptr);
            u16 row_count = ReadBufAdv<u16>(ptr);
            u16 row_capacity = ReadBufAdv<u16>(ptr);
            TxnTimeStamp min_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp checkpoint_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            u16 checkpoint_row_count = ReadBufAdv<u16>(ptr);
            operation = MakeUnique<AddBlockEntryOp>(begin_ts,
                                                    is_delete,
                                                    txn_id,
                                                    commit_ts,
                                                    std::move(db_name),
                                                    std::move(table_name),
                                                    segment_id,
                                                    block_id,
                                                    block_dir,
                                                    row_count,
                                                    row_capacity,
                                                    min_row_ts,
                                                    max_row_ts,
                                                    checkpoint_ts,
                                                    checkpoint_row_count);
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);
            i32 next_outline_idx = ReadBufAdv<i32>(ptr);
            operation = MakeUnique<AddColumnEntryOp>(begin_ts,
                                                     is_delete,
                                                     txn_id,
                                                     commit_ts,
                                                     std::move(db_name),
                                                     std::move(table_name),
                                                     segment_id,
                                                     block_id,
                                                     column_id,
                                                     next_outline_idx);
            break;
        }
        case CatalogDeltaOpType::ADD_INDEX_META: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            operation =
                MakeUnique<AddIndexMetaOp>(begin_ts, is_delete, txn_id, commit_ts, std::move(db_name), std::move(table_name), std::move(index_name));
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_dir = ReadBufAdv<String>(ptr);
            SharedPtr<IndexBase> index_base = is_delete ? nullptr : IndexBase::ReadAdv(ptr, ptr_end - ptr);
            operation = MakeUnique<AddTableIndexEntryOp>(begin_ts,
                                                         is_delete,
                                                         txn_id,
                                                         commit_ts,
                                                         std::move(db_name),
                                                         std::move(table_name),
                                                         std::move(index_name),
                                                         index_dir,
                                                         index_base);
            break;
        }
        case CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddFulltextIndexEntryOp>(begin_ts,
                                                            is_delete,
                                                            txn_id,
                                                            commit_ts,
                                                            std::move(db_name),
                                                            std::move(table_name),
                                                            std::move(index_name),
                                                            index_dir);
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String col_index_dir = ReadBufAdv<String>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);

            SharedPtr<IndexBase> index_base = is_delete ? nullptr : IndexBase::ReadAdv(ptr, ptr_end - ptr);
            operation = MakeUnique<AddColumnIndexEntryOp>(begin_ts,
                                                          is_delete,
                                                          txn_id,
                                                          commit_ts,
                                                          std::move(db_name),
                                                          std::move(table_name),
                                                          std::move(index_name),
                                                          col_index_dir,
                                                          column_id,
                                                          index_base);
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            TxnTimeStamp min_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation = MakeUnique<AddSegmentColumnIndexEntryOp>(begin_ts,
                                                                 is_delete,
                                                                 txn_id,
                                                                 commit_ts,
                                                                 std::move(db_name),
                                                                 std::move(table_name),
                                                                 std::move(index_name),
                                                                 column_id,
                                                                 segment_id,
                                                                 min_ts,
                                                                 max_ts);
            break;
        }
        default:
            UnrecoverableError(fmt::format("UNIMPLEMENTED ReadAdv for CatalogDeltaOperation type {}", int(operation_type)));
    }

    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableError("ptr goes out of range when reading CatalogDeltaOperation");
    }
    if (operation.get() == nullptr) {
        UnrecoverableError("operation is nullptr");
    }
    return operation;
}

void AddDBMetaOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->data_dir_);
}

void AddTableMetaOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->db_entry_dir_);
}

void AddDBEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_entry_dir_);
}

void AddTableEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_entry_dir_);

    WriteBufAdv(buf, (i32)(column_defs_.size()));
    SizeT column_count = column_defs_.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        const ColumnDef &cd = *column_defs_[idx];
        WriteBufAdv<i64>(buf, cd.id_);
        cd.column_type_->WriteAdv(buf);
        WriteBufAdv(buf, cd.name_);
        WriteBufAdv(buf, (i32)cd.constraints_.size());
        for (const auto &cons : cd.constraints_) {
            WriteBufAdv(buf, cons);
        }
    }
    WriteBufAdv(buf, this->row_count_);
}

void AddSegmentEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_dir_);

    WriteBufAdv(buf, this->column_count_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->actual_row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
}

void AddBlockEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->block_dir_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
    WriteBufAdv(buf, this->checkpoint_ts_);
    WriteBufAdv(buf, this->checkpoint_row_count_);
}

void AddColumnEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->next_outline_idx_);
}

void AddIndexMetaOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
}

void AddTableIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
    if (!is_delete()) {
        index_base_->WriteAdv(buf);
    }
}

void AddFulltextIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
}

void AddColumnIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->col_index_dir_);
    WriteBufAdv(buf, this->column_id_);
    if (!is_delete()) {
        index_base_->WriteAdv(buf);
    }
}

void AddSegmentColumnIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->min_ts_);
    WriteBufAdv(buf, this->max_ts_);
}

void AddDBMetaOp::SaveState() {
    this->db_name_ = *this->db_meta_->db_name();
    this->data_dir_ = *this->db_meta_->data_dir();
    is_saved_sate_ = true;
}

void AddTableMetaOp::SaveState() {
    this->table_name_ = this->table_meta_->table_name();
    this->db_entry_dir_ = this->table_meta_->db_entry_dir();
    is_saved_sate_ = true;
}

void AddDBEntryOp::SaveState() {
    this->is_delete_ = this->db_entry_->deleted_;
    this->begin_ts_ = this->db_entry_->begin_ts_;
    this->db_name_ = this->db_entry_->db_name();
    if (!this->is_delete_) {
        this->db_entry_dir_ = *this->db_entry_->db_entry_dir();
    }
    is_saved_sate_ = true;
}

void AddTableEntryOp::SaveState() {
    this->is_delete_ = this->table_entry_->deleted_;
    this->begin_ts_ = this->table_entry_->begin_ts_;
    this->table_name_ = *this->table_entry_->GetTableName();
    if (!this->is_delete_) {
        this->table_entry_dir_ = *this->table_entry_->TableEntryDir();
        this->column_defs_ = this->table_entry_->column_defs();
        this->row_count_ = this->table_entry_->row_count();
    }
    is_saved_sate_ = true;
}

void AddSegmentEntryOp::SaveState() {
    this->is_delete_ = segment_entry_->deleted_;
    this->begin_ts_ = segment_entry_->begin_ts_;
    this->segment_id_ = this->segment_entry_->segment_id();
    this->segment_dir_ = *this->segment_entry_->segment_dir();
    this->min_row_ts_ = this->segment_entry_->min_row_ts();
    this->max_row_ts_ = this->segment_entry_->max_row_ts();
    this->row_capacity_ = this->segment_entry_->row_capacity();
    this->row_count_ = this->segment_entry_->row_count();
    this->actual_row_count_ = this->segment_entry_->actual_row_count();
    this->column_count_ = this->segment_entry_->column_count();
    is_saved_sate_ = true;
}

void AddBlockEntryOp::SaveState() {
    this->is_delete_ = block_entry_->deleted_;
    this->begin_ts_ = block_entry_->begin_ts_;
    this->block_id_ = this->block_entry_->block_id();
    this->block_dir_ = this->block_entry_->DirPath();
    this->row_count_ = this->block_entry_->row_count();
    this->row_capacity_ = this->block_entry_->row_capacity();
    this->min_row_ts_ = this->block_entry_->min_row_ts();
    this->max_row_ts_ = this->block_entry_->max_row_ts();
    this->checkpoint_ts_ = this->block_entry_->checkpoint_ts();
    this->checkpoint_row_count_ = this->block_entry_->checkpoint_row_count();
    is_saved_sate_ = true;
}

void AddColumnEntryOp::SaveState() {
    this->is_delete_ = column_entry_->deleted_;
    this->begin_ts_ = column_entry_->begin_ts_;
    this->column_id_ = this->column_entry_->column_id();
    this->next_outline_idx_ = this->column_entry_->OutlineBufferCount();
    is_saved_sate_ = true;
}

/// Related to index
void AddIndexMetaOp::SaveState() { is_saved_sate_ = true; }

void AddTableIndexEntryOp::SaveState() {
    this->is_delete_ = table_index_entry_->deleted_;
    this->begin_ts_ = table_index_entry_->begin_ts_;
    if (!this->is_delete_) {
        this->index_dir_ = *this->table_index_entry_->index_dir();
        this->index_base_ = this->table_index_entry_->table_index_def();
    }
    is_saved_sate_ = true;
}

void AddFulltextIndexEntryOp::SaveState() {
    this->is_delete_ = fulltext_index_entry_->deleted_;
    this->begin_ts_ = fulltext_index_entry_->begin_ts_;
    this->index_dir_ = this->fulltext_index_entry_->index_dir();
    is_saved_sate_ = true;
}

void AddColumnIndexEntryOp::SaveState() {
    this->is_delete_ = column_index_entry_->deleted_;
    this->begin_ts_ = column_index_entry_->begin_ts_;
    this->col_index_dir_ = *this->column_index_entry_->col_index_dir();
    this->column_id_ = this->column_index_entry_->column_id();
    this->index_base_ = this->column_index_entry_->index_base();
    is_saved_sate_ = true;
}

void AddSegmentColumnIndexEntryOp::SaveState() {
    this->is_delete_ = segment_column_index_entry_->deleted_;
    this->begin_ts_ = segment_column_index_entry_->begin_ts_;
    this->segment_id_ = this->segment_column_index_entry_->segment_id();
    this->min_ts_ = this->segment_column_index_entry_->min_ts();
    this->max_ts_ = this->segment_column_index_entry_->max_ts();
    is_saved_sate_ = true;
}

const String AddDBMetaOp::ToString() const { return fmt::format("AddDBMetaOp db_name: {} data_dir: {}", db_name_, data_dir_); }

const String AddTableMetaOp::ToString() const {
    return fmt::format("AddTableMetaOp db_name: {} table_name: {} db_entry_dir: {}", *db_name_, table_name_, db_entry_dir_);
}

const String AddDBEntryOp::ToString() const { return fmt::format("AddDBEntryOp db_name: {} db_entry_dir: {}", db_name_, db_entry_dir_); }

const String AddTableEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddTableEntryOp db_name: {} table_name: {} table_entry_dir: {}", *db_name_, table_name_, table_entry_dir_);
    for (const auto &column_def : column_defs_) {
        sstream << fmt::format(" column_def: {}", column_def->ToString());
    }
    sstream << fmt::format(" row_count: {}", row_count_);
    return sstream.str();
}

const String AddSegmentEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddSegmentEntryOp begin_ts: {}, commit_ts: {}, db_name: {} table_name: {} segment_id: {} segment_dir: {}\n",
                           begin_ts_,
                           commit_ts_,
                           *db_name_,
                           *table_name_,
                           segment_id_,
                           segment_dir_);

    sstream << fmt::format(" min_row_ts: {} max_row_ts: {} row_capacity: {} row_count: {} actual_row_count: {} column_count: {}",
                           min_row_ts_,
                           max_row_ts_,
                           row_capacity_,
                           row_count_,
                           actual_row_count_,
                           column_count_);
    return sstream.str();
}

const String AddBlockEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format(
        "AddBlockEntryOp db_name: {} table_name: {} segment_id: {} block_id: {} block_dir: {} row_count: {} row_capacity: {} min_row_ts: {} "
        "max_row_ts: {}",
        *db_name_,
        *table_name_,
        segment_id_,
        block_id_,
        block_dir_,
        row_count_,
        row_capacity_,
        min_row_ts_,
        max_row_ts_);

    sstream << fmt::format(" checkpoint_ts: {} checkpoint_row_count: {}", checkpoint_ts_, checkpoint_row_count_);

    return sstream.str();
}

const String AddColumnEntryOp::ToString() const {
    return fmt::format("AddColumnEntryOp db_name: {} table_name: {} segment_id: {} block_id: {} column_id: {} next_outline_idx: {}",
                       *db_name_,
                       *table_name_,
                       segment_id_,
                       block_id_,
                       column_id_,
                       next_outline_idx_);
}

const String AddIndexMetaOp::ToString() const {
    return fmt::format("AddIndexMetaOp db_name: {} table_name: {} index_name: {}", *db_name_, *table_name_, *index_name_);
}

const String AddTableIndexEntryOp::ToString() const {
    return fmt::format("AddTableIndexEntryOp db_name: {} table_name: {} index_name: {} index_dir: {} index_base: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       index_dir_,
                       index_base_ ? index_base_->ToString() : "nullptr");
}

const String AddFulltextIndexEntryOp::ToString() const {
    return fmt::format("AddFulltextIndexEntryOp db_name: {} table_name: {} index_name: {} index_dir: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       index_dir_);
}

const String AddColumnIndexEntryOp::ToString() const {
    return fmt::format("AddColumnIndexEntryOp db_name: {} table_name: {} index_name: {} col_index_dir: {} column_id: {} index_base: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       col_index_dir_,
                       column_id_,
                       index_base_->ToString());
}

const String AddSegmentColumnIndexEntryOp::ToString() const {
    return fmt::format("AddSegmentColumnIndexEntryOp db_name: {} table_name: {} index_name: {} column_id: {} segment_id: {} min_ts: {} max_ts: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       column_id_,
                       segment_id_,
                       min_ts_,
                       max_ts_);
}

void AddSegmentEntryOp::FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    this->segment_entry_->FlushDataToDisk(max_commit_ts, is_full_checkpoint);
    LOG_TRACE("Segment has flushed to disk, now try set to deprecated if it was compacted before");
    this->segment_entry_->TrySetDeprecated();
}

void AddSegmentColumnIndexEntryOp::Flush(TxnTimeStamp max_commit_ts) { this->segment_column_index_entry_->Flush(max_commit_ts); }

/// class CatalogDeltaEntry
i32 CatalogDeltaEntry::GetSizeInBytes() const {
    i32 size = sizeof(CatalogDeltaEntryHeader) + sizeof(i32);
    SizeT operations_size = operations_.size();
    for (SizeT idx = 0; idx < operations_size; ++idx) {
        const auto &operation = operations_[idx];
        size += operation->GetSizeInBytes();
    }
    size += sizeof(i32); // pad
    return size;
}

SharedPtr<CatalogDeltaEntry> CatalogDeltaEntry::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        UnrecoverableError("ptr goes out of range when reading WalEntry");
    }
    auto entry = MakeShared<CatalogDeltaEntry>();
    auto *header = (CatalogDeltaEntryHeader *)ptr;
    entry->size_ = header->size_;
    entry->checksum_ = header->checksum_;
    entry->txn_id_ = header->txn_id_;
    entry->commit_ts_ = header->commit_ts_;
    i32 size2 = *(i32 *)(ptr + entry->size_ - sizeof(i32));
    if (entry->size_ != size2) {
        return nullptr;
    }
    header->checksum_ = 0;
    u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr), entry->size_);
    if (entry->checksum_ != checksum2) {
        return nullptr;
    }
    ptr += sizeof(CatalogDeltaEntryHeader);
    i32 cnt = ReadBufAdv<i32>(ptr);
    for (i32 i = 0; i < cnt; i++) {
        max_bytes = ptr_end - ptr;
        if (max_bytes <= 0) {
            UnrecoverableError("ptr goes out of range when reading WalEntry");
        }
        UniquePtr<CatalogDeltaOperation> operation = CatalogDeltaOperation::ReadAdv(ptr, max_bytes);
        entry->operations_.push_back(std::move(operation));
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableError("ptr goes out of range when reading WalEntry");
    }
    return entry;
}

/**
 * An entry is serialized as follows:
 * - CatalogDeltaEntryHeader
 *   - size
 *   - checksum
 *   - txn_id
 *   - commit_ts
 * - number of WalCmd
 *   - (repeated) WalCmd
 * - 4 bytes pad
 * @param ptr
 * @return void
 */
// called by bg_task
void CatalogDeltaEntry::WriteAdv(char *&ptr) const {
    char *const saved_ptr = ptr;
    std::memcpy(ptr, this, sizeof(CatalogDeltaEntryHeader));
    ptr += sizeof(CatalogDeltaEntryHeader);

    WriteBufAdv(ptr, static_cast<i32>(operations_.size()));
    SizeT operation_count = operations_.size();
    for (SizeT idx = 0; idx < operation_count; ++idx) {
        const auto &operation = operations_[idx];
        LOG_TRACE(fmt::format("!Write {}", operation->ToString()));
        char *const save_ptr = ptr;
        operation->WriteAdv(ptr);
        i32 act_size = ptr - save_ptr;
        i32 exp_size = operation->GetSizeInBytes();
        if (exp_size != act_size) {
            UnrecoverableError(fmt::format("catalog delta operation write failed, exp_size: {}, act_size: {}", exp_size, act_size));
        }
    }
    i32 size = ptr - saved_ptr + sizeof(i32);
    WriteBufAdv(ptr, size);
    auto *header = (CatalogDeltaEntryHeader *)saved_ptr;
    header->size_ = size;
    header->checksum_ = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum_ = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(saved_ptr), size);
}

// called by wal thread
void CatalogDeltaEntry::SaveState(TransactionID txn_id, TxnTimeStamp commit_ts) {
    LOG_INFO(fmt::format("SaveState txn_id {} commit_ts {}", txn_id, commit_ts));
    this->commit_ts_ = commit_ts;
    this->txn_id_ = txn_id;
    for (auto &operation : operations_) {
        LOG_TRACE(fmt::format("SaveState operation {}", operation->GetTypeStr()));
        operation->SaveState();
        operation->txn_id_ = txn_id;
        operation->commit_ts_ = commit_ts;
    }
}

String CatalogDeltaEntry::ToString() const {
    std::stringstream sstream;
    for (const auto &operation : operations_) {
        sstream << operation->ToString() << '\n';
    }
    return sstream.str();
}

UniquePtr<CatalogDeltaEntry> CatalogDeltaEntry::PickFlushEntry(TxnTimeStamp max_commit_ts) {
    std::lock_guard<std::mutex> lock(mtx_);

    auto flush_delta_entry = MakeUnique<CatalogDeltaEntry>();
    for (auto &op : operations_) {
        TxnTimeStamp op_commit_ts = op->commit_ts();
        if (op_commit_ts <= max_commit_ts) {
            flush_delta_entry->operations_.emplace_back(std::move(op));
        } else {
            break; // commit_ts is in ascending order
        }
    }
    auto flush_size = flush_delta_entry->operations_.size();
    operations_.erase(operations_.begin(), operations_.begin() + flush_size);
    return flush_delta_entry;
}

// called by bg_task
void GlobalCatalogDeltaEntry::Merge(UniquePtr<CatalogDeltaEntry> other) {
    // FIXME: should make timestamp increase ?
    std::lock_guard<std::mutex> lock(mtx_);

    auto &global_operations = this->operations();
    auto &local_operations = other->operations();

    // TODO: optimize this merge process
    // add the local operations to the global operations
    for (auto &local_operation : local_operations) {
        global_operations.push_back(std::move(local_operation));
    }

    local_operations.clear();
    this->set_txn_id(other->txn_id());
    this->set_commit_ts(other->commit_ts());
}
} // namespace infinity
