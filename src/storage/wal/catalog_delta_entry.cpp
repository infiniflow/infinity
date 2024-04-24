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
import parsed_expr;
import constant_expr;
import stl;
import data_type;

import third_party;
import logger;

namespace infinity {

CatalogDeltaOperation::CatalogDeltaOperation(CatalogDeltaOpType type, BaseEntry *base_entry, TxnTimeStamp commit_ts)
    : begin_ts_(base_entry->begin_ts_), txn_id_(base_entry->txn_id_), commit_ts_(base_entry->commit_ts_), type_(type) {
    if (base_entry->deleted_) {
        merge_flag_ = MergeFlag::kDelete;
    } else if (commit_ts == base_entry->commit_ts_) {
        merge_flag_ = MergeFlag::kNew;
    } else if (!base_entry->Committed()) {
        UnrecoverableError("Entry not committed.");
    } else {
        if (commit_ts < base_entry->commit_ts_) {
            UnrecoverableError(fmt::format("Invalid commit_ts: {} < {}", commit_ts, base_entry->commit_ts_));
        }
        merge_flag_ = MergeFlag::kUpdate;
    }
}

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
           this->merge_flag_ == rhs.merge_flag_ && this->type_ == rhs.type_;
}

PruneFlag CatalogDeltaOperation::ToPrune(Optional<MergeFlag> old_merge_flag_opt, MergeFlag new_merge_flag) {
    if (!old_merge_flag_opt.has_value()) {
        switch (new_merge_flag) {
            case MergeFlag::kDelete:
            case MergeFlag::kDeleteAndNew: {
                return PruneFlag::kPruneSub;
            }
            case MergeFlag::kNew:
            case MergeFlag::kUpdate: {
                return PruneFlag::kKeep;
            }
            default: {
                UnrecoverableError(fmt::format("Invalid MergeFlag: {}", u8(new_merge_flag)));
            }
        }
    }
    MergeFlag old_merge_flag = old_merge_flag_opt.value();
    switch (new_merge_flag) {
        case MergeFlag::kDelete: {
            switch (old_merge_flag) {
                case MergeFlag::kNew: {
                    return PruneFlag::kPrune;
                }
                case MergeFlag::kUpdate:
                case MergeFlag::kDeleteAndNew: {
                    return PruneFlag::kPruneSub;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid MergeFlag: {}", u8(old_merge_flag)));
                }
            }
            break;
        }
        case MergeFlag::kNew:
        case MergeFlag::kUpdate: {
            return PruneFlag::kKeep;
        }
        case MergeFlag::kDeleteAndNew: {
            switch (old_merge_flag) {
                case MergeFlag::kNew:
                case MergeFlag::kUpdate:
                case MergeFlag::kDeleteAndNew: {
                    return PruneFlag::kPruneSub;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid MergeFlag: {}", u8(old_merge_flag)));
                }
            }
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid MergeFlag: {}", u8(new_merge_flag)));
        }
    }
    return PruneFlag::kInvalid;
}

