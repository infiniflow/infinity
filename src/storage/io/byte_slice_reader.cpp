#include "byte_slice_reader.h"

#include <ostream>
#include <stdint.h>
#include <string.h>
#include <string>

namespace infinity {

ByteSliceReader::ByteSliceReader() : current_slice_(nullptr), current_slice_offset_(0), global_offset_(0), slice_list_(nullptr), size_(0) {}

ByteSliceReader::ByteSliceReader(ByteSliceList *slice_list) : ByteSliceReader() { Open(slice_list); }

void ByteSliceReader::Open(ByteSliceList *slice_list) {
    Close();
    slice_list_ = slice_list;
    size_ = GetSize();

    auto head_slice = slice_list_->GetHead();
    current_slice_ = head_slice;
    global_offset_ = 0;
    current_slice_offset_ = 0;

    if (current_slice_ == nullptr) {
        StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
    }
}

void ByteSliceReader::Open(ByteSlice *slice) {
    Close();
    current_slice_ = slice;
    global_offset_ = 0;
    current_slice_offset_ = 0;

    if (current_slice_ == nullptr) {
        StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
    }
}

void ByteSliceReader::Close() {
    current_slice_ = nullptr;
    global_offset_ = 0;
    current_slice_offset_ = 0;
    size_ = 0;
    if (slice_list_) {
        slice_list_ = nullptr;
    }
}

size_t ByteSliceReader::Read(void *value, size_t len) {
    if (current_slice_ == nullptr || len == 0) {
        return 0;
    }

    if (current_slice_offset_ + len <= GetSliceDataSize(current_slice_)) {
        memcpy(value, current_slice_->data_ + current_slice_offset_, len);
        current_slice_offset_ += len;
        global_offset_ += len;
        return len;
    }
    // current byteslice is not long enough, read next byteslices
    char *dest = (char *)value;
    int64_t total_len = (int64_t)len;
    size_t offset = current_slice_offset_;
    int64_t leftLen = 0;
    while (total_len > 0) {
        leftLen = GetSliceDataSize(current_slice_) - offset;
        if (leftLen < total_len) {
            memcpy(dest, current_slice_->data_ + offset, leftLen);
            total_len -= leftLen;
            dest += leftLen;

            offset = 0;
            current_slice_ = NextSlice(current_slice_);
            if (!current_slice_) {
                break;
            }
        } else {
            memcpy(dest, current_slice_->data_ + offset, total_len);
            dest += total_len;
            offset += (size_t)total_len;
            total_len = 0;
        }
    }

    current_slice_offset_ = offset;
    size_t read_len = (size_t)(len - total_len);
    global_offset_ += read_len;
    return read_len;
}

size_t ByteSliceReader::ReadMayCopy(void *&value, size_t len) {
    if (current_slice_ == nullptr || len == 0)
        return 0;

    if (current_slice_offset_ + len <= GetSliceDataSize(current_slice_)) {
        value = current_slice_->data_ + current_slice_offset_;
        current_slice_offset_ += len;
        global_offset_ += len;
        return len;
    }
    return Read(value, len);
}

size_t ByteSliceReader::Seek(size_t offset) {
    if (offset < global_offset_) {
        StorageError(fmt::format("invalid offset value: seek offset = {}, State: list length = {}, offset = {}", offset, GetSize(), global_offset_));
    }

    size_t len = offset - global_offset_;
    if (current_slice_ == nullptr || len == 0) {
        return global_offset_;
    }

    if (current_slice_offset_ + len < GetSliceDataSize(current_slice_)) {
        current_slice_offset_ += len;
        global_offset_ += len;
        return global_offset_;
    } else {
        // current byteslice is not long enough, seek to next byteslices
        int64_t total_len = len;
        int64_t remainLen;
        while (total_len > 0) {
            remainLen = current_slice_->size_ - current_slice_offset_;
            if (remainLen <= total_len) {
                global_offset_ += remainLen;
                total_len -= remainLen;
                current_slice_ = current_slice_->next_;
                current_slice_offset_ = 0;
                if (current_slice_ == nullptr) {
                    if (global_offset_ == GetSize()) {
                        global_offset_ = BYTE_SLICE_EOF;
                    }
                    return BYTE_SLICE_EOF;
                }
            } else {
                global_offset_ += total_len;
                current_slice_offset_ += total_len;
                total_len = 0;
            }
        }
        if (global_offset_ == GetSize()) {
            global_offset_ = BYTE_SLICE_EOF;
            current_slice_ = ByteSlice::GetEmptySlice();
            current_slice_offset_ = 0;
        }

        return global_offset_;
    }
}

} // namespace infinity
