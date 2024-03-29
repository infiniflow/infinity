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

#include "type/complex/row_id.h"
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
    switch (operation_type) {
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            operation = AddDBEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            operation = AddTableEntryOp::ReadAdv(ptr, ptr_end);
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            operation = AddSegmentEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
            operation = AddBlockEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
            operation = AddColumnEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            operation = AddTableIndexEntryOp::ReadAdv(ptr, ptr_end);
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
            operation = AddSegmentIndexEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY: {
            operation = AddChunkIndexEntryOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
            operation = SetSegmentStatusSealedOp::ReadAdv(ptr);
            break;
        }
        case CatalogDeltaOpType::SET_BLOCK_STATUS_SEALED: {
            operation = SetBlockStatusSealedOp::ReadAdv(ptr);
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

bool CatalogDeltaOperation::operator==(const CatalogDeltaOperation &rhs) const {
    return this->begin_ts_ == rhs.begin_ts_ && this->txn_id_ == rhs.txn_id_ && this->commit_ts_ == rhs.commit_ts_ &&
           this->is_delete_ == rhs.is_delete_ && this->type_ == rhs.type_;
}

UniquePtr<AddDBEntryOp> AddDBEntryOp::ReadAdv(char *&ptr) {
    auto add_db_op = MakeUnique<AddDBEntryOp>();
    add_db_op->ReadAdvBase(ptr);

    add_db_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_db_op->db_entry_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    return add_db_op;
}

UniquePtr<AddTableEntryOp> AddTableEntryOp::ReadAdv(char *&ptr, char *ptr_end) {
    auto add_table_op = MakeUnique<AddTableEntryOp>();
    add_table_op->ReadAdvBase(ptr);

    add_table_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_op->table_entry_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));

    i32 columns_size = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnDef>> columns;
    for (i32 i = 0; i < columns_size; i++) {
        i64 id = ReadBufAdv<i64>(ptr);
        SizeT max_bytes = ptr_end - ptr;
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
    add_table_op->column_defs_ = std::move(columns);
    add_table_op->row_count_ = ReadBufAdv<SizeT>(ptr);
    add_table_op->unsealed_id_ = ReadBufAdv<SegmentID>(ptr);
    return add_table_op;
}

UniquePtr<AddSegmentEntryOp> AddSegmentEntryOp::ReadAdv(char *&ptr) {
    auto add_segment_op = MakeUnique<AddSegmentEntryOp>();
    add_segment_op->ReadAdvBase(ptr);

    add_segment_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_segment_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_segment_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_segment_op->status_ = ReadBufAdv<SegmentStatus>(ptr);
    add_segment_op->column_count_ = ReadBufAdv<u64>(ptr);
    add_segment_op->row_count_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->actual_row_count_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->row_capacity_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->min_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->max_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->deprecate_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    return add_segment_op;
}

UniquePtr<AddBlockEntryOp> AddBlockEntryOp::ReadAdv(char *&ptr) {
    auto add_block_op = MakeUnique<AddBlockEntryOp>();
    add_block_op->ReadAdvBase(ptr);

    add_block_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_block_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_block_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_block_op->block_id_ = ReadBufAdv<BlockID>(ptr);
    add_block_op->row_count_ = ReadBufAdv<u16>(ptr);
    add_block_op->row_capacity_ = ReadBufAdv<u16>(ptr);
    add_block_op->min_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->max_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->checkpoint_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->checkpoint_row_count_ = ReadBufAdv<u16>(ptr);
    return add_block_op;
}

UniquePtr<AddColumnEntryOp> AddColumnEntryOp::ReadAdv(char *&ptr) {
    auto add_column_op = MakeUnique<AddColumnEntryOp>();
    add_column_op->ReadAdvBase(ptr);

    add_column_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_column_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_column_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_column_op->block_id_ = ReadBufAdv<BlockID>(ptr);
    add_column_op->column_id_ = ReadBufAdv<ColumnID>(ptr);
    add_column_op->next_outline_idx_ = ReadBufAdv<i32>(ptr);
    return add_column_op;
}

UniquePtr<AddTableIndexEntryOp> AddTableIndexEntryOp::ReadAdv(char *&ptr, char *ptr_end) {
    auto add_table_index_op = MakeUnique<AddTableIndexEntryOp>();
    add_table_index_op->ReadAdvBase(ptr);

    add_table_index_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->index_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->index_dir_ = add_table_index_op->is_delete_ ? nullptr : MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->index_base_ = add_table_index_op->is_delete_ ? nullptr : IndexBase::ReadAdv(ptr, ptr_end - ptr);
    return add_table_index_op;
}

UniquePtr<AddSegmentIndexEntryOp> AddSegmentIndexEntryOp::ReadAdv(char *&ptr) {
    auto add_segment_index_op = MakeUnique<AddSegmentIndexEntryOp>();
    add_segment_index_op->ReadAdvBase(ptr);

    add_segment_index_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_segment_index_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_segment_index_op->index_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_segment_index_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_segment_index_op->min_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_index_op->max_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    return add_segment_index_op;
}

UniquePtr<AddChunkIndexEntryOp> AddChunkIndexEntryOp::ReadAdv(char *&ptr) {
    auto add_chunk_index_op = MakeUnique<AddChunkIndexEntryOp>();
    add_chunk_index_op->ReadAdvBase(ptr);

    add_chunk_index_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->index_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_chunk_index_op->base_name_ = ReadBufAdv<String>(ptr);
    add_chunk_index_op->base_rowid_ = RowID::FromUint64(ReadBufAdv<u64>(ptr));
    add_chunk_index_op->row_count_ = ReadBufAdv<u32>(ptr);
    return add_chunk_index_op;
}

UniquePtr<SetSegmentStatusSealedOp> SetSegmentStatusSealedOp::ReadAdv(char *&ptr) {
    auto set_segment_status_sealed_op = MakeUnique<SetSegmentStatusSealedOp>();
    set_segment_status_sealed_op->ReadAdvBase(ptr);

    set_segment_status_sealed_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    set_segment_status_sealed_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    set_segment_status_sealed_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    set_segment_status_sealed_op->segment_filter_binary_data_ = ReadBufAdv<String>(ptr);
    return set_segment_status_sealed_op;
}

UniquePtr<SetBlockStatusSealedOp> SetBlockStatusSealedOp::ReadAdv(char *&ptr) {
    auto set_block_status_sealed_op = MakeUnique<SetBlockStatusSealedOp>();
    set_block_status_sealed_op->ReadAdvBase(ptr);

    set_block_status_sealed_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    set_block_status_sealed_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    set_block_status_sealed_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    set_block_status_sealed_op->block_id_ = ReadBufAdv<BlockID>(ptr);
    set_block_status_sealed_op->block_filter_binary_data_ = ReadBufAdv<String>(ptr);
    return set_block_status_sealed_op;
}

void AddDBEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->db_entry_dir_);
}

void AddTableEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->table_entry_dir_);

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
    WriteBufAdv(buf, this->unsealed_id_);
}

void AddSegmentEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->status_);
    WriteBufAdv(buf, this->column_count_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->actual_row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
    WriteBufAdv(buf, this->deprecate_ts_);
}

void AddBlockEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
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

void AddTableIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    if (!is_delete_) {
        WriteBufAdv(buf, *this->index_dir_);
        index_base_->WriteAdv(buf);
    }
}

void AddSegmentIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->min_ts_);
    WriteBufAdv(buf, this->max_ts_);
}

void AddChunkIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->base_name_);
    WriteBufAdv(buf, this->base_rowid_.ToUint64());
    WriteBufAdv(buf, this->row_count_);
}

void SetSegmentStatusSealedOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
}

void SetBlockStatusSealedOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->block_filter_binary_data_);
}

const String AddDBEntryOp::ToString() const {
    return fmt::format("AddDBEntryOp db_name: {} db_entry_dir: {}", *db_name_, db_entry_dir_.get() != nullptr ? *db_entry_dir_ : "nullptr");
}

const String AddTableEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddTableEntryOp db_name: {} table_name: {} table_entry_dir: {}",
                           *db_name_,
                           *table_name_,
                           table_entry_dir_.get() != nullptr ? *table_entry_dir_ : "nullptr");
    for (const auto &column_def : column_defs_) {
        sstream << fmt::format(" column_def: {}", column_def->ToString());
    }
    sstream << fmt::format(" row_count: {}", row_count_);
    return sstream.str();
}

