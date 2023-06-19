#include "bytesref.h"

#include <string.h>

namespace infinity {

std::vector<uint8_t> BytesRef::EMPTY_BYTES_ = std::vector<uint8_t>(0);

BytesRef::BytesRef() : BytesRef((EMPTY_BYTES_)) {}

BytesRef::BytesRef(std::vector<uint8_t> &bytes, int offset, int length) {
    bytes_ = bytes;
    offset_ = offset;
    length_ = length;
}

BytesRef::BytesRef(std::vector<uint8_t> &bytes) : BytesRef(bytes, 0, bytes.size()) {
}

BytesRef::BytesRef(int capacity) {
    bytes_ = std::vector<uint8_t>(capacity);
}

bool BytesRef::IsValid() {
    if (bytes_.empty()) {
        return false;
    }
    if (length_ < 0) {
        return false;
    }
    if (length_ > bytes_.size()) {
        return false;
    }
    if (offset_ < 0) {
        return false;
    }
    if (offset_ > bytes_.size()) {
        return false;
    }
    if (offset_ + length_ < 0) {
        return false;
    }
    if (offset_ + length_ > bytes_.size()) {
        return false;
    }
    return true;
}

}