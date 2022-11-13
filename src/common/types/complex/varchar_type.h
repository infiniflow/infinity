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
public:
    VarcharType() = default;

    explicit
    VarcharType(const String& str);

    explicit
    VarcharType(const char* ptr);

    explicit
    VarcharType(const char* ptr, size_t len);

    ~VarcharType();
    VarcharType(const VarcharType& other);
    VarcharType(VarcharType&& other) noexcept;
    VarcharType& operator=(const VarcharType& other);
    VarcharType& operator=(VarcharType&& other) noexcept;

    void
    DeepCopy(const VarcharType& other);

    void
    Initialize(const String& str);

    void
    Initialize(const char* ptr);

    void
    Initialize(const char* ptr, size_t len);

    [[nodiscard]] inline bool
    IsInlined() const {
        return length <= INLINE_LENGTH;
    }

    void
    Reset();

    [[nodiscard]] String
    ToString() const;

    i16 length{0};  // 65535 will be the limitation.
    char_t prefix[PREFIX_LENGTH] {}; // prefix of the varchar
    // If length <= 14, ptr will be used as prefix.
    ptr_t ptr {nullptr};   // pointer to the varchar value buffer.
};

struct Char1Type {
    char_t value[1]{};
};

struct Char2Type {
    char_t value[2]{};
};

struct Char4Type {
    char_t value[4]{};
};

struct Char8Type {
    char_t value[8]{};
};

struct Char16Type {
    i8     length{0};
    char_t value[15]{};
};

struct Char32Type {
    i8     length{0};
    char_t value[31]{};
};

struct Char64Type {
    i8     length{0};
    char_t value[63]{};
};

}

