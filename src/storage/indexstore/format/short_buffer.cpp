#include "short_buffer.h"
#include "storage/indexstore/index_defines.h"

namespace infinity {

ShortBuffer::ShortBuffer(MemoryPool* pool)
    : buffer_(nullptr)
    , capacity_(0)
    , size_(0)
    , is_buffer_valid_(true)
    , has_pool_(false)
    , pool_(pool)
    , posting_values_(nullptr) {
    if (!pool_) {
        pool_ = new MemoryPool;
        has_pool_ = true;
    }
}

ShortBuffer::~ShortBuffer() {
    // here we assume this object and pool have the same life cycle
    // ReleaseBuffer((uint8_t*)buffer_, capacity_);
    if (!has_pool_) {
        // ReleaseBuffer((uint8_t*)buffer_, capacity_);
    }

    if (has_pool_) {
        delete pool_;
        pool_ = nullptr;
    }
}

bool ShortBuffer::Reallocate() {
    uint8_t old_capacity = capacity_;
    uint8_t* old_buffer = (uint8_t*)buffer_;
    uint8_t new_capacity = AllocatePlan(old_capacity);
    if (new_capacity == old_capacity) {
        return false;
    }
    size_t doc_item_size = new_capacity * posting_values_->GetTotalSize();
    uint8_t* new_buffer = (uint8_t*)Allocate(doc_item_size);
    assert(new_buffer);

    BufferMemoryCopy(new_buffer, new_capacity, (uint8_t*)buffer_, capacity_, posting_values_, size_);

    is_buffer_valid_ = false;

    buffer_ = new_buffer;

    capacity_ = new_capacity;

    is_buffer_valid_ = true;

    ReleaseBuffer(old_buffer, old_capacity);

    return true;
}

void ShortBuffer::ReleaseBuffer(uint8_t* buffer, uint8_t capacity) {
    if (buffer == nullptr || capacity == 0) {
        return;
    }
    size_t bufferSize = capacity * posting_values_->GetTotalSize();
    Deallocate((void*)buffer, bufferSize);
}

void ShortBuffer::BufferMemoryCopy(
    uint8_t* dst, uint8_t dst_col_count, 
    uint8_t* src, uint8_t src_col_count, 
    const PostingValues* posting_values, uint8_t src_size) {
    if (src == nullptr || src_size == 0) {
        return;
    }
    for (uint8_t i = 0; i < posting_values->GetSize(); ++i) {
        const PostingValue* value = posting_values->GetValue(i);
        memcpy(
            GetRow(dst, dst_col_count, value), 
            GetRow(src, src_col_count, value), 
            src_size * value->GetSize());
    }
}

void ShortBuffer::Clear() {
    size_ = 0;
}

bool ShortBuffer::SnapShot(ShortBuffer& short_buffer) const {
    short_buffer.Clear();

    if (short_buffer.GetRowCount() != posting_values_->GetSize()) {
        return false;
    }
    if (buffer_ == nullptr) {
        return true;
    }

    // here we assume buffer size only can be increased
    uint8_t snapshot_size = size_;

    // snapshot_size <= snapshot_capacity in recycle pool
    short_buffer.Reserve(snapshot_size);

    uint8_t snapshot_capacity = 0;
    uint8_t* buffer_snapshot = buffer_;
    do {
        snapshot_capacity = capacity_;
        buffer_snapshot = buffer_;

        BufferMemoryCopy(
            (uint8_t*)short_buffer.buffer_, short_buffer.capacity_, 
            (uint8_t*)buffer_snapshot, snapshot_capacity, 
            posting_values_, snapshot_size);
    } while (!is_buffer_valid_ || buffer_ != buffer_snapshot || capacity_ > snapshot_capacity);

    short_buffer.size_ = snapshot_size;
    return true;
}

void ShortBuffer::Reserve(uint8_t capacity) {
    if (buffer_ != nullptr) {
        if (capacity_ >= capacity)
            return;
    }

    size_t doc_item_size = capacity * posting_values_->GetTotalSize();

    uint8_t* new_buffer = (uint8_t*)Allocate(doc_item_size);

    if (buffer_ != nullptr) {
        BufferMemoryCopy(new_buffer, capacity, (uint8_t*)buffer_, capacity_, posting_values_, size_);
        ReleaseBuffer((uint8_t*)buffer_, capacity_);
    }

    buffer_ = new_buffer;
    capacity_ = capacity;
}

uint8_t ShortBuffer::AllocatePlan(uint8_t cur_capacity) {
    if (cur_capacity < 2) {
        return 2;
    } else if (cur_capacity < 16) {
        return 16;
    }
    return MAX_DOC_PER_RECORD;
}

}
