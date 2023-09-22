#include "column_buffer.h"
#include "common/types/data_type.h"
#include "common/types/varchar_layout.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/column_data_entry.h"

namespace infinity {

ColumnBuffer::ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool is_outline) : inline_col_(buffer_handle) {
    if (is_outline) {
        outline_buffer_ = MakeUnique<OutlineBuffer>(buffer_mgr, buffer_handle->current_dir_);
    }
}

ptr_t ColumnBuffer::GetAll() {
    // StorageAssert(outline_buffer_ == nullptr, "Cannot get whole data of an outline column");
    return inline_col_.GetData();
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAt(SizeT row_idx) {
    StorageAssert(outline_buffer_ != nullptr, "Cannot get one element of an inline column");
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    auto &long_info = varchar_layout->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = ColumnDataEntry::GetOutlineFilename(long_info.file_idx_);
        auto buffer_handle = outline_buffer_->buffer_mgr_->GetBufferHandle(outline_buffer_->file_dir_, filename, BufferType::kFile);
        outline_buffer_->outline_ele_ = ObjectHandle(buffer_handle);
    }
    ptr_t ptr = outline_buffer_->outline_ele_.GetData() + long_info.file_offset_;
    return {ptr, varchar_layout->length_};
}

Pair<ptr_t, SizeT> ColumnBuffer::GetVarcharAtPrefix(SizeT row_idx) {
    StorageAssert(outline_buffer_ != nullptr, "Cannot get prefix of one element of an inline column");
    auto varchar_layout = reinterpret_cast<VarcharLayout *>(inline_col_.GetData()) + row_idx;
    if (varchar_layout->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_layout->u.short_info_.data.data();
        return {ptr, varchar_layout->length_};
    }
    ptr_t ptr = varchar_layout->u.long_info_.prefix_.data();
    return {ptr, VarcharT::PREFIX_LENGTH};
}

}

