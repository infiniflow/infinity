module;

module byte_slice_reader;

import stl;
import byte_slice;
import status;
import infinity_exception;
import logger;

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
        String error_message = "Read past EOF";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ByteSliceReader::Open(ByteSlice *slice) {
    Close();
    current_slice_ = slice;
    global_offset_ = 0;
    current_slice_offset_ = 0;

    if (current_slice_ == nullptr) {
        String error_message = "Read past EOF";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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

SizeT ByteSliceReader::Read(void *value, SizeT len) {
    if (current_slice_ == nullptr || len == 0) {
        return 0;
    }

    if (current_slice_offset_ + len <= GetSliceDataSize(current_slice_)) {
        std::memcpy(value, current_slice_->data_ + current_slice_offset_, len);
        current_slice_offset_ += len;
        global_offset_ += len;
        return len;
    }
    // current byteslice is not long enough, read next byteslices
    char *dest = (char *)value;
    i64 total_len = (i64)len;
    SizeT offset = current_slice_offset_;
    i64 leftLen = 0;
    while (total_len > 0) {
        leftLen = GetSliceDataSize(current_slice_) - offset;
        if (leftLen < total_len) {
            std::memcpy(dest, current_slice_->data_ + offset, leftLen);
            total_len -= leftLen;
            dest += leftLen;

            offset = 0;
            current_slice_ = NextSlice(current_slice_);
            if (!current_slice_) {
                break;
            }
        } else {
            std::memcpy(dest, current_slice_->data_ + offset, total_len);
            dest += total_len;
            offset += (SizeT)total_len;
            total_len = 0;
        }
    }

    current_slice_offset_ = offset;
    SizeT read_len = (SizeT)(len - total_len);
    global_offset_ += read_len;
    return read_len;
}

SizeT ByteSliceReader::ReadMayCopy(void *&value, SizeT len) {
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

SizeT ByteSliceReader::Seek(SizeT offset) {
    if (offset < global_offset_) {
        // fmt::format("invalid offset value: seek offset = {}, State: list length = {}, offset = {}", offset, GetSize(), global_offset_));
        String error_message = "Invalid offset value";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    SizeT len = offset - global_offset_;
    if (current_slice_ == nullptr || len == 0) {
        return global_offset_;
    }

    if (current_slice_offset_ + len < GetSliceDataSize(current_slice_)) {
        current_slice_offset_ += len;
        global_offset_ += len;
        return global_offset_;
    } else {
        // current byteslice is not long enough, seek to next byteslices
        i64 total_len = len;
        i64 remainLen;
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
