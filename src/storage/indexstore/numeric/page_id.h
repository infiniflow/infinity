#pragma once

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <functional>

namespace infinity {

class PhysicalPageId {
public:
    PhysicalPageId(const size_t file_id, const size_t offset) {
        CheckFileId(file_id);
        CheckOffset(offset);
        size_t temp_value =
            (offset & offset_mask_) | ((file_id << offset_bits_) & segment_mask_);
        CheckValid(temp_value);
        value_ = temp_value;
    }

    PhysicalPageId() {
        value_ = kInvalidValue;
    }

    void SetOffset(const size_t offset) {
        CheckOffset(offset);
        size_t temp_value = (value_ & ~offset_mask_) | (offset & offset_mask_);
        CheckValid(temp_value);
        value_ = temp_value;
    }

    void SetFileId(const size_t file_id) {
        CheckFileId(file_id);
        size_t temp_value =
            (value_ & ~segment_mask_) | ((file_id << offset_bits_) & segment_mask_);
        CheckValid(temp_value);
        value_ = temp_value;
    }

    size_t GetFileId() const {
        return value_ >> offset_bits_;
    }

    size_t GetOffset() const {
        return value_ & offset_mask_;
    }

    bool IsValid() const {
        return value_ != kInvalidValue;
    }

    inline bool operator==(const PhysicalPageId& rhs) const {
        return this->value_ == rhs.value_;
    }
    inline bool operator!=(const PhysicalPageId& rhs) const {
        return !(*this == rhs);
    }

private:
    // A special value used to indicate an "invalid" page_id.
    static const size_t kInvalidValue = ~(0ULL);

    static void CheckValid(const size_t temp_value) {
        if (temp_value == kInvalidValue) {
            throw std::runtime_error(
                "Failed to create PageId: tried to use reserved value.");
        }
    }

    static void CheckFileId(const size_t file_id) {
        if ((file_id & ~(segment_mask_ >> offset_bits_)) != 0) {
            throw std::runtime_error(
                "Failed to create PageId: file_id overflow.");
        }
    }

    static void CheckOffset(const size_t offset) {
        if ((offset & ~offset_mask_) != 0) {
            throw std::runtime_error(
                "Failed to create PageId: offset overflow.");
        }
    }

    // The number of bits of `value_` allocated to each component of a
    // PhysicalPageId.
    static size_t offset_bits_;
    static size_t segment_mask_;
    static size_t offset_mask_;

    // The lowest-order `offset_bits_` store the page offset, while the rest store
    // the file_id.
    size_t value_;

    friend struct std::hash<PhysicalPageId>;
};

}

namespace std {

ostream& operator<<(ostream& os, const infinity::PhysicalPageId& id);

template <>
struct hash<infinity::PhysicalPageId> {
    std::size_t operator()(const infinity::PhysicalPageId& id) const {
        return std::hash<size_t>()(id.value_);
    }
};

}