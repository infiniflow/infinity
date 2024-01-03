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
import buffer_handle;
import buffer_manager;
import infinity_exception;

import varchar_layout;
import parser;
import buffer_obj;
import data_file_worker;
import default_values;
import column_buffer;
import catalog;

module column_buffer;
namespace infinity {

const_ptr_t ColumnBuffer::GetAll() {
    if (outline_buffer_.get() == nullptr) {
        return static_cast<const_ptr_t>(inline_col_.GetData());
    }
    Error<NotImplementException>("Cannot get all data of an outline column");
    return nullptr;
}

Pair<const_ptr_t, SizeT> ColumnBuffer::GetVarcharAt(SizeT row_idx) {
    if (outline_buffer_.get() == nullptr) {
        Error<StorageException>("Cannot get one element of an inline column");
    }
    auto varchar_layout = reinterpret_cast<const VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VARCHAR_INLINE_LEN) {
        const_ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = BlockColumnEntry::OutlineFilename(column_id_, long_info.file_idx_);
        auto base_dir = outline_buffer_->base_dir_;
        auto file_worker = MakeUnique<DataFileWorker>(base_dir, filename, DEFAULT_OUTLINE_FILE_MAX_SIZE);
        auto buffer_obj = outline_buffer_->buffer_mgr_->Get(Move(file_worker));
        outline_buffer_->outline_ele_ = buffer_obj->Load();
        outline_buffer_->current_file_idx_ = long_info.file_idx_;
    }
    auto ptr = static_cast<const_ptr_t>(outline_buffer_->outline_ele_.GetData()) + long_info.file_offset_;
    return {ptr, varchar_layout->length_};
}

Pair<const_ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefix(SizeT row_idx) {\
    if (outline_buffer_.get() == nullptr) {
        Error<StorageException>("Cannot get one element of an inline column");
    }
    auto varchar_layout = static_cast<const VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VARCHAR_INLINE_LEN) {
        const_ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    const_ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VARCHAR_PREFIX_LEN};
}

const_ptr_t ColumnBuffer::GetValueAt(SizeT row_idx, const DataType &data_type) {
    if (data_type.Plain()) {
        return static_cast<const_ptr_t>(inline_col_.GetData()) + data_type.Size() * row_idx;
    } else {
        switch (data_type.type()) {
            case LogicalType::kVarchar:
            case LogicalType::kArray:
            case LogicalType::kTuple:
//            case LogicalType::kPath:
//            case LogicalType::kPolygon:
//            case LogicalType::kBlob:
            case LogicalType::kMixed: {
                Error<NotImplementException>("Not implement complex type GetValueAt function");
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type");
            }
            default:
                break;
        }
    }
    return nullptr;
}

ptr_t ColumnBuffer::GetAllMut() {
    if (outline_buffer_.get() == nullptr) {
        return static_cast<ptr_t>(inline_col_.GetDataMut());
    }
    Error<NotImplementException>("Cannot get all data of an outline column");
    return nullptr;
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefixMut(SizeT row_idx) {
    if (outline_buffer_.get() == nullptr) {
        Error<StorageException>("Cannot get one element of an inline column");
    }
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetDataMut()) + row_idx;
    if (varchar_layout->length_ <= VARCHAR_INLINE_LEN) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, u64(varchar_layout->length_)};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = BlockColumnEntry::OutlineFilename(column_id_, long_info.file_idx_);
        auto base_dir = outline_buffer_->base_dir_;
        auto file_worker = MakeUnique<DataFileWorker>(base_dir, filename, DEFAULT_OUTLINE_FILE_MAX_SIZE);
        auto buffer_obj = outline_buffer_->buffer_mgr_->Get(Move(file_worker));
        outline_buffer_->outline_ele_ = buffer_obj->Load();
        outline_buffer_->current_file_idx_ = long_info.file_idx_;
    }
    auto ptr = static_cast<ptr_t>(outline_buffer_->outline_ele_.GetDataMut()) + long_info.file_offset_;
    return {ptr, u64(varchar_layout->length_)};
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAtMut(SizeT row_idx) {
    if (outline_buffer_.get() == nullptr) {
        Error<StorageException>("Cannot get one element of an inline column");
    }
    auto varchar_layout = static_cast<VarcharLayout *>(inline_col_.GetDataMut()) + row_idx;
    if (varchar_layout->length_ <= VARCHAR_INLINE_LEN) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, u64(varchar_layout->length_)};
    }
    ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VARCHAR_PREFIX_LEN};
}

ptr_t ColumnBuffer::GetValueAtMut(SizeT row_idx, const DataType &data_type) {
    if (data_type.Plain()) {
        return static_cast<ptr_t>(inline_col_.GetDataMut()) + data_type.Size() * row_idx;
    } else {
        switch (data_type.type()) {
            case kVarchar:
            case kArray:
            case kTuple:
//            case kPath:
//            case kPolygon:
//            case kBlob:
            case kMixed: {
                Error<NotImplementException>("Not implement complex type GetValueAt function");
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type");
            }
            default:
                break;
        }
    }
    return nullptr;
}

} // namespace infinity
