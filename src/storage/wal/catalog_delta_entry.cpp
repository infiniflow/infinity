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
    UniquePtr<CatalogDeltaOperation> operation = nullptr;
    auto operation_type = ReadBufAdv<CatalogDeltaOperationType>(ptr);
    switch (operation_type) {
        case CatalogDeltaOperationType::ADD_DATABASE_META: {
            String db_name = ReadBufAdv<String>(ptr);
            String data_dir = ReadBufAdv<String>(ptr);
            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);
            operation = MakeUnique<AddDBMetaOperation>(begin_ts, is_delete, db_name, data_dir);
            break;
        }
        case CatalogDeltaOperationType::ADD_DATABASE_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_entry_dir = ReadBufAdv<String>(ptr);
            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);
            operation = MakeUnique<AddDBEntryOperation>(begin_ts, is_delete, db_name, db_entry_dir);
            operation->begin_ts_ = begin_ts;
            operation->is_delete_ = is_delete;
            break;
        }
        case CatalogDeltaOperationType::ADD_TABLE_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_entry_dir = ReadBufAdv<String>(ptr);
            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);
            operation = MakeUnique<AddTableEntryOperation>(begin_ts, is_delete, db_name, table_name, table_entry_dir);
            operation->begin_ts_ = begin_ts;
            operation->is_delete_ = is_delete;
            break;
        }
        case CatalogDeltaOperationType::ADD_SEGMENT_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_dir = ReadBufAdv<String>(ptr);

            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);
            operation = MakeUnique<AddSegmentEntryOperation>(begin_ts, is_delete, db_name, table_name, segment_id, segment_dir);
            operation->begin_ts_ = begin_ts;
            operation->is_delete_ = is_delete;
            break;
        }
        case CatalogDeltaOperationType::ADD_BLOCK_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            String block_dir = ReadBufAdv<String>(ptr);
            u16 row_count = ReadBufAdv<u16>(ptr);
            u16 row_capacity = ReadBufAdv<u16>(ptr);

            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);
            operation = MakeUnique<AddBlockEntryOperation>(begin_ts,
                                                           is_delete,
                                                           db_name,
                                                           table_name,
                                                           segment_id,
                                                           block_id,
                                                           block_dir,
                                                           row_count,
                                                           row_capacity);
            break;
        }
        case CatalogDeltaOperationType::ADD_COLUMN_ENTRY: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            BlockID block_id = ReadBufAdv<BlockID>(ptr);
            ColumnID column_id = ReadBufAdv<ColumnID>(ptr);
            i32 next_outline_idx = ReadBufAdv<i32>(ptr);

            TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            bool is_delete = ReadBufAdv<bool>(ptr);

            operation =
                MakeUnique<AddColumnEntryOperation>(begin_ts, is_delete, db_name, table_name, segment_id, block_id, column_id, next_outline_idx);
            break;
        }
        default:
            UnrecoverableException(fmt::format("UNIMPLEMENTED ReadAdv for CatalogDeltaOperation type {}", int(operation_type)));
    }

    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableException("ptr goes out of range when reading CatalogDeltaOperation");
    }
    return operation;
}

void AddDBMetaOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_DATABASE_META);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->data_dir_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddTableMetaOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_DATABASE_META);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->db_entry_dir_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddDBEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_DATABASE_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_entry_dir_);

    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddTableEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_TABLE_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_entry_dir_);

    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddSegmentEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_SEGMENT_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_dir_);

    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddBlockEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_BLOCK_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->block_dir_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->row_capacity_);

    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddColumnEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_COLUMN_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->block_id_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->next_outline_idx_);

    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddIndexMetaOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_INDEX_META);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddTableIndexEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_TABLE_INDEX_ENTRY);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddIrsIndexEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_IRS_INDEX_ENTRY);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_dir_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddColumnIndexEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_COLUMN_INDEX_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddSegmentColumnIndexEntryOperation::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, CatalogDeltaOperationType::ADD_SEGMENT_COLUMN_INDEX_ENTRY);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->column_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->min_ts_);
    WriteBufAdv(buf, this->max_ts_);
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->is_delete_);
}

void AddDBMetaOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->db_meta_->db_name();
    this->data_dir_ = *this->db_meta_->data_dir();
    is_snapshotted_ = true;
}

void AddTableMetaOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = this->table_meta_->db_name();
    this->table_name_ = this->table_meta_->table_name();
    this->db_entry_dir_ = this->table_meta_->db_entry_dir();
    is_snapshotted_ = true;
}

void AddDBEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->is_delete_ = db_entry_->deleted_;
    String db_name = db_entry_->db_name();
    String db_entry_dir_ = db_entry_->db_entry_dir();
    this->db_name_ = db_name;
    this->db_entry_dir_ = db_entry_dir_;
    is_snapshotted_ = true;
}

void AddTableEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->is_delete_ = this->table_entry_->deleted_;
    this->db_name_ = *this->table_entry_->GetDBName();
    this->table_name_ = *this->table_entry_->GetTableName();
    this->table_entry_dir_ = *this->table_entry_->TableEntryDir();
    is_snapshotted_ = true;
}

void AddSegmentEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->segment_entry_->GetTableEntry()->GetDBName();
    this->table_name_ = *this->segment_entry_->GetTableEntry()->GetTableName();
    this->segment_id_ = this->segment_entry_->segment_id();
    this->segment_dir_ = this->segment_entry_->DirPath();
    is_snapshotted_ = true;
}

void AddBlockEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->block_entry_->GetSegmentEntry()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->block_entry_->GetSegmentEntry()->GetTableEntry()->GetTableName();
    this->segment_id_ = this->block_entry_->GetSegmentEntry()->segment_id();
    this->block_id_ = this->block_entry_->block_id();
    this->block_dir_ = this->block_entry_->DirPath();
    this->row_count_ = this->block_entry_->row_count();
    this->row_capacity_ = this->block_entry_->row_capacity();
    is_snapshotted_ = true;
}

void AddColumnEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->column_entry_->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->column_entry_->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetTableName();
    this->segment_id_ = this->column_entry_->GetBlockEntry()->GetSegmentEntry()->segment_id();
    this->block_id_ = this->column_entry_->GetBlockEntry()->block_id();
    this->column_id_ = this->column_entry_->column_id();
    if (this->column_entry_->outline_info_ptr() != nullptr) {
        this->next_outline_idx_ = this->column_entry_->outline_info_ptr()->next_file_idx;
    }
    is_snapshotted_ = true;
}

/// Related to index
void AddIndexMetaOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->index_meta_->GetTableEntry()->GetDBName();
    this->table_name_ = *this->index_meta_->GetTableEntry()->GetTableName();
    this->index_name_ = this->index_meta_->index_name();
    is_snapshotted_ = true;
}

void AddTableIndexEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->is_delete_ = this->table_index_entry_->deleted_;
    this->db_name_ = *this->table_index_entry_->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->table_index_entry_->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->table_index_entry_->table_index_meta()->index_name();
    this->index_dir_ = *this->table_index_entry_->index_dir();
    this->index_def_ = this->table_index_entry_->table_index_def();
    is_snapshotted_ = true;
}

void AddIrsIndexEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->irs_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->irs_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->irs_index_entry_->table_index_entry()->table_index_meta()->index_name();
    this->index_dir_ = this->irs_index_entry_->index_dir();
    is_snapshotted_ = true;
}

void AddColumnIndexEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
    this->db_name_ = *this->column_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName();
    this->table_name_ = *this->column_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName();
    this->index_name_ = this->column_index_entry_->table_index_entry()->table_index_meta()->index_name();
    this->col_index_dir_ = *this->column_index_entry_->col_index_dir();
    this->column_id_ = this->column_index_entry_->column_id();
    is_snapshotted_ = true;
}

void AddSegmentColumnIndexEntryOperation::SaveSate() {
    if (is_snapshotted_) {
        return;
    }
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
        UnrecoverableException("ptr goes out of range when reading WalEntry");
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
            UnrecoverableException("ptr goes out of range when reading WalEntry");
        }
        UniquePtr<CatalogDeltaOperation> operation = CatalogDeltaOperation::ReadAdv(ptr, max_bytes);
        entry->operations_.emplace_back(std::move(operation));
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableException("ptr goes out of range when reading WalEntry");
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
        operation->WriteAdv(ptr);
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

    LOG_INFO(fmt::format("Snapshot txn_id {} commit_ts {}", txn_id, commit_ts));
    this->commit_ts_ = commit_ts;
    this->txn_id_ = txn_id;
    for (auto &operation : operations_) {
        LOG_TRACE(fmt::format("Snapshot operation {}", operation->ToString()));
        operation->SaveSate();
    }
}

String CatalogDeltaEntry::ToString() const {
    String str;
    for (const auto &operation : operations_) {
        str += operation->ToString() + "\n";
    }
    return str;
}

void GlobalCatalogDeltaEntry::Merge(UniquePtr<CatalogDeltaEntry> other) {

    auto &global_map = this->encode_op_to_id_map_;
    auto &global_operations = this->global_operations_;
    auto &local_operations = other->operations();

    for (auto &local_operation : local_operations) {
        auto it = global_map.find(local_operation->EncodeIndex());
        if (it == global_map.end()) {
            global_map[local_operation->EncodeIndex()] = global_operations.size();
            global_operations.push_back(std::move(local_operation));
        } else {
            global_operations[it->second] = std::move(local_operation);
        }
    }

    local_operations.clear();
    this->set_txn_id(other->txn_id());
    this->set_commit_ts(other->commit_ts());
}

} // namespace infinity
