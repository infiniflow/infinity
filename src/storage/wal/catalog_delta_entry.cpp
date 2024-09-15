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
#include <sstream>
#include <vector>
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
import block_version;
import create_index_info;
import infinity_context;
import index_defines;
import persistence_manager;
import defer_op;
import third_party;
import logger;

namespace infinity {

String ToString(CatalogDeltaOpType op_type) {
    switch (op_type) {
        case CatalogDeltaOpType::ADD_DATABASE_ENTRY: {
            return "AddDatabase";
        }
        case CatalogDeltaOpType::ADD_TABLE_ENTRY: {
            return "AddTable";
        }
        case CatalogDeltaOpType::ADD_SEGMENT_ENTRY: {
            return "AddSegment";
        }
        case CatalogDeltaOpType::ADD_BLOCK_ENTRY: {
            return "AddBlock";
        }
        case CatalogDeltaOpType::ADD_COLUMN_ENTRY: {
            return "AddColumn";
        }
        case CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY: {
            return "AddTableIndex";
        }
        case CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY: {
            return "AddSegmentIndex";
        }
        case CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY: {
            return "AddChunkIndex";
        }
        case CatalogDeltaOpType::SET_SEGMENT_STATUS_SEALED: {
            return "SealSegment";
        }
        case CatalogDeltaOpType::SET_BLOCK_STATUS_SEALED: {
            return "SealBlock";
        }
        case CatalogDeltaOpType::INVALID: {
            String error_message = "Invalid catalog delta operation type.";
            UnrecoverableError(error_message);
        }
    }
    return String();
}

SizeT CatalogDeltaOperation::GetBaseSizeInBytes() const {
    SizeT size = sizeof(TxnTimeStamp) + sizeof(merge_flag_) + sizeof(TransactionID) + sizeof(TxnTimeStamp);
    size += sizeof(i32) + encode_->size();

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        pm_size_ = addr_serializer_.GetSizeInBytes();
        size += pm_size_;
    }
    return size;
}

void CatalogDeltaOperation::InitializeAddrSerializer() {
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        addr_serializer_.Initialize(pm, GetFilePaths());
    }
}

void CatalogDeltaOperation::WriteAdvBase(char *&buf) const {
    WriteBufAdv(buf, this->begin_ts_);
    WriteBufAdv(buf, this->merge_flag_);
    WriteBufAdv(buf, this->txn_id_);
    WriteBufAdv(buf, this->commit_ts_);
    WriteBufAdv(buf, *(this->encode_));

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        char *start = buf;
        addr_serializer_.WriteBufAdv(buf);
        SizeT pm_size = buf - start;
        if (pm_size != pm_size_) {
            String error_message = fmt::format("Mismatched pm_size: {} != {}", pm_size, pm_size_);
            UnrecoverableError(error_message);
        }
    }
}

void CatalogDeltaOperation::ReadAdvBase(const char *&ptr) {
    begin_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    merge_flag_ = ReadBufAdv<MergeFlag>(ptr);
    txn_id_ = ReadBufAdv<TransactionID>(ptr);
    commit_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    encode_ = MakeUnique<String>(ReadBufAdv<String>(ptr));

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        addr_serializer_.ReadBufAdv(ptr); // discard return value
    }
}

const String CatalogDeltaOperation::ToString() const {
    return fmt::format("begin_ts: {}, txn_id: {}, commit_ts: {}, merge_flag: {}, encode: {}",
                       begin_ts_,
                       txn_id_,
                       commit_ts_,
                       (u8)merge_flag_,
                       *encode_);
}

CatalogDeltaOperation::CatalogDeltaOperation(CatalogDeltaOpType type, BaseEntry *base_entry, TxnTimeStamp commit_ts)
    : begin_ts_(base_entry->begin_ts_), txn_id_(base_entry->txn_id_), commit_ts_(base_entry->commit_ts_), encode_(base_entry->encode_ptr()),
      type_(type) {
    if (base_entry->deleted_) {
        merge_flag_ = MergeFlag::kDelete;
    } else if (commit_ts == base_entry->commit_ts_) {
        merge_flag_ = MergeFlag::kNew;
    } else if (!base_entry->Committed()) {
        String error_message = "Entry not committed.";
        UnrecoverableError(error_message);
    } else {
        if (commit_ts < base_entry->commit_ts_) {
            String error_message = fmt::format("Invalid commit_ts: {} < {}", commit_ts, base_entry->commit_ts_);
            UnrecoverableError(error_message);
        }
        merge_flag_ = MergeFlag::kUpdate;
    }
    // LOG_TRACE(fmt::format("Create delta op: {} ", this->ToString()));
}

