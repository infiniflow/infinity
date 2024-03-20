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
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto db_entry_dir = MakeShared<String>(ReadBufAdv<String>(ptr));
            operation = MakeUnique<AddDBEntryOp>(begin_ts, is_delete, txn_id, commit_ts, std::move(db_name), std::move(db_entry_dir));
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_entry_dir = MakeShared<String>(ReadBufAdv<String>(ptr));

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
            SegmentID unsealed_id = ReadBufAdv<SegmentID>(ptr);
            operation = MakeUnique<AddTableEntryOp>(begin_ts,
                                                    is_delete,
                                                    txn_id,
                                                    commit_ts,
                                                    std::move(db_name),
                                                    std::move(table_name),
                                                    std::move(table_entry_dir),
                                                    columns,
                                                    row_count,
                                                    unsealed_id);
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            SegmentStatus segment_status = ReadBufAdv<SegmentStatus>(ptr);
            u64 column_count = ReadBufAdv<u64>(ptr);
            SizeT row_count = ReadBufAdv<SizeT>(ptr);
            SizeT actual_row_count = ReadBufAdv<SizeT>(ptr);
            SizeT row_capacity = ReadBufAdv<SizeT>(ptr);
            TxnTimeStamp min_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp deprecate_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation = MakeUnique<AddSegmentEntryOp>(begin_ts,
                                                      is_delete,
                                                      txn_id,
                                                      commit_ts,
                                                      std::move(db_name),
                                                      std::move(table_name),
                                                      segment_id,
                                                      segment_status,
                                                      column_count,
                                                      row_count,
                                                      row_capacity,
                                                      actual_row_count,
                                                      min_row_ts,
                                                      max_row_ts,
                                                      deprecate_ts);
            break;
        }
        case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
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
                                                    row_count,
                                                    row_capacity,
                                                    min_row_ts,
                                                    max_row_ts,
                                                    checkpoint_ts,
                                                    checkpoint_row_count);
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
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
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            SharedPtr<String> index_dir = is_delete ? nullptr : MakeShared<String>(ReadBufAdv<String>(ptr));
            SharedPtr<IndexBase> index_base = is_delete ? nullptr : IndexBase::ReadAdv(ptr, ptr_end - ptr);
            operation = MakeUnique<AddTableIndexEntryOp>(begin_ts,
                                                         is_delete,
                                                         txn_id,
                                                         commit_ts,
                                                         std::move(db_name),
                                                         std::move(table_name),
                                                         std::move(index_name),
                                                         std::move(index_dir),
                                                         index_base);
            break;
        }
        case CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            operation = MakeUnique<AddFulltextIndexEntryOp>(begin_ts,
                                                            is_delete,
                                                            txn_id,
                                                            commit_ts,
                                                            std::move(db_name),
                                                            std::move(table_name),
                                                            std::move(index_name));
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            TxnTimeStamp min_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation = MakeUnique<AddSegmentIndexEntryOp>(begin_ts,
                                                           is_delete,
                                                           txn_id,
                                                           commit_ts,
                                                           std::move(db_name),
                                                           std::move(table_name),
                                                           std::move(index_name),
                                                           segment_id,
                                                           min_ts,
                                                           max_ts);
            break;
        }
        case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
            auto db_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            auto table_name = MakeShared<String>(ReadBufAdv<String>(ptr));
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_filter_binary_data = ReadBufAdv<String>(ptr);
            Vector<Pair<BlockID, String>> block_filter_binary_data;
            i32 block_filter_binary_vector_size = ReadBufAdv<i32>(ptr);
            for (i32 i = 0; i < block_filter_binary_vector_size; i++) {
                BlockID block_id = ReadBufAdv<BlockID>(ptr);
                String block_filter_binary = ReadBufAdv<String>(ptr);
                block_filter_binary_data.emplace_back(block_id, std::move(block_filter_binary));
            }
            operation = MakeUnique<SetSegmentStatusSealedOp>(begin_ts,
                                                             is_delete,
                                                             txn_id,
                                                             commit_ts,
                                                             db_name,
                                                             table_name,
                                                             segment_id,
                                                             std::move(segment_filter_binary_data),
                                                             std::move(block_filter_binary_data));
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
    if (!is_delete()) {
        WriteBufAdv(buf, *this->index_dir_);
        index_base_->WriteAdv(buf);
    }
}

void AddFulltextIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, *this->index_name_);
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

void SetSegmentStatusSealedOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_name_);
    WriteBufAdv(buf, *this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
    i32 block_filter_binary_vector_size = block_filter_binary_data_.size();
    WriteBufAdv(buf, block_filter_binary_vector_size);
    for (const auto &block_filter : block_filter_binary_data_) {
        WriteBufAdv(buf, block_filter.first);
        WriteBufAdv(buf, block_filter.second);
    }
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

const String AddFulltextIndexEntryOp::ToString() const {
    return fmt::format("AddFulltextIndexEntryOp db_name: {} table_name: {} index_name: {}", *db_name_, *table_name_, *index_name_);
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

const String SetSegmentStatusSealedOp::ToString() const {
    return fmt::format("SetSegmentStatusSealedOp db_name: {} table_name: {} segment_id: {}", *db_name_, *table_name_, segment_id_);
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

bool AddFulltextIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddFulltextIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_);
}

bool AddSegmentIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddSegmentIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
           IsEqual(*table_name_, *rhs_op->table_name_) && IsEqual(*index_name_, *rhs_op->index_name_) && segment_id_ == rhs_op->segment_id_ &&
           min_ts_ == rhs_op->min_ts_ && max_ts_ == rhs_op->max_ts_;
}

bool SetSegmentStatusSealedOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const SetSegmentStatusSealedOp *>(&rhs);
    bool res = rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_name_, *rhs_op->db_name_) &&
               IsEqual(*table_name_, *rhs_op->table_name_) && segment_id_ == rhs_op->segment_id_ &&
               segment_filter_binary_data_ == rhs_op->segment_filter_binary_data_ &&
               block_filter_binary_data_.size() == rhs_op->block_filter_binary_data_.size();
    if (!res) {
        return false;
    }
    for (size_t i = 0; i < block_filter_binary_data_.size(); ++i) {
        if (block_filter_binary_data_[i].first != rhs_op->block_filter_binary_data_[i].first ||
            block_filter_binary_data_[i].second != rhs_op->block_filter_binary_data_[i].second) {
            return false;
        }
    }
    return true;
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
    *this = std::move(*static_cast<AddTableEntryOp *>(other.get()));
}

void AddSegmentEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddSegmentEntryOp *>(other.get()));
}

void AddBlockEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_BLOCK_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddBlockEntryOp *>(other.get()));
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

void AddFulltextIndexEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddFulltextIndexEntryOp *>(other.get()));
}

void AddSegmentIndexEntryOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<AddSegmentIndexEntryOp *>(other.get()));
}

void SetSegmentStatusSealedOp::Merge(UniquePtr<CatalogDeltaOperation> other) {
    if (other->type_ != CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED) {
        UnrecoverableError(fmt::format("Merge failed, other type: {}", other->GetTypeStr()));
    }
    *this = std::move(*static_cast<SetSegmentStatusSealedOp *>(other.get()));
}

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

SharedPtr<CatalogDeltaEntry> CatalogDeltaEntry::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_start = ptr;
    char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        UnrecoverableError("ptr goes out of range when reading WalEntry");
    }
    CatalogDeltaEntryHeader header;
    header.size_ = ReadBufAdv<i32>(ptr);
    header.checksum_ = ReadBufAdv<u32>(ptr);
    auto entry = MakeShared<CatalogDeltaEntry>();
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
}

String CatalogDeltaEntry::ToString() const {
    std::stringstream sstream;
    for (const auto &operation : operations_) {
        sstream << operation->ToString() << '\n';
    }
    return sstream.str();
}

void GlobalCatalogDeltaEntry::AddDeltaEntries(Vector<UniquePtr<CatalogDeltaEntry>> &&delta_entries) {
    std::unique_lock w_lock(mtx_);
    for (auto &delta_entry : delta_entries) {
        SizeT idx = delta_entry_infos_.size();
        for (auto &op : delta_entry->operations()) {
            String encode = op->EncodeIndex();
            auto iter = delta_ops_.find(encode);
            if (iter != delta_ops_.end()) {
                auto &[delta_op, info_idxs] = iter->second;
                delta_op->Merge(std::move(op));
                info_idxs.push_back(idx);
            } else {
                delta_ops_[encode] = {std::move(op), Vector<SizeT>{idx}};
            }
        }
        delta_entry_infos_.emplace_back(delta_entry->txn_ids()[0], delta_entry->commit_ts());
    }
}

UniquePtr<CatalogDeltaEntry> GlobalCatalogDeltaEntry::PickFlushEntry(TxnTimeStamp max_commit_ts) {
    auto flush_delta_entry = MakeUnique<CatalogDeltaEntry>();

    HashSet<TransactionID> txn_ids;
    TxnTimeStamp flush_max_ts = 0;
    {
        std::unique_lock w_lock(mtx_);

        for (auto &[_, op_pair] : delta_ops_) {
            auto &[delta_op, info_idxs] = op_pair;
            for (SizeT idx : info_idxs) {
                const auto &[txn_id, commit_ts] = delta_entry_infos_[idx];
                flush_max_ts = std::max(flush_max_ts, delta_op->commit_ts());
                txn_ids.insert(txn_id);
            }
            flush_delta_entry->AddOperation(std::move(delta_op));
        }
        delta_ops_.clear();
        delta_entry_infos_.clear();
    }
    if (flush_max_ts > max_commit_ts) {
        // all delta op in global delta entry should have been flushed in wal
        UnrecoverableError(fmt::format("flush_max_ts {} > max_commit_ts {}", flush_max_ts, max_commit_ts));
    }
    flush_delta_entry->set_commit_ts(flush_max_ts);
    flush_delta_entry->set_txn_ids(Vector<TransactionID>(txn_ids.begin(), txn_ids.end()));

    // write delta op from top to bottom.
    std::sort(flush_delta_entry->operations().begin(), flush_delta_entry->operations().end(), [](const auto &lhs, const auto &rhs) {
        return lhs->type_ < rhs->type_;
    });
    return flush_delta_entry;
}

} // namespace infinity
