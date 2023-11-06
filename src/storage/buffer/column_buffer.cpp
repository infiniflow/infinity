module;

import stl;
import buffer_handle;
import buffer_manager;
import infinity_exception;

import varchar_layout;
import parser;
import block_column_entry;
import buffer_obj;
import data_file_worker;
import default_values;
import column_buffer;

module column_buffer;
namespace infinity {

const_ptr_t ColumnBuffer::GetAll() {
    if (outline_buffer_.get() == nullptr) {
        return static_cast<const_ptr_t>(inline_col_.GetData());
    }
    Error<NotImplementException>("Cannot get all data of an outline column");
}

Pair<const_ptr_t, SizeT> ColumnBuffer::GetVarcharAt(SizeT row_idx) {
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get one element of an inline column");
    auto varchar_layout = reinterpret_cast<const VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        const_ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = BlockColumnEntry::OutlineFilename(long_info.file_idx_);
        auto base_dir = outline_buffer_->base_dir_;
        auto file_worker = MakeUnique<DataFileWorker>(base_dir, filename, DEFAULT_OUTLINE_FILE_MAX_SIZE);
        auto buffer_obj = outline_buffer_->buffer_mgr_->Get(Move(file_worker));
        outline_buffer_->outline_ele_ = buffer_obj->Load();
        outline_buffer_->current_file_idx_ = long_info.file_idx_;
    }
    auto ptr = static_cast<const_ptr_t>(outline_buffer_->outline_ele_.GetData()) + long_info.file_offset_;
    return {ptr, varchar_layout->length_};
}

Pair<const_ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefix(SizeT row_idx) {
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get prefix of one element of an inline column");
    auto varchar_layout = static_cast<const VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        const_ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    const_ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VarcharT::PREFIX_LENGTH};
}

const_ptr_t ColumnBuffer::GetValueAt(SizeT row_idx, const DataType &data_type) {
    if (data_type.Plain()) {
        return static_cast<const_ptr_t>(inline_col_.GetData()) + data_type.Size() * row_idx;
    } else {
        switch (data_type.type()) {
            case kVarchar:
            case kArray:
            case kTuple:
            case kPath:
            case kPolygon:
            case kBlob:
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

ptr_t ColumnBuffer::GetAllMut() {
    if (outline_buffer_.get() == nullptr) {
        return static_cast<ptr_t>(inline_col_.GetDataMut());
    }
    Error<NotImplementException>("Cannot get all data of an outline column");
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefixMut(SizeT row_idx) {
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get one element of an inline column");
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetDataMut()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = BlockColumnEntry::OutlineFilename(long_info.file_idx_);
        auto base_dir = outline_buffer_->base_dir_;
        auto file_worker = MakeUnique<DataFileWorker>(base_dir, filename, DEFAULT_OUTLINE_FILE_MAX_SIZE);
        auto buffer_obj = outline_buffer_->buffer_mgr_->Get(Move(file_worker));
        outline_buffer_->outline_ele_ = buffer_obj->Load();
        outline_buffer_->current_file_idx_ = long_info.file_idx_;
    }
    auto ptr = static_cast<ptr_t>(outline_buffer_->outline_ele_.GetDataMut()) + long_info.file_offset_;
    return {ptr, varchar_layout->length_};
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAtMut(SizeT row_idx) {
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get prefix of one element of an inline column");
    auto varchar_layout = static_cast<VarcharLayout *>(inline_col_.GetDataMut()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VarcharT::PREFIX_LENGTH};
}

ptr_t ColumnBuffer::GetValueAtMut(SizeT row_idx, const DataType &data_type) {
    if (data_type.Plain()) {
        return static_cast<ptr_t>(inline_col_.GetDataMut()) + data_type.Size() * row_idx;
    } else {
        switch (data_type.type()) {
            case kVarchar:
            case kArray:
            case kTuple:
            case kPath:
            case kPolygon:
            case kBlob:
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
