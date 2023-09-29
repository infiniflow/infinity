//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/logger.h"

namespace infinity {

struct VarcharType {
public:
    static constexpr size_t PREFIX_LENGTH = 6;
    static constexpr size_t INLINE_LENGTH = 14;
    static constexpr size_t LEN_LIMIT = 65536;
public:
    VarcharType() {
        length = 0;
        ptr = nullptr;
    }

    explicit
    VarcharType(const String& str);

    explicit
    VarcharType(const char* ptr);

    explicit
    VarcharType(const char* ptr, size_t len);

    ~VarcharType();

    VarcharType(const VarcharType& other);

    VarcharType(VarcharType&& other) noexcept;

    VarcharType&
    operator=(const VarcharType& other);

    VarcharType&
    operator=(VarcharType&& other) noexcept;

public:
    bool
    operator==(const VarcharType& other) const;

    bool
    operator>=(const VarcharType& other) const;

    bool
    operator>(const VarcharType& other) const;

    bool
    operator<=(const VarcharType& other) const;

    bool
    operator<(const VarcharType& other) const;

    [[nodiscard]] inline ptr_t
    GetDataPtr() const {
        if(IsInlined()) {
            return (ptr_t)(prefix);
        } else {
            return this->ptr;
        }
    }

    inline SizeT
    GetDataLen() const {
        return this->length;
    }

public:
    void
    DeepCopy(const VarcharType& other);

    void
    Initialize(const String& str);

    void
    Initialize(const char* ptr);

    void
    Initialize(const char* ptr, size_t len);

    void
    InitializeAsEmptyStr();

    [[nodiscard]] inline bool
    IsInlined() const {
        return length <= INLINE_LENGTH;
    }

    void
    Reset(bool clean_memory = true);

    [[nodiscard]] String
    ToString() const;

    u16 length{0};  // 65535 will be the limitation.
    char_t prefix[PREFIX_LENGTH]{}; // prefix of the varchar
    // If length <= 14, ptr will be used as prefix.
    ptr_t ptr{nullptr};   // pointer to the varchar value buffer.
};

}