UniquePtr<CatalogDeltaOperation> CatalogDeltaOperation::ReadAdv(const char *&ptr, i32 max_bytes) {
    const char *const ptr_end = ptr + max_bytes;
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
        default: {
            String error_message = fmt::format("UNIMPLEMENTED ReadAdv for CatalogDeltaOperation type {}", int(operation_type));
            UnrecoverableError(error_message);
        }
    }

    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        String error_message = "ptr goes out of range when reading CatalogDeltaOperation";
        UnrecoverableError(error_message);
    }
    if (operation.get() == nullptr) {
        String error_message = "operation is nullptr";
        UnrecoverableError(error_message);
    }
    return operation;
}

bool CatalogDeltaOperation::operator==(const CatalogDeltaOperation &rhs) const {
    return this->begin_ts_ == rhs.begin_ts_ && this->txn_id_ == rhs.txn_id_ && this->commit_ts_ == rhs.commit_ts_ &&
           this->merge_flag_ == rhs.merge_flag_ && this->type_ == rhs.type_ && *this->encode_ == *rhs.encode_;
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
                String error_message = fmt::format("Invalid MergeFlag: {}", u8(new_merge_flag));
                UnrecoverableError(error_message);
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
                    String error_message = fmt::format("Invalid MergeFlag: {}", u8(old_merge_flag));
                    UnrecoverableError(error_message);
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
                    String error_message = fmt::format("Invalid MergeFlag: {}", u8(old_merge_flag));
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        default: {
            String error_message = fmt::format("Invalid MergeFlag: {}", u8(new_merge_flag));
            UnrecoverableError(error_message);
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
                    String error_message = fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag));
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case MergeFlag::kNew: {
            switch (new_merge_flag) {
                case MergeFlag::kDelete: {
                    String error_message = "Should prune before reach this.";
                    UnrecoverableError(error_message);
                    break;
                }
                case MergeFlag::kUpdate:
                case MergeFlag::kDeleteAndNew: {
                    return MergeFlag::kNew;
                }
                default: {
                    String error_message = fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag));
                    UnrecoverableError(error_message);
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
                    String error_message = fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag));
                    UnrecoverableError(error_message);
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
                    String error_message = fmt::format("Invalid MergeFlag from {} to {}", u8(this->merge_flag_), u8(new_merge_flag));
                    UnrecoverableError(error_message);
                }
            }
        }
        default: {
            String error_message = fmt::format("Invalid MergeFlag: {}", u8(this->merge_flag_));
            UnrecoverableError(error_message);
        }
    }
    return MergeFlag::kInvalid;
};

AddDBEntryOp::AddDBEntryOp(DBEntry *db_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY, db_entry, commit_ts), db_entry_dir_(db_entry->db_entry_dir()) {}

AddTableEntryOp::AddTableEntryOp(TableEntry *table_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, table_entry, commit_ts), table_entry_dir_(table_entry->TableEntryDir()),
      column_defs_(table_entry->column_defs()), row_count_(table_entry->row_count()), // TODO: fix it
      unsealed_id_(table_entry->unsealed_id()), next_segment_id_(table_entry->next_segment_id()), next_column_id_(table_entry->next_column_id()) {}

AddSegmentEntryOp::AddSegmentEntryOp(SegmentEntry *segment_entry, TxnTimeStamp commit_ts, String segment_filter_binary_data)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY, segment_entry, commit_ts), status_(segment_entry->status()),
      column_count_(segment_entry->column_count()), row_count_(segment_entry->row_count()), // FIXME: use append_state
      actual_row_count_(segment_entry->actual_row_count()),                                 // FIXME: use append_state
      row_capacity_(segment_entry->row_capacity()), min_row_ts_(segment_entry->min_row_ts()), max_row_ts_(segment_entry->max_row_ts()),
      first_delete_ts_(segment_entry->first_delete_ts()), deprecate_ts_(segment_entry->deprecate_ts()),
      segment_filter_binary_data_(std::move(segment_filter_binary_data)) {}

AddBlockEntryOp::AddBlockEntryOp(BlockEntry *block_entry, TxnTimeStamp commit_ts, String block_filter_binary_data)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY, block_entry, commit_ts), block_entry_(block_entry),
      row_capacity_(block_entry->row_capacity()), row_count_(block_entry->row_count()), min_row_ts_(block_entry->min_row_ts()),
      max_row_ts_(block_entry->max_row_ts()), checkpoint_ts_(block_entry->checkpoint_ts()),
      checkpoint_row_count_(block_entry->checkpoint_row_count()), block_filter_binary_data_(std::move(block_filter_binary_data)) {
    String file_path = fmt::format("{}/{}", *block_entry->block_dir(), *BlockVersion::FileName());
    local_paths_.push_back(file_path);
}

AddColumnEntryOp::AddColumnEntryOp(BlockColumnEntry *column_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, column_entry, commit_ts), column_entry_(column_entry) {
    outline_info_ = {column_entry->OutlineBufferCount(), column_entry->LastChunkOff()};
    Vector<String> paths = column_entry->FilePaths();
    local_paths_.insert(local_paths_.end(), paths.begin(), paths.end());
}

