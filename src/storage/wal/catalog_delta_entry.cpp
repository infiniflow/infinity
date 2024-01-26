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
import index_def;
import infinity_exception;

import stl;
import parser;
import third_party;
import logger;

namespace infinity {

UniquePtr<CatalogDeltaOperation> CatalogDeltaOperation::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    UniquePtr<CatalogDeltaOperation> operation{nullptr};
    auto operation_type = ReadBufAdv<CatalogDeltaOpType>(ptr);
    switch (operation_type) {
        case CatalogDeltaOpType::ADD_DATABASE_META: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String data_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddDBMetaOp>(begin_ts, is_delete, db_name, data_dir);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_META: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String db_entry_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddTableMetaOp>(begin_ts, is_delete, db_name, table_name, db_entry_dir);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String db_entry_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddDBEntryOp>(begin_ts, is_delete, db_name, db_entry_dir);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
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

            operation = MakeUnique<AddTableEntryOp>(begin_ts, is_delete, db_name, table_name, table_entry_dir, columns);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddSegmentEntryOp>(begin_ts, is_delete, db_name, table_name, segment_id, segment_dir);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            String block_dir = ReadBufAdv<String>(ptr);
            u16 row_count = ReadBufAdv<u16>(ptr);
            u16 row_capacity = ReadBufAdv<u16>(ptr);
            TxnTimeStamp min_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_row_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation = MakeUnique<AddBlockEntryOp>(begin_ts,
                                                    is_delete,
                                                    db_name,
                                                    table_name,
                                                    segment_id,
                                                    block_id,
                                                    block_dir,
                                                    row_count,
                                                    row_capacity,
                                                    min_row_ts,
                                                    max_row_ts);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);
            i32 next_outline_idx = ReadBufAdv<i32>(ptr);
            operation = MakeUnique<AddColumnEntryOp>(begin_ts, is_delete, db_name, table_name, segment_id, block_id, column_id, next_outline_idx);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_INDEX_META: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddIndexMetaOp>(begin_ts, is_delete, db_name, table_name, index_name);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_dir = ReadBufAdv<String>(ptr);
            SharedPtr<IndexDef> index_def = IndexDef::ReadAdv(ptr, ptr_end - ptr);
            //  TODO: index_def
            operation = MakeUnique<AddTableIndexEntryOp>(begin_ts, is_delete, db_name, table_name, index_name, index_dir, index_def);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_IRS_INDEX_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_dir = ReadBufAdv<String>(ptr);
            operation = MakeUnique<AddIrsIndexEntryOp>(begin_ts, is_delete, db_name, table_name, index_name, index_dir);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String col_index_dir = ReadBufAdv<String>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);

            SharedPtr<IndexBase> index_base = IndexBase::ReadAdv(ptr, ptr_end - ptr);
            operation = MakeUnique<AddColumnIndexEntryOp>(begin_ts, is_delete, db_name, table_name, index_name, col_index_dir, column_id, index_base);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        case CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY: {
            auto [begin_ts, is_delete, txn_id, commit_ts] = ReadAdvBase(ptr);
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            TxnTimeStamp min_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            TxnTimeStamp max_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            operation =
                MakeUnique<AddSegmentColumnIndexEntryOp>(begin_ts, is_delete, db_name, table_name, index_name, column_id, segment_id, min_ts, max_ts);
            operation->commit_ts_ = commit_ts;
            operation->txn_id_ = txn_id;
            break;
        }
        default:
            UnrecoverableError(fmt::format("UNIMPLEMENTED ReadAdv for CatalogDeltaOperation type {}", int(operation_type)));
    }

    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableError("ptr goes out of range when reading CatalogDeltaOperation");
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
    WriteBufAdv(buf, this->db_name_);
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
    WriteBufAdv(buf, this->db_name_);
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
}

void AddSegmentEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_dir_);
}

void AddBlockEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->block_dir_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
}

void AddColumnEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->next_outline_idx_);
}

void AddIndexMetaOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
}

void AddTableIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
    index_def_->WriteAdv(buf);
}

void AddIrsIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
}

void AddColumnIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->col_index_dir_);
    WriteBufAdv(buf, this->column_id_);
    index_base_->WriteAdv(buf);
}

void AddSegmentColumnIndexEntryOp::WriteAdv(char *&buf) const {
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->min_ts_);
    WriteBufAdv(buf, this->max_ts_);
}

void AddDBMetaOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->db_name_ = *this->db_meta_->db_name();
    this->data_dir_ = *this->db_meta_->data_dir();
    is_snapshotted_ = true;
}

void AddTableMetaOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //
    this->db_name_ = this->table_meta_->db_name();
    this->table_name_ = this->table_meta_->table_name();
    this->db_entry_dir_ = this->table_meta_->db_entry_dir();
    is_snapshotted_ = true;
}

void AddDBEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = db_entry_->deleted_;
    this->begin_ts_ = db_entry_->begin_ts_;
    String db_name = db_entry_->db_name();
    String db_entry_dir_ = db_entry_->db_entry_dir();
    this->db_name_ = db_name;
    this->db_entry_dir_ = db_entry_dir_;
    is_snapshotted_ = true;
}

void AddTableEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = this->table_entry_->deleted_;
    this->begin_ts_ = this->table_entry_->begin_ts_;
    this->db_name_ = *this->table_entry_->GetDBName();
    this->table_name_ = *this->table_entry_->GetTableName();
    this->table_entry_dir_ = *this->table_entry_->TableEntryDir();
    this->column_defs_ = this->table_entry_->column_defs();
    is_snapshotted_ = true;
}

void AddSegmentEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = segment_entry_->deleted_;
    this->begin_ts_ = segment_entry_->begin_ts_;
    this->db_name_ = *this->segment_entry_->GetTableEntry()->GetDBName();
    this->table_name_ = *this->segment_entry_->GetTableEntry()->GetTableName();
    this->segment_id_ = this->segment_entry_->segment_id();
    this->segment_dir_ = this->segment_entry_->DirPath();
    is_snapshotted_ = true;
}

void AddBlockEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = block_entry_->deleted_;
    this->begin_ts_ = block_entry_->begin_ts_;
    this->db_name_ = *this->block_entry_->GetSegmentEntry()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->block_entry_->GetSegmentEntry()->GetTableEntry()->GetTableName();
    this->segment_id_ = this->block_entry_->GetSegmentEntry()->segment_id();
    this->block_id_ = this->block_entry_->block_id();
    this->block_dir_ = this->block_entry_->DirPath();
    this->row_count_ = this->block_entry_->row_count();
    this->row_capacity_ = this->block_entry_->row_capacity();
    is_snapshotted_ = true;
}

void AddColumnEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = column_entry_->deleted_;
    this->begin_ts_ = column_entry_->begin_ts_;
    this->db_name_ = *this->column_entry_->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->column_entry_->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetTableName();
    this->segment_id_ = this->column_entry_->GetBlockEntry()->GetSegmentEntry()->segment_id();
    this->block_id_ = this->column_entry_->GetBlockEntry()->block_id();
    this->column_id_ = this->column_entry_->column_id();
    this->next_outline_idx_ = this->column_entry_->outline_buffers_.size();
    is_snapshotted_ = true;
}

/// Related to index
void AddIndexMetaOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->db_name_ = *this->index_meta_->GetTableEntry()->GetDBName();
    this->table_name_ = *this->index_meta_->GetTableEntry()->GetTableName();
    this->index_name_ = this->index_meta_->index_name();
    is_snapshotted_ = true;
}

void AddTableIndexEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = table_index_entry_->deleted_;
    this->begin_ts_ = table_index_entry_->begin_ts_;
    this->db_name_ = *this->table_index_entry_->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->table_index_entry_->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->table_index_entry_->table_index_meta()->index_name();
    this->index_dir_ = *this->table_index_entry_->index_dir();
    this->index_def_ = this->table_index_entry_->table_index_def();
    is_snapshotted_ = true;
}

void AddIrsIndexEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = irs_index_entry_->deleted_;
    this->begin_ts_ = irs_index_entry_->begin_ts_;
    this->db_name_ = *this->irs_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->irs_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->irs_index_entry_->table_index_entry()->table_index_meta()->index_name();
    this->index_dir_ = this->irs_index_entry_->index_dir();
    is_snapshotted_ = true;
}

void AddColumnIndexEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = column_index_entry_->deleted_;
    this->begin_ts_ = column_index_entry_->begin_ts_;
    this->db_name_ = *this->column_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->column_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->column_index_entry_->table_index_entry()->table_index_meta()->index_name();
    this->col_index_dir_ = *this->column_index_entry_->col_index_dir();
    this->column_id_ = this->column_index_entry_->column_id();
    this->index_base_ = this->column_index_entry_->index_base();
    is_snapshotted_ = true;
}

void AddSegmentColumnIndexEntryOp::SaveSate() {
    //    if (is_snapshotted_) {
    //        return;
    //    }
    this->is_delete_ = segment_column_index_entry_->deleted_;
    this->begin_ts_ = segment_column_index_entry_->begin_ts_;
    this->db_name_ = *this->segment_column_index_entry_->column_index_entry()->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ =
        *this->segment_column_index_entry_->column_index_entry()->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->segment_column_index_entry_->column_index_entry()->table_index_entry()->table_index_meta()->index_name();
    this->column_id_ = this->segment_column_index_entry_->column_index_entry()->column_id();
    this->segment_id_ = this->segment_column_index_entry_->segment_id();
    this->min_ts_ = this->segment_column_index_entry_->min_ts();
    this->max_ts_ = this->segment_column_index_entry_->max_ts();
    is_snapshotted_ = true;
}

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
void CatalogDeltaEntry::WriteAdv(char *&ptr) const {
    char *const saved_ptr = ptr;
    std::memcpy(ptr, this, sizeof(CatalogDeltaEntryHeader));
    ptr += sizeof(CatalogDeltaEntryHeader);

    WriteBufAdv(ptr, static_cast<i32>(operations_.size()));
    SizeT operation_count = operations_.size();
    for (SizeT idx = 0; idx < operation_count; ++idx) {
        const auto &operation = operations_[idx];
        LOG_TRACE(fmt::format("!!!Write Adv {}", operation->ToString()));
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

void CatalogDeltaEntry::SaveState(TransactionID txn_id, TxnTimeStamp commit_ts) {
    LOG_INFO(fmt::format("SaveState txn_id {} commit_ts {}", txn_id, commit_ts));
    this->commit_ts_ = commit_ts;
    this->txn_id_ = txn_id;
    for (auto &operation : operations_) {
        LOG_TRACE(fmt::format("SaveState operation {}", operation->ToString()));
        operation->SaveSate();
        operation->txn_id_ = txn_id;
        operation->commit_ts_ = commit_ts;
    }
}

std::string CatalogDeltaEntry::ToString() const {
    std::stringstream sstream;
    for (const auto &operation : operations_) {
        sstream << operation->ToString() << '\n';
    }
    return sstream.str();
}

void GlobalCatalogDeltaEntry::Merge(UniquePtr<CatalogDeltaEntry> other) {
    auto &global_map = this->encode_to_id_map();
    auto &global_operations = this->operations();
    auto &local_operations = other->operations();

    for (auto &local_operation : local_operations) {
        //        auto it = global_map.find(local_operation->EncodeIndex());
        //        if (it == global_map.end()) {
        //            global_map[local_operation->EncodeIndex()] = global_operations.size();
        global_operations.push_back(std::move(local_operation));
        //        } else {
        //            global_operations[it->second] = std::move(local_operation);
        //        }
    }

    local_operations.clear();
    this->set_txn_id(other->txn_id());
    this->set_commit_ts(other->commit_ts());
}
} // namespace infinity
