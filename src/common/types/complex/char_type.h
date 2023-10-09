//
// Created by JinHai on 2022/11/14.
//

#pragma once

#include "common/utility/infinity_assert.h"

#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

struct CharType {
public:
    ptr_t ptr = nullptr;

public:
    inline explicit CharType(ptr_t &&from_ptr) : ptr(from_ptr) { from_ptr = nullptr; }

    inline explicit CharType(SizeT limit_size) {
        ptr = new char_t[limit_size]{0};
        GlobalResourceUsage::IncrRawMemCount();
    }

    inline ~CharType() {
        if (ptr != nullptr) {
            LOG_TRACE("Char type isn't null, need to manually SetNull or Reset");
            Reset();
        }
    }

    inline CharType(const CharType &other) = default;

    inline CharType(CharType &&other) noexcept {
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }

    CharType &operator=(const CharType &other) {
        if (this == &other)
            return *this;
        if (ptr != nullptr) {
            LOG_TRACE("Target Char isn't null, need to manually SetNull or Reset");
            Reset();
        }
        ptr = other.ptr;
        return *this;
    }

    CharType &operator=(CharType &&other) noexcept {
        if (this == &other)
            return *this;
        if (ptr != nullptr) {
            LOG_TRACE("Target char type isn't null, need to manually SetNull or Reset");
            Reset();
        }
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    bool operator==(const CharType &other) const { return strcmp(this->ptr, other.ptr); }

    inline void Initialize(const String &input, SizeT limit) {
        StorageAssert(input.size() < limit,
                      "Attempt to store size: " + std::to_string(input.size() + 1) + " string into CharT with capacity: " + std::to_string(limit));
        memcpy(this->ptr, input.c_str(), input.size());
    }

    inline void Reset() {
        if (ptr != nullptr) {
            delete[] ptr;
            ptr = nullptr;
            GlobalResourceUsage::DecrRawMemCount();
        }
    }

    inline void SetNull() { ptr = nullptr; }

    [[nodiscard]] inline SizeT size() const { StorageAssert(this->ptr != nullptr, "Char type isn't initialized.") return std::strlen(ptr) + 1; }

    [[nodiscard]] inline ptr_t GetDataPtr() const { return this->ptr; }

    [[nodiscard]] inline SizeT Length() const { return std::strlen(ptr); }

    [[nodiscard]] String ToString() const { return {ptr, std::strlen(ptr)}; }
};

} // namespace infinity