const String AddSegmentEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddSegmentEntryOp begin_ts: {}, commit_ts: {}, db_name: {} table_name: {} segment_id: {}\n",
                           begin_ts_,
                           commit_ts_,
                           *db_name_,
                           *table_name_,
                           segment_id_);

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
    sstream << fmt::format("AddBlockEntryOp db_name: {} table_name: {} segment_id: {} block_id: {} row_count: {} row_capacity: {} min_row_ts: {} "
                           "max_row_ts: {}",
                           *db_name_,
                           *table_name_,
                           segment_id_,
                           block_id_,
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

const String AddTableIndexEntryOp::ToString() const {
    return fmt::format("AddTableIndexEntryOp db_name: {} table_name: {} index_name: {} index_dir: {} index_base: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       index_dir_.get() != nullptr ? *index_dir_ : "nullptr",
                       index_base_.get() != nullptr ? index_base_->ToString() : "nullptr");
}

const String AddSegmentIndexEntryOp::ToString() const {
    return fmt::format("AddSegmentIndexEntryOp db_name: {} table_name: {} index_name: {} segment_id: {} min_ts: {} max_ts: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       segment_id_,
                       min_ts_,
                       max_ts_);
}

const String AddChunkIndexEntryOp::ToString() const {
    return fmt::format("AddChunkIndexEntryOp db_name: {} table_name: {} index_name: {} segment_id: {} base_name: {} base_rowid: {} row_count: {}",
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       segment_id_,
                       base_name_,
                       base_rowid_.ToUint64(),
                       row_count_);
}

const String SetSegmentStatusSealedOp::ToString() const {
    return fmt::format("SetSegmentStatusSealedOp db_name: {} table_name: {} segment_id: {}", *db_name_, *table_name_, segment_id_);
}

const String SetBlockStatusSealedOp::ToString() const {
    return fmt::format("SetBlockStatusSealedOp db_name: {} table_name: {} segment_id: {} block_id: {}",
                       *db_name_,
                       *table_name_,
                       segment_id_,
                       block_id_);
}

bool AddDBEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddDBEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*db_entry_dir_, *rhs_op->db_entry_dir_);
}

bool AddTableEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddTableEntryOp *>(&rhs);
    bool res = rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
               IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*table_entry_dir_, *rhs_op->table_entry_dir_) &&
               table_entry_type_ == rhs_op->table_entry_type_ && row_count_ == rhs_op->row_count_ && unsealed_id_ == rhs_op->unsealed_id_ &&
               column_defs_.size() == rhs_op->column_defs_.size();
    if (!res) {
        return false;
    }
    for (size_t i = 0; i < column_defs_.size(); ++i) {
        if (!(*column_defs_[i] == *rhs_op->column_defs_[i])) {
            return false;
        }
    }
    return true;
}

bool AddSegmentEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddSegmentEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ && status_ == rhs_op->status_ &&
           column_count_ == rhs_op->column_count_ && row_count_ == rhs_op->row_count_ && actual_row_count_ == rhs_op->actual_row_count_ &&
           row_capacity_ == rhs_op->row_capacity_ && min_row_ts_ == rhs_op->min_row_ts_ && max_row_ts_ == rhs_op->max_row_ts_ &&
           deprecate_ts_ == rhs_op->deprecate_ts_;
}

bool AddBlockEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddBlockEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ && block_id_ == rhs_op->block_id_ &&
           row_count_ == rhs_op->row_count_ && row_capacity_ == rhs_op->row_capacity_ && min_row_ts_ == rhs_op->min_row_ts_ &&
           max_row_ts_ == rhs_op->max_row_ts_ && checkpoint_ts_ == rhs_op->checkpoint_ts_ && checkpoint_row_count_ == rhs_op->checkpoint_row_count_;
}

