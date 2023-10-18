
module;

import stl;
import buffer_handle;
import buffer_manager;
import infinity_exception;
import infinity_assert;
import varchar_layout;
import parser;

module column_buffer;

namespace infinity {

ColumnBuffer::ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool is_outline) : inline_col_(buffer_handle) {
#if 0
    if (is_outline) {
        outline_buffer_ = MakeUnique<OutlineBuffer>(buffer_mgr);
    }
#endif
}

const ptr_t ColumnBuffer::GetAll() {
#if 0
    if (outline_buffer_.get() == nullptr) {
        return inline_col_.GetData();
    }
    Error<TypeException>("Cannot get all data of an outline column", __FILE_NAME__, __LINE__);
#endif
}

Pair<const ptr_t, SizeT> ColumnBuffer::GetVarcharAt(SizeT row_idx) {
#if 0
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get one element of an inline column", __FILE_NAME__, __LINE__);
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = SegmentColumnEntry::OutlineFilename(long_info.file_idx_);
        auto buffer_handle = outline_buffer_->buffer_mgr_->GetBufferHandle(inline_col_.GetDir(), filename, BufferType::kFile);
        outline_buffer_->outline_ele_ = CommonObjectHandle(buffer_handle);
        outline_buffer_->current_file_idx_ = long_info.file_idx_;
    }
    ptr_t ptr = outline_buffer_->outline_ele_.GetData() + long_info.file_offset_;
    return {ptr, varchar_layout->length_};
#endif
}

Pair<const ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefix(SizeT row_idx) {
#if 0
    Assert<StorageException>(outline_buffer_.get() != nullptr, "Cannot get prefix of one element of an inline column", __FILE_NAME__, __LINE__);
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VarcharT::PREFIX_LENGTH};
#endif
}

ptr_t ColumnBuffer::GetValueAt(SizeT row_idx, const DataType &data_type) {
#if 0
    if (data_type.Plain()) {
        return inline_col_.GetData() + data_type.Size() * row_idx;
    } else {
        switch (data_type.type()) {
            case kVarchar:
            case kArray:
            case kTuple:
            case kPath:
            case kPolygon:
            case kBlob:
            case kMixed: {
                Error<NotImplementException>("Not implement complex type GetValueAt function", __FILE_NAME__, __LINE__);
            }
            case kInvalid: {
                Error<ExecutorException>("Invalid data type", __FILE_NAME__, __LINE__);
            }
            default:
                break;
        }
    }
#endif
    return nullptr;
}

} // namespace infinity