AddTableIndexEntryOp::AddTableIndexEntryOp(TableIndexEntry *table_index_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY, table_index_entry, commit_ts), index_dir_(table_index_entry->index_dir()),
      index_base_(table_index_entry->table_index_def()) {}

AddSegmentIndexEntryOp::AddSegmentIndexEntryOp(SegmentIndexEntry *segment_index_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY, segment_index_entry, commit_ts), segment_index_entry_(segment_index_entry),
      min_ts_(segment_index_entry->min_ts()), max_ts_(segment_index_entry->max_ts()), next_chunk_id_(segment_index_entry->next_chunk_id()) {}

AddChunkIndexEntryOp::AddChunkIndexEntryOp(ChunkIndexEntry *chunk_index_entry, TxnTimeStamp commit_ts)
    : CatalogDeltaOperation(CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY, chunk_index_entry, commit_ts), base_name_(chunk_index_entry->base_name_),
      base_rowid_(chunk_index_entry->base_rowid_), row_count_(chunk_index_entry->row_count_), deprecate_ts_(chunk_index_entry->deprecate_ts_) {
    SegmentIndexEntry *segment_index_entry = chunk_index_entry->segment_index_entry_;
    IndexType index_type = segment_index_entry->table_index_entry()->index_base()->index_type_;
    switch (index_type) {
        case IndexType::kFullText: {
            Path rela_dir = *(segment_index_entry->index_dir());
            local_paths_.push_back(rela_dir / (chunk_index_entry->base_name_ + POSTING_SUFFIX));
            local_paths_.push_back(rela_dir / (chunk_index_entry->base_name_ + DICT_SUFFIX));
            local_paths_.push_back(rela_dir / (chunk_index_entry->base_name_ + LENGTH_SUFFIX));
            break;
        }
        case IndexType::kHnsw:
        case IndexType::kEMVB:
        case IndexType::kSecondary:
        case IndexType::kBMP: {
            Path rela_dir = *(segment_index_entry->index_dir());
            String file_name = ChunkIndexEntry::IndexFileName(segment_index_entry->segment_id(), chunk_index_entry->chunk_id_);
            String rela_path = rela_dir / file_name;
            local_paths_.push_back(rela_path);
            break;
        }
        default: {
            String error_message = "Unsupported index type when add delta catalog op.";
            UnrecoverableError(error_message);
        }
    }
}

UniquePtr<AddDBEntryOp> AddDBEntryOp::ReadAdv(const char *&ptr) {
    auto add_db_op = MakeUnique<AddDBEntryOp>();
    add_db_op->ReadAdvBase(ptr);

    add_db_op->db_entry_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));
    return add_db_op;
}

UniquePtr<AddTableEntryOp> AddTableEntryOp::ReadAdv(const char *&ptr, const char *ptr_end) {
    auto add_table_op = MakeUnique<AddTableEntryOp>();
    add_table_op->ReadAdvBase(ptr);

    add_table_op->table_entry_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));

    i32 columns_size = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnDef>> columns;
    for (i32 i = 0; i < columns_size; i++) {
        i64 id = ReadBufAdv<i64>(ptr);
        SizeT max_bytes = ptr_end - ptr;
        if (max_bytes <= 0) {
            String error_message = "ptr goes out of range when reading TableDef";
            UnrecoverableError(error_message);
        }
        SharedPtr<DataType> column_type = DataType::ReadAdv(ptr, max_bytes);
        String column_name = ReadBufAdv<String>(ptr);
        i32 constraints_size = ReadBufAdv<i32>(ptr);
        std::set<ConstraintType> constraints;
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
    add_table_op->next_column_id_ = ReadBufAdv<ColumnID>(ptr);
    return add_table_op;
}

UniquePtr<AddSegmentEntryOp> AddSegmentEntryOp::ReadAdv(const char *&ptr) {
    auto add_segment_op = MakeUnique<AddSegmentEntryOp>();
    add_segment_op->ReadAdvBase(ptr);

    add_segment_op->status_ = ReadBufAdv<SegmentStatus>(ptr);
    add_segment_op->column_count_ = ReadBufAdv<u64>(ptr);
    add_segment_op->row_count_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->actual_row_count_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->row_capacity_ = ReadBufAdv<SizeT>(ptr);
    add_segment_op->min_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->max_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->first_delete_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->deprecate_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_op->segment_filter_binary_data_ = ReadBufAdv<String>(ptr);
    return add_segment_op;
}

