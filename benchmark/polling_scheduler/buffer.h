//
// Created by jinhai on 23-5-11.
//

#pragma once

#include <cstring>
#include <memory>

namespace infinity {

class Buffer {
public:
    explicit
    Buffer(size_t size) : size_(size) {
        buffer_ = std::make_unique<char>(size);
    }

    inline void
    Append(const char* str) const {
        size_t len = std::strlen(str);
        if(len + offset_ >= size_) {
            throw;
        }
        memcpy(buffer_.get() + offset_, str, len);
    }

    inline char*
    Get() const {
        return buffer_.get();
    }

private:
    std::unique_ptr<char> buffer_{nullptr};
    size_t size_{};
    size_t offset_{};
};

}
