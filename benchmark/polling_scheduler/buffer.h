//
// Created by jinhai on 23-5-11.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

class Buffer {
public:
    explicit
    Buffer(SizeT size) : size_(size) {
        buffer_ = MakeUnique<char>(size);
    }

    inline void
    Append(const char* str) const {
        SizeT len = std::strlen(str);
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
    UniquePtr<char> buffer_{nullptr};
    SizeT size_{};
    SizeT offset_{};
};

}