UniquePtr<AddBlockEntryOp> AddBlockEntryOp::ReadAdv(const char *&ptr) {
    auto add_block_op = MakeUnique<AddBlockEntryOp>();
    add_block_op->ReadAdvBase(ptr);

    add_block_op->row_count_ = ReadBufAdv<u16>(ptr);
    add_block_op->row_capacity_ = ReadBufAdv<u16>(ptr);
    add_block_op->min_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->max_row_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->checkpoint_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_block_op->checkpoint_row_count_ = ReadBufAdv<u16>(ptr);
    add_block_op->block_filter_binary_data_ = ReadBufAdv<String>(ptr);
    return add_block_op;
}

UniquePtr<AddColumnEntryOp> AddColumnEntryOp::ReadAdv(const char *&ptr) {
    auto add_column_op = MakeUnique<AddColumnEntryOp>();
    add_column_op->ReadAdvBase(ptr);
    auto &[outline_buffer_count, last_chunk_offset] = add_column_op->outline_info_;
    outline_buffer_count = ReadBufAdv<u32>(ptr);
    last_chunk_offset = ReadBufAdv<u64>(ptr);
    return add_column_op;
}

UniquePtr<AddTableIndexEntryOp> AddTableIndexEntryOp::ReadAdv(const char *&ptr, const char *ptr_end) {
    auto add_table_index_op = MakeUnique<AddTableIndexEntryOp>();
    add_table_index_op->ReadAdvBase(ptr);

    if (add_table_index_op->merge_flag_ != MergeFlag::kDelete) {
        add_table_index_op->index_dir_ = MakeShared<String>(ReadBufAdv<String>(ptr));
        add_table_index_op->index_base_ = IndexBase::ReadAdv(ptr, ptr_end - ptr);
    }
    return add_table_index_op;
}

UniquePtr<AddSegmentIndexEntryOp> AddSegmentIndexEntryOp::ReadAdv(const char *&ptr) {
    auto add_segment_index_op = MakeUnique<AddSegmentIndexEntryOp>();
    add_segment_index_op->ReadAdvBase(ptr);

    add_segment_index_op->min_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_index_op->max_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    add_segment_index_op->next_chunk_id_ = ReadBufAdv<ChunkID>(ptr);
    return add_segment_index_op;
}

UniquePtr<AddChunkIndexEntryOp> AddChunkIndexEntryOp::ReadAdv(const char *&ptr) {
    auto add_chunk_index_op = MakeUnique<AddChunkIndexEntryOp>();
    add_chunk_index_op->ReadAdvBase(ptr);

    add_chunk_index_op->base_name_ = ReadBufAdv<String>(ptr);
    add_chunk_index_op->base_rowid_ = RowID::FromUint64(ReadBufAdv<u64>(ptr));
    add_chunk_index_op->row_count_ = ReadBufAdv<u32>(ptr);
    add_chunk_index_op->deprecate_ts_ = ReadBufAdv<u64>(ptr);
    return add_chunk_index_op;
}

SizeT AddDBEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(i32) + db_entry_dir_->size();
    return total_size;
}

SizeT AddTableEntryOp::GetSizeInBytes() const {
    auto total_size = 0;
    total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(i32) + this->table_entry_dir_->size();

    total_size += sizeof(i32);
    for (u32 i = 0; i < column_defs_.size(); i++) {
        const ColumnDef &cd = *column_defs_[i];
        total_size += sizeof(i64);
        total_size += cd.column_type_->GetSizeInBytes();
        total_size += sizeof(i32) + cd.name_.length();
        total_size += sizeof(i32);
        total_size += cd.constraints_.size() * sizeof(ConstraintType);
        auto const_expr = dynamic_cast<ConstantExpr *>(cd.default_expr_.get());
        total_size += const_expr->GetSizeInBytes();
    }

    total_size += sizeof(SizeT);
    total_size += sizeof(SegmentID) * 2;
    total_size += sizeof(ColumnID);
    return total_size;
}

SizeT AddSegmentEntryOp::GetSizeInBytes() const {
    auto total_size = 0;
    total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(SegmentStatus);
    total_size += sizeof(u64);
    total_size += sizeof(SizeT);
    total_size += sizeof(actual_row_count_);
    total_size += sizeof(SizeT);
    total_size += sizeof(TxnTimeStamp) * 4;
    total_size += sizeof(i32) + segment_filter_binary_data_.size();
    return total_size;
}

SizeT AddBlockEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(u16) + sizeof(u16) + sizeof(TxnTimeStamp) * 2;
    total_size += sizeof(TxnTimeStamp) + sizeof(u16);
    total_size += sizeof(i32) + this->block_filter_binary_data_.size();
    return total_size;
}

SizeT AddColumnEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += (sizeof(u32) + sizeof(u64));
    return total_size;
}

SizeT AddTableIndexEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    if (merge_flag_ != MergeFlag::kDelete) {
        total_size += sizeof(i32) + this->index_dir_->size();
        total_size += this->index_base_->GetSizeInBytes();
    }
    return total_size;
}