MergeFlag CatalogDeltaOperation::NextDeleteFlag(MergeFlag new_merge_flag) const {
    switch (this->merge_flag_) {
        case MergeFlag::kDelete: {
            switch (new_merge_flag) {
                case MergeFlag::kNew: {
                    return MergeFlag::kDeleteAndNew;
                }
                default: {
                    LOG_CRITICAL(fmt::format("Invalid MergeFlag {}", this->ToString()));
                    UnrecoverableError(fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag)));
                }
            }
            break;
        }
        case MergeFlag::kNew: {
            switch (new_merge_flag) {
                case MergeFlag::kDelete: {
                    UnrecoverableError("Should prune before reach this.");
                    break;
                }
                case MergeFlag::kNew:
                case MergeFlag::kUpdate:
                case MergeFlag::kDeleteAndNew: {
                    return MergeFlag::kNew;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag)));
                }
            }
            break;
        }
        case MergeFlag::kUpdate: {
            switch (new_merge_flag) {
                case MergeFlag::kDelete: {
                    return MergeFlag::kDelete;
                }
                case MergeFlag::kUpdate: {
                    return MergeFlag::kUpdate;
                }
                case MergeFlag::kDeleteAndNew: {
                    return MergeFlag::kNew;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag)));
                }
            }
            break;
        }
        case MergeFlag::kDeleteAndNew: {
            switch (new_merge_flag) {
                case MergeFlag::kDelete: {
                    return MergeFlag::kDelete;
                }
                case MergeFlag::kUpdate:
                case MergeFlag::kDeleteAndNew: {
                    return MergeFlag::kDeleteAndNew;
                }
                default: {
                    UnrecoverableError(fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag)));
                }
            }
        }
        default: {
            UnrecoverableError(fmt::format("Invalid MergeFlag: {}", u8(this->merge_flag_)));
        }
    }
    return MergeFlag::kInvalid;
};

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
        SharedPtr<ParsedExpr> default_expr = ConstantExpr::ReadAdv(ptr, max_bytes);
        SharedPtr<ColumnDef> cd = MakeShared<ColumnDef>(id, column_type, column_name, constraints, std::move(default_expr));
        columns.push_back(cd);
    }
    add_table_op->column_defs_ = std::move(columns);
    add_table_op->row_count_ = ReadBufAdv<SizeT>(ptr);
    add_table_op->unsealed_id_ = ReadBufAdv<SegmentID>(ptr);
    add_table_op->next_segment_id_ = ReadBufAdv<SegmentID>(ptr);
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
    add_segment_op->segment_filter_binary_data_ = ReadBufAdv<String>(ptr);
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
    add_block_op->block_filter_binary_data_ = ReadBufAdv<String>(ptr);
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
    add_column_op->last_chunk_offset_ = ReadBufAdv<u64>(ptr);
    return add_column_op;
}

UniquePtr<AddTableIndexEntryOp> AddTableIndexEntryOp::ReadAdv(char *&ptr, char *ptr_end) {
    auto add_table_index_op = MakeUnique<AddTableIndexEntryOp>();
    add_table_index_op->ReadAdvBase(ptr);

    add_table_index_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_table_index_op->index_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    if (add_table_index_op->merge_flag_ != MergeFlag::kDelete) {
        add_table_index_op->index_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));
        add_table_index_op->index_base_ = IndexBase::ReadAdv(ptr, ptr_end - ptr);
    }
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
    add_segment_index_op->next_chunk_id_ = ReadBufAdv<ChunkID>(ptr);
    return add_segment_index_op;
}

UniquePtr<AddChunkIndexEntryOp> AddChunkIndexEntryOp::ReadAdv(char *&ptr) {
    auto add_chunk_index_op = MakeUnique<AddChunkIndexEntryOp>();
    add_chunk_index_op->ReadAdvBase(ptr);

    add_chunk_index_op->db_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->table_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->index_name_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    add_chunk_index_op->segment_id_ = ReadBufAdv<SegmentID>(ptr);
    add_chunk_index_op->chunk_id_ = ReadBufAdv<ChunkID>(ptr);
    add_chunk_index_op->base_name_ = ReadBufAdv<String>(ptr);
    add_chunk_index_op->base_rowid_ = RowID::FromUint64(ReadBufAdv<u64>(ptr));
    add_chunk_index_op->row_count_ = ReadBufAdv<u32>(ptr);
    return add_chunk_index_op;
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
        (dynamic_cast<ConstantExpr *>(cd.default_expr_.get()))->WriteAdv(buf);
    }
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->unsealed_id_);
    WriteBufAdv(buf, this->next_segment_id_);
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
    WriteBufAdv(buf, this->segment_filter_binary_data_);
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
    WriteBufAdv(buf, this->block_filter_binary_data_);
}

void AddColumnEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->next_outline_idx_);
    WriteBufAdv(buf, this->last_chunk_offset_);
}

void AddTableIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    if (merge_flag_ != MergeFlag::kDelete) {
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
    WriteBufAdv(buf, this->next_chunk_id_);
}

void AddChunkIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->chunk_id_);
    WriteBufAdv(buf, this->base_name_);
    WriteBufAdv(buf, this->base_rowid_.ToUint64());
    WriteBufAdv(buf, this->row_count_);
}

const String AddDBEntryOp::ToString() const {
    return fmt::format("AddDBEntryOp {}  db_name: {} db_entry_dir: {}",
                       CatalogDeltaOperation::ToString(),
                       *db_name_,
                       db_entry_dir_.get() != nullptr ? *db_entry_dir_ : "nullptr");
}

