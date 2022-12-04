//
// Created by JinHai on 2022/11/14.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/logger.h"

namespace infinity {

struct Char1Type {

public:
    Char1Type() = default;

    inline explicit
    Char1Type(char_t input) : value(input) {}

    inline explicit
    Char1Type(const String& str) {
        // Assume the str size is OK;
        value = str[0];
    }

    ~Char1Type() = default;

    inline
    Char1Type(const Char1Type& other) = default;

    inline
    Char1Type(Char1Type&& other) noexcept = default;

    inline Char1Type&
    operator=(const Char1Type& other) = default;

    inline Char1Type&
    operator=(Char1Type&& other) noexcept = default;

    inline void
    Initialize(char input) {
        value = input;
    }

    [[nodiscard]] inline String
    ToString() const {
        return &value;
    }

public:
    char_t value {};
};

struct Char2Type {
    static constexpr size_t CHAR_LENGTH = 2;
public:
    Char2Type() = default;

    explicit
    Char2Type(const String& str);

    ~Char2Type() = default;
    Char2Type(const Char2Type& other);
    Char2Type(Char2Type&& other) noexcept;
    Char2Type& operator=(const Char2Type& other);
    Char2Type& operator=(Char2Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t len);

    [[nodiscard]] inline String
    ToString() const {
        size_t len = std::min(strlen(value), CHAR_LENGTH);
        return String(value, len);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};
};

struct Char4Type {
    static constexpr size_t CHAR_LENGTH = 4;
public:
    Char4Type() = default;
    ~Char4Type() = default;

    explicit
    Char4Type(const String& str);

    Char4Type(const Char4Type& other);
    Char4Type(Char4Type&& other) noexcept;
    Char4Type& operator=(const Char4Type& other);
    Char4Type& operator=(Char4Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t len);

    [[nodiscard]] inline String
    ToString() const {
        size_t len = std::min(strlen(value), CHAR_LENGTH);
        return String(value, len);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};

};

struct Char8Type {
    static constexpr size_t CHAR_LENGTH = 8;
public:
    Char8Type() = default;
    ~Char8Type() = default;

    explicit
    Char8Type(const String& str);

    Char8Type(const Char8Type& other);
    Char8Type(Char8Type&& other) noexcept;
    Char8Type& operator=(const Char8Type& other);
    Char8Type& operator=(Char8Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t len);

    [[nodiscard]] inline String
    ToString() const {
        size_t len = std::min(strlen(value), CHAR_LENGTH);
        return String(value, len);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};
};

struct Char16Type {
    static constexpr size_t CHAR_LENGTH = 16;
public:
    Char16Type() = default;
    ~Char16Type() = default;

    explicit inline
    Char16Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    Char16Type(const Char16Type& other);
    Char16Type(Char16Type&& other) noexcept;
    Char16Type& operator=(const Char16Type& other);
    Char16Type& operator=(Char16Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return CHAR_LENGTH;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, CHAR_LENGTH);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};
};

struct Char32Type {
    static constexpr size_t CHAR_LENGTH = 32;
public:
    Char32Type() = default;
    ~Char32Type() = default;

    explicit inline
    Char32Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    Char32Type(const Char32Type& other);
    Char32Type(Char32Type&& other) noexcept;
    Char32Type& operator=(const Char32Type& other);
    Char32Type& operator=(Char32Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return CHAR_LENGTH;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, CHAR_LENGTH);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};
};

struct Char64Type {
    static constexpr size_t CHAR_LENGTH = 64;
public:
    Char64Type() = default;
    ~Char64Type() = default;

    explicit inline
    Char64Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    Char64Type(const Char64Type& other);
    Char64Type(Char64Type&& other) noexcept;
    Char64Type& operator=(const Char64Type& other);
    Char64Type& operator=(Char64Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return CHAR_LENGTH;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, CHAR_LENGTH);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    char_t value[CHAR_LENGTH]{};
};

}