SizeT AddSegmentIndexEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(TxnTimeStamp) + sizeof(TxnTimeStamp);
    total_size += sizeof(ChunkID);
    return total_size;
}

SizeT AddChunkIndexEntryOp::GetSizeInBytes() const {
    auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
    total_size += sizeof(i32) + this->base_name_.size();
    total_size += sizeof(RowID);
    total_size += sizeof(u32);
    total_size += sizeof(TxnTimeStamp);
    return total_size;
}

void AddDBEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    WriteBufAdv(buf, *this->db_entry_dir_);
}

void AddTableEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
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
    WriteBufAdv(buf, this->next_column_id_);
}

void AddSegmentEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->status_);
    WriteBufAdv(buf, this->column_count_);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->actual_row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
    WriteBufAdv(buf, this->first_delete_ts_);
    WriteBufAdv(buf, this->deprecate_ts_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
}

void AddBlockEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->row_capacity_);
    WriteBufAdv(buf, this->min_row_ts_);
    WriteBufAdv(buf, this->max_row_ts_);
    WriteBufAdv(buf, this->checkpoint_ts_);
    WriteBufAdv(buf, this->checkpoint_row_count_);
    WriteBufAdv(buf, this->block_filter_binary_data_);
}

void AddColumnEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    const auto &[outline_buffer_count, last_chunk_offset] = outline_info_;
    WriteBufAdv(buf, outline_buffer_count);
    WriteBufAdv(buf, last_chunk_offset);
}

void AddTableIndexEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    if (merge_flag_ != MergeFlag::kDelete) {
        WriteBufAdv(buf, *this->index_dir_);
        index_base_->WriteAdv(buf);
    }
}

void AddSegmentIndexEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->min_ts_);
    WriteBufAdv(buf, this->max_ts_);
    WriteBufAdv(buf, this->next_chunk_id_);
}

void AddChunkIndexEntryOp::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, this->type_);
    WriteAdvBase(buf);
    WriteBufAdv(buf, this->base_name_);
    WriteBufAdv(buf, this->base_rowid_.ToUint64());
    WriteBufAdv(buf, this->row_count_);
    WriteBufAdv(buf, this->deprecate_ts_);
}

const String AddDBEntryOp::ToString() const {
    return fmt::format("AddDBEntryOp {} db_entry_dir: {}",
                       CatalogDeltaOperation::ToString(),
                       db_entry_dir_.get() != nullptr ? *db_entry_dir_ : "nullptr");
}

const String AddTableEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddTableEntryOp {} table_entry_dir: {}",
                           CatalogDeltaOperation::ToString(),
                           table_entry_dir_.get() != nullptr ? *table_entry_dir_ : "nullptr");
    for (const auto &column_def : column_defs_) {
        sstream << fmt::format(" column_def: {}", column_def->ToString());
    }
    sstream << fmt::format(" row_count: {}", row_count_) << fmt::format(" unsealed_id: {}", unsealed_id_)
            << fmt::format(" next_segment_id: {}", next_segment_id_) << fmt::format(" next_column_id: {}", next_column_id_);
    return sstream.str();
}

const String AddSegmentEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddSegmentEntryOp {}", CatalogDeltaOperation::ToString());

    sstream << fmt::format(" min_row_ts: {} max_row_ts: {} first_delete_ts: {} row_capacity: {} row_count: {} actual_row_count: {} column_count: {}",
                           min_row_ts_,
                           max_row_ts_,
                           first_delete_ts_,
                           row_capacity_,
                           row_count_,
                           actual_row_count_,
                           column_count_);
    return sstream.str();
}

const String AddBlockEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddBlockEntryOp {} row_count: {} row_capacity: {} min_row_ts: {} "
                           "max_row_ts: {}",
                           CatalogDeltaOperation::ToString(),
                           row_count_,
                           row_capacity_,
                           min_row_ts_,
                           max_row_ts_);

    sstream << fmt::format(" checkpoint_ts: {} checkpoint_row_count: {}", checkpoint_ts_, checkpoint_row_count_);

    return sstream.str();
}

const String AddColumnEntryOp::ToString() const {
    std::stringstream sstream;
    sstream << fmt::format("AddColumnEntryOp {} outline_infos: [", CatalogDeltaOperation::ToString());
    const auto &[outline_buffer_count, last_chunk_offset] = outline_info_;
    sstream << fmt::format("outline_buffer_count: {}, last_chunk_offset: {}", outline_buffer_count, last_chunk_offset);
    sstream << "]";
    return sstream.str();
}

const String AddTableIndexEntryOp::ToString() const {
    bool is_delete = merge_flag_ == MergeFlag::kDelete;
    return fmt::format("AddTableIndexEntryOp {} index_dir: {} index_base: {}",
                       CatalogDeltaOperation::ToString(),
                       !is_delete ? *index_dir_ : "nullptr",
                       !is_delete ? index_base_->ToString() : "nullptr");
}