const String AddTableEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddTableEntryOp {} db_name: {} table_name: {} table_entry_dir: {}",
                           CatalogDeltaOperation::ToString(),
                           *db_name_,
                           *table_name_,
                           table_entry_dir_.get() != nullptr ? *table_entry_dir_ : "nullptr");
    for (const auto &column_def : column_defs_) {
        sstream << fmt::format(" column_def: {}", column_def->ToString());
    }
    sstream << fmt::format(" row_count: {}", row_count_) << fmt::format(" unsealed_id: {}", unsealed_id_)
            << fmt::format(" next_segment_id: {}", next_segment_id_);
    return sstream.str();
}

const String AddSegmentEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddSegmentEntryOp {} db_name: {} table_name: {} segment_id: {} ",
                           CatalogDeltaOperation::ToString(),
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
    sstream << fmt::format("AddBlockEntryOp {} db_name: {} table_name: {} segment_id: {} block_id: {} row_count: {} row_capacity: {} min_row_ts: {} "
                           "max_row_ts: {}",
                           CatalogDeltaOperation::ToString(),
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
    return fmt::format(
        "AddColumnEntryOp {} db_name: {} table_name: {} segment_id: {} block_id: {} column_id: {} next_outline_idx: {}, last_chunk_offset: {}",
        CatalogDeltaOperation::ToString(),
        *db_name_,
        *table_name_,
        segment_id_,
        block_id_,
        column_id_,
        next_outline_idx_,
        last_chunk_offset_);
}

const String AddTableIndexEntryOp::ToString() const {
    bool is_delete = merge_flag_ == MergeFlag::kDelete;
    return fmt::format("AddTableIndexEntryOp {} db_name: {} table_name: {} index_name: {} index_dir: {} index_base: {}",
                       CatalogDeltaOperation::ToString(),
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       !is_delete ? *index_dir_ : "nullptr",
                       !is_delete ? index_base_->ToString() : "nullptr");
}

const String AddSegmentIndexEntryOp::ToString() const {
    return fmt::format("AddSegmentIndexEntryOp {} db_name: {} table_name: {} index_name: {} segment_id: {} min_ts: {} max_ts: {}, next_chunk_id: {}",
                       CatalogDeltaOperation::ToString(),
                       *db_name_,
                       *table_name_,
                       *index_name_,
                       segment_id_,
                       min_ts_,
                       max_ts_,
                       next_chunk_id_);
}

const String AddChunkIndexEntryOp::ToString() const {
    return fmt::format(
        "AddChunkIndexEntryOp db_name: {} table_name: {} index_name: {} segment_id: {} chunk_id: {} base_name: {} base_rowid: {} row_count: {}",
        *db_name_,
        *table_name_,
        *index_name_,
        segment_id_,
        chunk_id_,
        base_name_,
        base_rowid_.ToUint64(),
        row_count_);
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
               next_segment_id_ == rhs_op->next_segment_id_ && column_defs_.size() == rhs_op->column_defs_.size();
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
           column_id_ == rhs_op->column_id_ && next_outline_idx_ == rhs_op->next_outline_idx_ && last_chunk_offset_ == rhs_op->last_chunk_offset_;
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
           min_ts_ == rhs_op->min_ts_ && max_ts_ == rhs_op->max_ts_ && next_chunk_id_ == rhs_op->next_chunk_id_;
}

bool AddChunkIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddChunkIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_) && segment_id_ == rhs_op->segment_id_ &&
           chunk_id_ == rhs_op->chunk_id_ && base_name_ == rhs_op->base_name_ && base_rowid_ == rhs_op->base_rowid_ &&
           row_count_ == rhs_op->row_count_;
}

void AddDBEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_DATABASE_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    MergeFlag flag = this->NextDeleteFlag(other->merge_flag_);
    *this = std::move(*static_cast<AddDBEntryOp *>(other.get()));
    this->merge_flag_ = flag;
}

void AddTableEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_TABLE_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_table_op = static_cast<AddTableEntryOp *>(other.get());
    MergeFlag flag = this->NextDeleteFlag(add_table_op->merge_flag_);
    *this = std::move(*add_table_op);
    this->merge_flag_ = flag;
}

void AddSegmentEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_segment_op = static_cast<AddSegmentEntryOp *>(other.get());
    MergeFlag flag = this->NextDeleteFlag(add_segment_op->merge_flag_);
    auto segment_filter_binary_data = std::move(segment_filter_binary_data_);
    *this = std::move(*add_segment_op);
    this->merge_flag_ = flag;
    if (!segment_filter_binary_data.empty()) {
        if (!segment_filter_binary_data_.empty()) {
            UnrecoverableError("Serialize segment filter binary twice");
        }
        segment_filter_binary_data_ = std::move(segment_filter_binary_data);
    }
}

void AddBlockEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_BLOCK_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    auto *add_block_op = static_cast<AddBlockEntryOp *>(other.get());
    auto block_filter_binary_data = std::move(block_filter_binary_data_);
    *this = std::move(*add_block_op);
    if (!block_filter_binary_data.empty()) {
        if (!block_filter_binary_data_.empty()) {
            UnrecoverableError("Serialize block filter binary twice");
        }
        block_filter_binary_data_ = std::move(block_filter_binary_data);
    }
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
    auto *add_table_index_op = static_cast<AddTableIndexEntryOp *>(other.get());
    MergeFlag flag = this->NextDeleteFlag(add_table_index_op->merge_flag_);
    *this = std::move(*add_table_index_op);
    this->merge_flag_ = flag;
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

void AddBlockEntryOp::FlushDataToDisk(TxnTimeStamp max_commit_ts) {
    LOG_TRACE(fmt::format("BlockEntry {} flush to disk", block_entry_->block_id()));
    block_entry_->Flush(max_commit_ts);
}

void AddSegmentIndexEntryOp::FlushDataToDisk(TxnTimeStamp max_commit_ts) { segment_index_entry_->Flush(max_commit_ts); }