bool AddColumnEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddColumnEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ && block_id_ == rhs_op->block_id_ &&
           column_id_ == rhs_op->column_id_ && next_outline_idx_ == rhs_op->next_outline_idx_;
}

bool AddTableIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddTableIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_) && IsEqual(*index_dir_, *rhs_op->index_dir_) &&
           *index_base_ == *rhs_op->index_base_;
}

bool AddSegmentIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddSegmentIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_) && segment_id_ == rhs_op->segment_id_ &&
           min_ts_ == rhs_op->min_ts_ && max_ts_ == rhs_op->max_ts_;
}

bool AddChunkIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddChunkIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_) && segment_id_ == rhs_op->segment_id_ &&
           base_name_ == rhs_op->base_name_ && base_rowid_ == rhs_op->base_rowid_ && row_count_ == rhs_op->row_count_;
}

bool SetSegmentStatusSealedOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const SetSegmentStatusSealedOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ &&
           segment_filter_binary_data_ == rhs_op->segment_filter_binary_data_;
}

bool SetBlockStatusSealedOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const SetBlockStatusSealedOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ && block_id_ == rhs_op->block_id_ &&
           block_filter_binary_data_ == rhs_op->block_filter_binary_data_;
}

void AddDBEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_DATABASE_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddDBEntryOp *>(other.get()));
}

void AddTableEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_TABLE_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_table_op = static_cast<AddTableEntryOp *>(other.get());
    *this = std::move(*add_table_op);
}

void AddSegmentEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_segment_op = static_cast<AddSegmentEntryOp *>(other.get());
    *this = std::move(*add_segment_op);
}

void AddBlockEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_BLOCK_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_block_op = static_cast<AddBlockEntryOp *>(other.get());
    *this = std::move(*add_block_op);
}

void AddColumnEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_COLUMN_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddColumnEntryOp *>(other.get()));
}

void AddTableIndexEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddTableIndexEntryOp *>(other.get()));
}

void AddSegmentIndexEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddSegmentIndexEntryOp *>(other.get()));
}

void AddChunkIndexEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddChunkIndexEntryOp *>(other.get()));
}

void SetSegmentStatusSealedOp::Merge(UniquePtr<CatalogDeltaOperation> other) { UnrecoverableError("SetSegmentStatusSealedOp::Merge not allowed"); }

void SetBlockStatusSealedOp::Merge(UniquePtr<CatalogDeltaOperation> other) { UnrecoverableError("SetBlockStatusSealedOp::Merge not allowed"); }

void AddBlockEntryOp::FlushDataToDisk(TxnTimeStamp max_commit_ts) {
    LOG_TRACE(fmt::format("BlockEntry {} flush to disk", block_entry_->block_id()));
    block_entry_->Flush(max_commit_ts);
}

void AddSegmentIndexEntryOp::Flush(TxnTimeStamp max_commit_ts) { segment_index_entry_->Flush(max_commit_ts); }

/// class CatalogDeltaEntry
i32 CatalogDeltaEntry::GetSizeInBytes() const {
    i32 size = CatalogDeltaEntryHeader::GetSizeInBytes();

    size += sizeof(i32); // number of operations
    SizeT operations_size = operations_.size();
    for (SizeT idx = 0; idx < operations_size; ++idx) {
        const auto &operation = operations_[idx];
        size += operation->GetSizeInBytes();
    }
    size += sizeof(i32); // pad
    return size;
}