const String AddSegmentIndexEntryOp::ToString() const {
    return fmt::format("AddSegmentIndexEntryOp {} min_ts: {} max_ts: {}, next_chunk_id: {}",
                       CatalogDeltaOperation::ToString(),
                       min_ts_,
                       max_ts_,
                       next_chunk_id_);
}

const String AddChunkIndexEntryOp::ToString() const {
    return fmt::format("AddChunkIndexEntryOp base_name: {} base_rowid: {} row_count: {} commit_ts: {} deprecate_ts: {}",
                       base_name_,
                       base_rowid_.ToUint64(),
                       row_count_,
                       commit_ts_,
                       deprecate_ts_);
}

bool AddDBEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddDBEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*db_entry_dir_, *rhs_op->db_entry_dir_);
}

bool AddTableEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddTableEntryOp *>(&rhs);
    bool res = rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*table_entry_dir_, *rhs_op->table_entry_dir_) &&
               table_entry_type_ == rhs_op->table_entry_type_ && row_count_ == rhs_op->row_count_ && unsealed_id_ == rhs_op->unsealed_id_ &&
               next_segment_id_ == rhs_op->next_segment_id_ && next_column_id_ == rhs_op->next_column_id_ &&
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
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && status_ == rhs_op->status_ && column_count_ == rhs_op->column_count_ &&
           row_count_ == rhs_op->row_count_ && actual_row_count_ == rhs_op->actual_row_count_ && row_capacity_ == rhs_op->row_capacity_ &&
           min_row_ts_ == rhs_op->min_row_ts_ && max_row_ts_ == rhs_op->max_row_ts_ && first_delete_ts_ == rhs_op->first_delete_ts_ &&
           deprecate_ts_ == rhs_op->deprecate_ts_;
}

bool AddBlockEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddBlockEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && row_count_ == rhs_op->row_count_ &&
           row_capacity_ == rhs_op->row_capacity_ && min_row_ts_ == rhs_op->min_row_ts_ && max_row_ts_ == rhs_op->max_row_ts_ &&
           checkpoint_ts_ == rhs_op->checkpoint_ts_ && checkpoint_row_count_ == rhs_op->checkpoint_row_count_;
}

bool AddColumnEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddColumnEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && outline_info_ == rhs_op->outline_info_;
}

bool AddTableIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddTableIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && IsEqual(*index_dir_, *rhs_op->index_dir_) &&
           *index_base_ == *rhs_op->index_base_;
}

bool AddSegmentIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddSegmentIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && min_ts_ == rhs_op->min_ts_ && max_ts_ == rhs_op->max_ts_ &&
           next_chunk_id_ == rhs_op->next_chunk_id_;
}

bool AddChunkIndexEntryOp::operator==(const CatalogDeltaOperation &rhs) const {
    auto *rhs_op = dynamic_cast<const AddChunkIndexEntryOp *>(&rhs);
    return rhs_op != nullptr && CatalogDeltaOperation::operator==(rhs) && base_name_ == rhs_op->base_name_ && base_rowid_ == rhs_op->base_rowid_ &&
           row_count_ == rhs_op->row_count_;
}

void AddDBEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_DATABASE_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    MergeFlag flag = this->NextDeleteFlag(other.merge_flag_);
    *this = std::move(static_cast<AddDBEntryOp &>(other));
    this->merge_flag_ = flag;
}

void AddTableEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_TABLE_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_table_op = static_cast<AddTableEntryOp &>(other);
    // LOG_INFO(fmt::format("Merge {} with {}", other.ToString(), this->ToString()));
    MergeFlag flag = this->NextDeleteFlag(add_table_op.merge_flag_);
    *this = std::move(add_table_op);
    this->merge_flag_ = flag;
}

void AddSegmentEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_segment_op = static_cast<AddSegmentEntryOp &>(other);
    MergeFlag flag = this->NextDeleteFlag(add_segment_op.merge_flag_);
    auto segment_filter_binary_data = std::move(segment_filter_binary_data_);
    *this = std::move(add_segment_op);
    this->merge_flag_ = flag;
    if (!segment_filter_binary_data.empty()) {
        if (!segment_filter_binary_data_.empty()) {
            String error_message = "Serialize segment filter binary twice";
            UnrecoverableError(error_message);
        }
        segment_filter_binary_data_ = std::move(segment_filter_binary_data);
    }
}

void AddBlockEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_BLOCK_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_block_op = static_cast<AddBlockEntryOp &>(other);
    MergeFlag flag = this->NextDeleteFlag(add_block_op.merge_flag_);
    auto block_filter_binary_data = std::move(block_filter_binary_data_);
    *this = std::move(add_block_op);
    this->merge_flag_ = flag;
    if (!block_filter_binary_data.empty()) {
        if (!block_filter_binary_data_.empty()) {
            String error_message = "Serialize block filter binary twice";
            UnrecoverableError(error_message);
        }
        block_filter_binary_data_ = std::move(block_filter_binary_data);
    }
}

void AddColumnEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_COLUMN_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    *this = std::move(static_cast<AddColumnEntryOp &>(other));
}

void AddTableIndexEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_table_index_op = static_cast<AddTableIndexEntryOp &>(other);
    MergeFlag flag = this->NextDeleteFlag(add_table_index_op.merge_flag_);
    *this = std::move(add_table_index_op);
    this->merge_flag_ = flag;
}

void AddSegmentIndexEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_segment_index_op = static_cast<AddSegmentIndexEntryOp &>(other);
    MergeFlag flag = this->NextDeleteFlag(add_segment_index_op.merge_flag_);
    *this = std::move(add_segment_index_op);
    this->merge_flag_ = flag;
}

void AddChunkIndexEntryOp::Merge(CatalogDeltaOperation &other) {
    if (other.type_ != CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY) {
        String error_message = fmt::format("Merge failed, other type: {}", other.GetTypeStr());
        UnrecoverableError(error_message);
    }
    auto &add_chunk_index_op = static_cast<AddChunkIndexEntryOp &>(other);
    MergeFlag flag = this->NextDeleteFlag(add_chunk_index_op.merge_flag_);
    *this = std::move(add_chunk_index_op);
    this->merge_flag_ = flag;
}

void AddColumnEntryOp::FlushDataToDisk(TxnTimeStamp max_commit_ts) {
    LOG_TRACE(fmt::format("ColumnEntry {} flush to disk", column_entry_->column_id()));
    column_entry_->FlushColumn(max_commit_ts);
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

UniquePtr<CatalogDeltaEntry> CatalogDeltaEntry::ReadAdv(const char *&ptr, i32 max_bytes) {
    const char *const ptr_start = ptr;
    const char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        String error_message = "ptr goes out of range when reading WalEntry";
        UnrecoverableError(error_message);
    }
    CatalogDeltaEntryHeader header;
    header.size_ = ReadBufAdv<i32>(ptr);
    header.checksum_ = ReadBufAdv<u32>(ptr);
    auto entry = MakeUnique<CatalogDeltaEntry>();
    if (const i32 size2 = ReadBuf<i32>(ptr_start + header.size_ - sizeof(i32)); header.size_ != size2) {
        return nullptr;
    }
    {
        const auto ptr_start_mutable = const_cast<char *>(ptr_start);
        WriteBuf<u32>(ptr_start_mutable + sizeof(header.size_), 0);
        DeferFn defer([&] { WriteBuf<u32>(ptr_start_mutable + sizeof(header.size_), header.checksum_); });
        if (const u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr_start), header.size_);
            header.checksum_ != checksum2) {
            UnrecoverableError(fmt::format("checksum failed, checksum: {}, checksum2: {}", header.checksum_, checksum2));
        }
    }
    entry->max_commit_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    i32 cnt = ReadBufAdv<i32>(ptr);
    for (i32 i = 0; i < cnt; i++) {
        max_bytes = ptr_end - ptr;
        if (max_bytes <= 0) {
            String error_message = "ptr goes out of range when reading WalEntry";
            UnrecoverableError(error_message);
        }
        UniquePtr<CatalogDeltaOperation> operation = CatalogDeltaOperation::ReadAdv(ptr, max_bytes);
        entry->operations_.push_back(std::move(operation));
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        String error_message = "ptr goes out of range when reading WalEntry";
        UnrecoverableError(error_message);
    }
    {
        LOG_INFO(fmt::format("Deserialize delta op count: {}", entry->operations_.size()));
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
        // LOG_TRACE(fmt::format("!Write {}", operation->ToString()));
        char *const save_ptr = ptr;
        operation->WriteAdv(ptr);
        i32 act_size = ptr - save_ptr;
        if (exp_size != act_size) {
            String error_message = fmt::format("catalog delta operation write failed, exp_size: {}, act_size: {}", exp_size, act_size);
            UnrecoverableError(error_message);
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
        String error_message = fmt::format("CatalogDeltaEntry SaveState failed, max_commit_ts_ {} txn_ids_ size {}", max_commit_ts_, txn_ids_.size());
        UnrecoverableError(error_message);
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

void GlobalCatalogDeltaEntry::AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry) {
    // {
    //     for (auto &delta_entry : delta_entries) {
    //         LOG_INFO(fmt::format("Add delta entry: {}", delta_entry->ToString()));
    //     }
    // }
    std::lock_guard<std::mutex> lock(catalog_delta_locker_);
    u64 entry_sequence = delta_entry->sequence();
    if (entry_sequence != last_sequence_ + 1) {
        // Discontinuous
        // LOG_INFO(fmt::format("Add delta entry: {} in to sequence_heap_", entry_sequence));
        sequence_heap_.push(entry_sequence);
        delta_entry_map_.emplace(entry_sequence, std::move(delta_entry));
    } else {
        // Continuous
        do {
            // LOG_INFO(fmt::format("Add delta entry: {} in to delta_ops_", entry_sequence));
            this->AddDeltaEntryInner(delta_entry.get());

            ++last_sequence_;

            if (!sequence_heap_.empty() && sequence_heap_.top() == last_sequence_ + 1) {
                auto iter = delta_entry_map_.find(sequence_heap_.top());
                if (iter == delta_entry_map_.end()) {
                    String error_message = fmt::format("sequence_heap_.top() {} in delta_entry_map_", sequence_heap_.top());
                    UnrecoverableError(error_message);
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
UniquePtr<CatalogDeltaEntry> GlobalCatalogDeltaEntry::PickFlushEntry(TxnTimeStamp &max_commit_ts) {
    auto flush_delta_entry = MakeUnique<CatalogDeltaEntry>();

    Map<String, UniquePtr<CatalogDeltaOperation>> delta_ops;
    HashSet<TransactionID> txn_ids;
    {
        std::lock_guard<std::mutex> lock(catalog_delta_locker_);
        delta_ops = std::exchange(delta_ops_, {});
        txn_ids = std::exchange(txn_ids_, {});
    }
    {
        TxnTimeStamp max_ts = 0;
        for (auto &[_, delta_op] : delta_ops) {
            if (delta_op->commit_ts_ <= last_full_ckp_ts_) { // skip the delta op that is before full checkpoint
                continue;
            }
            max_ts = std::max(max_ts, delta_op->commit_ts_);
            flush_delta_entry->AddOperation(std::move(delta_op));
        }
        max_commit_ts = std::max(max_ts, max_commit_ts_);
    }

    flush_delta_entry->set_txn_ids(Vector<TransactionID>(txn_ids.begin(), txn_ids.end()));
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
        String error_message = "max_commit_ts == UNCOMMIT_TS";
        UnrecoverableError(error_message);
    }
    max_commit_ts_ = std::max(max_commit_ts_, max_commit_ts);

    for (auto &new_op : delta_entry->operations()) {
        if (new_op->type_ == CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {
            auto *add_segment_op = static_cast<AddSegmentEntryOp *>(new_op.get());
            if (add_segment_op->status_ == SegmentStatus::kDeprecated) {
                add_segment_op->merge_flag_ = MergeFlag::kDelete;
            }
        } else if (new_op->type_ == CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY) {
            auto *add_chunk_index_op = static_cast<AddChunkIndexEntryOp *>(new_op.get());
            if (add_chunk_index_op->deprecate_ts_ != UNCOMMIT_TS) {
                add_chunk_index_op->merge_flag_ = MergeFlag::kDelete;
                LOG_DEBUG(fmt::format("Delete chunk: {} at {}", *new_op->encode_, add_chunk_index_op->deprecate_ts_));
            }
        }
        const String &encode = *new_op->encode_;
        if (encode.empty()) {
            String error_message = "encode is empty";
            UnrecoverableError(error_message);
        }
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
            try {
                op->Merge(*new_op);
            } catch (const UnrecoverableException &e) {
                std::stringstream ss;
                ss << "Merge failed, encode: " << encode << " txn_ids: ";
                for (const auto txn_id : delta_entry->txn_ids()) {
                    ss << txn_id << " ";
                }
                UnrecoverableError(ss.str());
            }
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
    auto iter = delta_ops_.lower_bound(encode1);
    while (iter != delta_ops_.end()) {
        const auto &[encode2, delta_op2] = *iter;
        auto [iter1, iter2] = std::mismatch(encode1.begin(), encode1.end(), encode2.begin());
        if (iter1 != encode1.end()) {
            break; // encode1 is not prefix of encode2
        }
        if (iter2 == encode2.end()) {
            ++iter;
            continue; // same
        }
        if (*iter2 != '#') {
            ++iter;
            continue; // not prefix
        }
        iter = delta_ops_.erase(iter); // is prefix
    }
}

Vector<CatalogDeltaOpBrief> GlobalCatalogDeltaEntry::GetOperationBriefs() const {
    Vector<CatalogDeltaOpBrief> res;
    {
        std::lock_guard<std::mutex> lock(catalog_delta_locker_);
        res.reserve(delta_ops_.size());
        for (const auto &op_pair : delta_ops_) {
            CatalogDeltaOperation *delta_op = op_pair.second.get();

            res.push_back({delta_op->begin_ts_, delta_op->commit_ts_, delta_op->txn_id_, delta_op->type_, MakeShared<String>(delta_op->ToString())});
        }
    }
    return res;
}

} // namespace infinity
