module;

import stl;
import memory_pool;
import index_defines;
import posting_value;
module short_buffer;

namespace infinity {

ShortBuffer::ShortBuffer(MemoryPool *pool)
    : buffer_(nullptr), capacity_(0), size_(0), is_buffer_valid_(true), has_pool_(false), pool_(pool), posting_values_(nullptr) {
    if (!pool) {
        pool_ = new MemoryPool;
        has_pool_ = true;
    }
}

ShortBuffer::~ShortBuffer() {
    // here we assume this object and pool have the same life cycle
    // ReleaseBuffer((u8*)buffer_, capacity_);
    if (!has_pool_) {
        // ReleaseBuffer((u8*)buffer_, capacity_);
    }

    if (has_pool_) {
        delete pool_;
        pool_ = nullptr;
    }
}

bool ShortBuffer::Reallocate() {
    u8 old_capacity = capacity_;
    u8 *old_buffer = (u8 *)buffer_;
    u8 new_capacity = AllocatePlan(old_capacity);
    if (new_capacity == old_capacity) {
        return false;
    }
    SizeT doc_item_size = new_capacity * posting_values_->GetTotalSize();
    u8 *new_buffer = (u8 *)Allocate(doc_item_size);

    BufferMemoryCopy(new_buffer, new_capacity, (u8 *)buffer_, capacity_, posting_values_, size_);

    is_buffer_valid_ = false;

    buffer_ = new_buffer;

    capacity_ = new_capacity;

    is_buffer_valid_ = true;

    ReleaseBuffer(old_buffer, old_capacity);

    return true;
}

void ShortBuffer::ReleaseBuffer(u8 *buffer, u8 capacity) {
    if (buffer == nullptr || capacity == 0) {
        return;
    }
    SizeT bufferSize = capacity * posting_values_->GetTotalSize();
    Deallocate((void *)buffer, bufferSize);
}

void ShortBuffer::BufferMemoryCopy(u8 *dst, u8 dst_col_count, u8 *src, u8 src_col_count, const PostingValues *posting_values, u8 src_size) {
    if (src == nullptr || src_size == 0) {
        return;
    }
    for (u8 i = 0; i < posting_values->GetSize(); ++i) {
        const PostingValue *value = posting_values->GetValue(i);
        std::memcpy(GetRow(dst, dst_col_count, value), GetRow(src, src_col_count, value), src_size * value->GetSize());
    }
}

void ShortBuffer::Clear() { size_ = 0; }

bool ShortBuffer::SnapShot(ShortBuffer &short_buffer) const {
    short_buffer.Clear();

    if (short_buffer.GetRowCount() != posting_values_->GetSize()) {
        return false;
    }
    if (buffer_ == nullptr) {
        return true;
    }

    // here we assume buffer size only can be increased
    u8 snapshot_size = size_;

    // snapshot_size <= snapshot_capacity in recycle pool
    short_buffer.Reserve(snapshot_size);

    u8 snapshot_capacity = 0;
    u8 *buffer_snapshot = buffer_;
    do {
        snapshot_capacity = capacity_;
        buffer_snapshot = buffer_;

        BufferMemoryCopy((u8 *)short_buffer.buffer_,
                         short_buffer.capacity_,
                         (u8 *)buffer_snapshot,
                         snapshot_capacity,
                         posting_values_,
                         snapshot_size);
    } while (!is_buffer_valid_ || buffer_ != buffer_snapshot || capacity_ > snapshot_capacity);

    short_buffer.size_ = snapshot_size;
    return true;
}

void ShortBuffer::Reserve(u8 capacity) {
    if (buffer_ != nullptr) {
        if (capacity_ >= capacity)
            return;
    }

    SizeT doc_item_size = capacity * posting_values_->GetTotalSize();

    u8 *new_buffer = (u8 *)Allocate(doc_item_size);

    if (buffer_ != nullptr) {
        BufferMemoryCopy(new_buffer, capacity, (u8 *)buffer_, capacity_, posting_values_, size_);
        ReleaseBuffer((u8 *)buffer_, capacity_);
    }

    buffer_ = new_buffer;
    capacity_ = capacity;
}

u8 ShortBuffer::AllocatePlan(u8 cur_capacity) {
    if (cur_capacity < 2) {
        return 2;
    } else if (cur_capacity < 16) {
        return 16;
    }
    return MAX_DOC_PER_RECORD;
}

} // namespace infinity
