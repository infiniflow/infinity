#include "column_buffer.h"
#include "common/types/data_type.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/column_data_entry.h"
#include "storage/varchar_store.h"

namespace infinity {

ColumnBuffer::ColumnBuffer(BufferHandle *buffer_handle, BufferManager *buffer_mgr, bool is_outline) : inline_col_(buffer_handle) {
    if (is_outline) {
        outline_buffer_ = MakeUnique<OutlineBuffer>();
    }
}

ptr_t ColumnBuffer::GetAll() {
    StorageAssert(outline_buffer_ == nullptr, "Cannot get whole data of an outline column");
    return inline_col_.GetData();
}

Pair<ptr_t, SizeT> ColumnBuffer::GetAt(SizeT row_idx) {
    StorageAssert(outline_buffer_ != nullptr, "Cannot get one element of an inline column");
    auto varchar_store = reinterpret_cast<VarcharStore *>(inline_col_.GetData()) + row_idx;
    if (varchar_store->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_store->u.short_info_.data.data();
        return {ptr, varchar_store->length_};
    }
    auto &long_info = varchar_store->u.long_info_;
    if (outline_buffer_->current_file_idx_ != long_info.file_idx_) {
        auto filename = ColumnDataEntry::GetOutlineFilename(outline_buffer_->current_file_idx_);
        outline_buffer_->outline_ele_ = ObjectHandle(outline_buffer_->buffer_mgr_->GetBufferHandle(outline_buffer_->file_dir_, filename, BufferType::kFile));
        
    }
    ptr_t ptr = outline_buffer_->outline_ele_.GetData() + long_info.file_offset_;
    return {ptr, varchar_store->length_};
}

Pair<ptr_t, SizeT> ColumnBuffer::GetAtPrefix(SizeT row_idx) {
    StorageAssert(outline_buffer_ != nullptr, "Cannot get prefix of one element of an inline column");
    auto varchar_store = reinterpret_cast<VarcharStore *>(inline_col_.GetData()) + row_idx;
    if (varchar_store->length_ <= VarcharT::INLINE_LENGTH) {
        ptr_t ptr = varchar_store->u.short_info_.data.data();
        return {ptr, varchar_store->length_};
    }
    ptr_t ptr = varchar_store->u.long_info_.prefix_.data();
    return {ptr, VarcharT::PREFIX_LENGTH};
}

}