/// class CatalogDeltaEntry
i32 CatalogDeltaEntry::GetSizeInBytes() const {
    i32 size = CatalogDeltaEntryHeader::GetSizeInBytes();

    size += sizeof(max_commit_ts_);
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
    entry->max_commit_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
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

    WriteBufAdv(ptr, max_commit_ts_);
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
void CatalogDeltaEntry::SaveState(TransactionID txn_id, TxnTimeStamp commit_ts, u64 sequence) {
    LOG_TRACE(fmt::format("SaveState txn_id {} commit_ts {}", txn_id, commit_ts));
    if (max_commit_ts_ != UNCOMMIT_TS || !txn_ids_.empty()) {
        UnrecoverableError(fmt::format("CatalogDeltaEntry SaveState failed, max_commit_ts_ {} txn_ids_ size {}", max_commit_ts_, txn_ids_.size()));
    }
    sequence_ = sequence;
    max_commit_ts_ = commit_ts;
    txn_ids_ = {txn_id};
    for (auto &operation : operations_) {
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

void CatalogDeltaEntry::AddOperation(UniquePtr<CatalogDeltaOperation> operation) { operations_.emplace_back(std::move(operation)); }

void GlobalCatalogDeltaEntry::InitFullCheckpointTs(TxnTimeStamp last_full_ckp_ts) { last_full_ckp_ts_ = last_full_ckp_ts; }

void GlobalCatalogDeltaEntry::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size) {
    // {
    //     for (auto &delta_entry : delta_entries) {
    //         LOG_INFO(fmt::format("Add delta entry: {}", delta_entry->ToString()));
    //     }
    // }
    std::lock_guard<std::mutex> lock(catalog_delta_locker_);
    if (delta_entry->sequence() != last_sequence_ + 1) {
        // Discontinuous
        u64 entry_sequence = delta_entry->sequence();
        sequence_heap_.push(entry_sequence);
        delta_entry_map_.emplace(entry_sequence, std::move(delta_entry));
    } else {
        // Continuous
        do {
            if (wal_size_ > wal_size) {
                UnrecoverableError(fmt::format("wal_size_ {} > wal_size {}", wal_size_, wal_size));
            }
            wal_size_ = wal_size;
            this->AddDeltaEntryInner(delta_entry.get());

            ++last_sequence_;

            if (!sequence_heap_.empty() && sequence_heap_.top() == last_sequence_ + 1) {
                auto iter = delta_entry_map_.find(sequence_heap_.top());
                if (iter == delta_entry_map_.end()) {
                    UnrecoverableError(fmt::format("sequence_heap_.top() {} in delta_entry_map_", sequence_heap_.top()));
                }
                delta_entry = std::move(iter->second);
                delta_entry_map_.erase(iter);
                sequence_heap_.pop();
            } else {
                break;
            }
        } while (true);
    }
}

void GlobalCatalogDeltaEntry::ReplayDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry) {
    std::lock_guard<std::mutex> lock(catalog_delta_locker_);
    this->AddDeltaEntryInner(delta_entry.get());
}

// background process Checkpoint call this.
UniquePtr<CatalogDeltaEntry> GlobalCatalogDeltaEntry::PickFlushEntry(TxnTimeStamp max_commit_ts) {
    auto flush_delta_entry = MakeUnique<CatalogDeltaEntry>();

    std::lock_guard<std::mutex> lock(catalog_delta_locker_);
    {
        auto delta_ops = std::exchange(delta_ops_, {});
        auto txn_ids = std::exchange(txn_ids_, {});
        for (auto &[_, delta_op] : delta_ops) {
            if (delta_op->commit_ts_ <= last_full_ckp_ts_) { // skip the delta op that is before full checkpoint
                continue;
            }
            flush_delta_entry->AddOperation(std::move(delta_op));
        }

        flush_delta_entry->set_txn_ids(Vector<TransactionID>(txn_ids.begin(), txn_ids.end()));
    }
    flush_delta_entry->set_commit_ts(max_commit_ts);

    // write delta op from top to bottom.
    auto &operations = flush_delta_entry->operations();
    std::sort(operations.begin(), operations.end(), [](const auto &lhs, const auto &rhs) { return lhs->type_ < rhs->type_; });
    // {
    //     for (const auto &operation : flush_delta_entry->operations()) {
    //         LOG_INFO(fmt::format("Flush delta op: {}", operation->ToString()));
    //     }
    // }
    return flush_delta_entry;
}

SizeT GlobalCatalogDeltaEntry::OpSize() const {
    std::lock_guard<std::mutex> lock(catalog_delta_locker_);
    return delta_ops_.size();
}

// background process AddDeltaOp call this.
void GlobalCatalogDeltaEntry::AddDeltaEntryInner(CatalogDeltaEntry *delta_entry) {
    TxnTimeStamp max_commit_ts = delta_entry->commit_ts();
    if (max_commit_ts == UNCOMMIT_TS) {
        UnrecoverableError("max_commit_ts == UNCOMMIT_TS");
    }
    if (max_commit_ts_ > max_commit_ts) {
        UnrecoverableError(fmt::format("max_commit_ts_ {} > max_commit_ts {}", max_commit_ts_, max_commit_ts));
    }
    max_commit_ts_ = max_commit_ts;

    for (auto &new_op : delta_entry->operations()) {
        if (new_op->type_ == CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
            auto *add_segment_op = static_cast<AddSegmentEntryOp *>(new_op.get());
            if (add_segment_op->status_ == SegmentStatus::kDeprecated) {
                add_segment_op->merge_flag_ = MergeFlag::kDelete;
            }
        }
        String encode = new_op->EncodeIndex();
        auto iter = delta_ops_.find(encode);
        bool found = iter != delta_ops_.end();
        if (found) {
            CatalogDeltaOperation *op = iter->second.get();
            if (op->commit_ts_ <= last_full_ckp_ts_) {
                delta_ops_.erase(iter);
                found = false;
            }
        }
        if (found) {
            CatalogDeltaOperation *op = iter->second.get();
            PruneFlag prune_flag = CatalogDeltaOperation::ToPrune(op->merge_flag_, new_op->merge_flag_);
            if (prune_flag == PruneFlag::kPrune) {
                delta_ops_.erase(iter);
                PruneOpWithSamePrefix(encode);
                continue;
            } else if (prune_flag == PruneFlag::kPruneSub) {
                PruneOpWithSamePrefix(encode);
            }
            op->Merge(std::move(new_op));
        } else {
            PruneFlag prune_flag = CatalogDeltaOperation::ToPrune(None, new_op->merge_flag_);
            delta_ops_[encode] = std::move(new_op);
            if (prune_flag == PruneFlag::kPruneSub) {
                PruneOpWithSamePrefix(encode);
            }
        }
    }
    txn_ids_.insert(delta_entry->txn_ids().begin(), delta_entry->txn_ids().end());
}

void GlobalCatalogDeltaEntry::PruneOpWithSamePrefix(const String &encode1) {
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