UniquePtr<CatalogDeltaEntry> CatalogDeltaEntry::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_start = ptr;
    char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        UnrecoverableError("ptr goes out of range when reading WalEntry");
    }
    CatalogDeltaEntryHeader header;
    header.size_ = ReadBufAdv<i32>(ptr);
    header.checksum_ = ReadBufAdv<u32>(ptr);
    auto entry = MakeUnique<CatalogDeltaEntry>();
    i32 size2 = *(i32 *)(ptr_start + header.size_ - sizeof(i32));
    if (header.size_ != size2) {
        return nullptr;
    }
    {
        ptr = ptr_start;
        WriteBufAdv(ptr, header.size_);
        u32 init_checksum = 0;
        WriteBufAdv(ptr, init_checksum);
        u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr_start), header.size_);
        if (header.checksum_ != checksum2) {
            UnrecoverableError(fmt::format("checksum failed, checksum: {}, checksum2: {}", header.checksum_, checksum2));
        }
    }
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
    {
        for (const auto &operation : entry->operations_) {
            LOG_INFO(fmt::format("Read delta op: {}", operation->ToString()));
        }
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
void CatalogDeltaEntry::WriteAdv(char *&ptr) {
    char *const saved_ptr = ptr;
    ptr += CatalogDeltaEntryHeader::GetSizeInBytes();

    SizeT operation_count = operations_.size();
    WriteBufAdv(ptr, static_cast<i32>(operation_count));
    for (SizeT idx = 0; idx < operation_count; ++idx) {
        const auto &operation = operations_[idx];
        i32 exp_size = operation->GetSizeInBytes();
        LOG_TRACE(fmt::format("!Write {}", operation->ToString()));
        char *const save_ptr = ptr;
        operation->WriteAdv(ptr);
        i32 act_size = ptr - save_ptr;
        if (exp_size != act_size) {
            UnrecoverableError(fmt::format("catalog delta operation write failed, exp_size: {}, act_size: {}", exp_size, act_size));
        }
    }
    i32 size = ptr - saved_ptr + sizeof(i32);
    WriteBufAdv(ptr, size);

    { // write head
        char *ptr1 = saved_ptr;
        WriteBufAdv(ptr1, size);
        char *ptr2 = ptr1;
        u32 init_checksum = 0;
        WriteBufAdv(ptr1, init_checksum);
        // CRC32IEEE is equivalent to boost::crc_32_type on little-endian machine.
        u32 crc = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(saved_ptr), size);
        WriteBufAdv(ptr2, crc);
    }
}

