#pragma once


#include "common/types/internal_types.h"
#include "indexstore/codec.h"

#include <leveldb/slice.h>
#include <leveldb/status.h>

#include <string>

namespace infinity {

using Slice = leveldb::Slice;
using Status = leveldb::Status;

class BlockPointer {
public:
    BlockPointer() {}
    BlockPointer(const BlockPointer &from) :
        offset_(from.offset_),
        size_(from.size_) {}

    BlockPointer(u64 offset, u64 size) :
        offset_(offset),
        size_(size) {}

    template<class StrType>
    void EncodeTo(StrType& s) const {
        Codec::AddVLong(s, offset_);
        Codec::AddVInt(s, size_);
    }

    Status DecodeFrom(const u8 *data, const u8 *limit) {
        data = Codec::GetVLongPtr(data, &offset_);
        if (!data) {
            return Status::Corruption("bad block pointer");
        }

        data = Codec::GetVIntPtr(data, &size_);
        if (!data) {
            return Status::Corruption("bad block pointer");
        }

        return Status::OK();
    }

    u64 Offset() const {
        return offset_;
    }

    u32 Size() const {
        return size_;
    }

    bool Equals(const BlockPointer& other) const {
        return offset_ == other.offset_ && size_ == other.size_;
    }

private:
    u64 offset_;
    u32 size_;
};

}