// called by wal thread
void CatalogDeltaEntry::SaveState(TransactionID txn_id, TxnTimeStamp commit_ts) {
    LOG_TRACE(fmt::format("SaveState txn_id {} commit_ts {}", txn_id, commit_ts));
    if (max_commit_ts_ != UNCOMMIT_TS || !txn_ids_.empty()) {
        UnrecoverableError(fmt::format("CatalogDeltaEntry SaveState failed, max_commit_ts_ {} txn_ids_ size {}", max_commit_ts_, txn_ids_.size()));
    }
    max_commit_ts_ = commit_ts;
    txn_ids_ = {txn_id};
    for (auto &operation : operations_) {
        operation->begin_ts_ = txn_id;
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

void GlobalCatalogDeltaEntry::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size) {
    // {
    //     for (auto &delta_entry : delta_entries) {
    //         LOG_INFO(fmt::format("Add delta entry: {}", delta_entry->ToString()));
    //     }
    // }
    std::unique_lock w_lock(mtx_);
    TxnTimeStamp current_commit_ts = delta_entry->commit_ts();

    TxnTimeStamp max_commit_ts = delta_entry->commit_ts();
    if (max_commit_ts_ > max_commit_ts || wal_size_ > wal_size) {
        UnrecoverableError(
            fmt::format("max_commit_ts_ {} > max_commit_ts {} or wal_size_ {} > wal_size {}", max_commit_ts_, max_commit_ts, wal_size_, wal_size));
    }
    max_commit_ts_ = max_commit_ts;
    wal_size_ = wal_size;

    for (auto &op : delta_entry->operations()) {
        bool prune = PruneDeltaOp(op.get(), current_commit_ts);

        String encode = op->EncodeIndex();
        auto iter = delta_ops_.find(encode);
        if (iter != delta_ops_.end()) {
            auto &delta_op = iter->second;
            if (!prune) {
                delta_op->Merge(std::move(op));
            } else {
                delta_ops_.erase(iter);
            }
        } else {
            delta_ops_[encode] = std::move(op);
        }
    }
    if (!delta_entry->txn_ids().empty()) {
        txn_ids_.insert(delta_entry->txn_ids()[0]);
    }
}

UniquePtr<CatalogDeltaEntry> GlobalCatalogDeltaEntry::PickFlushEntry(TxnTimeStamp full_ckp_ts, TxnTimeStamp max_commit_ts) {
    auto flush_delta_entry = MakeUnique<CatalogDeltaEntry>();
    if (max_commit_ts != max_commit_ts_) {
        UnrecoverableError(fmt::format("max_commit_ts {} != max_commit_ts_ {}", max_commit_ts, max_commit_ts_));
    }

    {
        std::unique_lock w_lock(mtx_);

        for (auto &[_, delta_op] : delta_ops_) {
            if (delta_op->commit_ts_ < full_ckp_ts) {
                continue;
            }
            flush_delta_entry->AddOperation(std::move(delta_op));
        }

        flush_delta_entry->set_txn_ids(Vector<TransactionID>(txn_ids_.begin(), txn_ids_.end()));

        txn_ids_.clear();
        delta_ops_.clear();
    }
    flush_delta_entry->set_commit_ts(max_commit_ts);

    // write delta op from top to bottom.
    std::sort(flush_delta_entry->operations().begin(), flush_delta_entry->operations().end(), [](const auto &lhs, const auto &rhs) {
        return lhs->type_ < rhs->type_;
    });
    // {
    //     for (const auto &operation : flush_delta_entry->operations()) {
    //         LOG_INFO(fmt::format("Flush delta op: {}", operation->ToString()));
    //     }
    // }
    return flush_delta_entry;
}

bool GlobalCatalogDeltaEntry::PruneDeltaOp(CatalogDeltaOperation *delta_op, TxnTimeStamp current_commit_ts) {
    bool prune = false;
    switch (delta_op->type_) {
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            auto *op = static_cast<AddDBEntryOp *>(delta_op);
            if (op->is_delete_) {
                prune = true;
            }
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            auto *op = static_cast<AddTableEntryOp *>(delta_op);
            if (op->is_delete_) {
                prune = true;
            }
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            auto *op = static_cast<AddTableIndexEntryOp *>(delta_op);
            if (op->is_delete_) {
                prune = true;
            }
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            auto *op = static_cast<AddSegmentEntryOp *>(delta_op);
            if (op->status_ == SegmentStatus::kDeprecated) {
                prune = true;
            }
            break;
        }
        default: {
            // skip
        }
    }
    if (prune) {
        String encode = delta_op->EncodeIndex();
        PruneOpWithSamePrefix(encode, current_commit_ts);
    }
    return prune;
}

void GlobalCatalogDeltaEntry::PruneOpWithSamePrefix(const String &encode1, TxnTimeStamp current_commit_ts) {
    SizeT end_i = encode1.rfind('@');
    if (end_i == String::npos) {
        UnrecoverableError(fmt::format("encode1 {} not found '@'", encode1));
    }
    auto end1 = encode1.begin() + end_i;
    auto encode1_prefix = encode1.substr(0, end_i);

    auto iter = delta_ops_.lower_bound(encode1_prefix);
    while (iter != delta_ops_.end()) {
        const auto &[encode2, delta_op2] = *iter;
        auto [iter1, iter2] = std::mismatch(encode1.begin(), end1, encode2.begin());
        if (iter1 != end1) {
            break;
        }
        if (iter2 == encode2.end()) { // encode == encode1
            ++iter;
            continue;
        }
        if (*iter2 != '#' && *iter2 != '@') {
            ++iter;
            continue;
        }
        if (*iter2 == '@') {
            auto type1 = static_cast<CatalogDeltaOpType>(std::stoi(encode1.substr(end_i + 1)));
            auto type2 = static_cast<CatalogDeltaOpType>(std::stoi(encode2.substr(iter2 - encode2.begin() + 1)));
            if (type1 == type2) { // same
                ++iter;
                continue;
            }
        }
        iter = delta_ops_.erase(iter);
    }
}

} // namespace infinity
