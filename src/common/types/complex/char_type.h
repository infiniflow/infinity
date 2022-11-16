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

    inline explicit
    Char1Type(const char* ptr) {
        // Assume the ptr is pointing to valid memory address
        value = ptr[0];
    }

    ~Char1Type() = default;

    inline
    Char1Type(const Char1Type& other) = default;

    inline
    Char1Type(Char1Type&& other) noexcept = default;

    inline
    Char1Type& operator=(const Char1Type& other) = default;

    inline
    Char1Type& operator=(Char1Type&& other) noexcept = default;

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

    explicit
    Char2Type(const char* ptr);

    ~Char2Type() = default;
    Char2Type(const Char2Type& other);
    Char2Type(Char2Type&& other) noexcept;
    Char2Type& operator=(const Char2Type& other);
    Char2Type& operator=(Char2Type&& other) noexcept;

    void
    Initialize(const char* ptr);

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

    explicit
    Char4Type(const char* ptr);

    Char4Type(const Char4Type& other);
    Char4Type(Char4Type&& other) noexcept;
    Char4Type& operator=(const Char4Type& other);
    Char4Type& operator=(Char4Type&& other) noexcept;

    void
    Initialize(const char* ptr);

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

    explicit
    Char8Type(const char* ptr);

    Char8Type(const Char8Type& other);
    Char8Type(Char8Type&& other) noexcept;
    Char8Type& operator=(const Char8Type& other);
    Char8Type& operator=(Char8Type&& other) noexcept;

    void
    Initialize(const char* ptr);

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

struct Char15Type {
    static constexpr size_t CHAR_LENGTH = 15;
public:
    Char15Type() = default;
    ~Char15Type() = default;

    explicit inline
    Char15Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    explicit inline
    Char15Type(const char* ptr) {
        // we assume the string length won't exceed the limitation;
        Initialize(ptr, strlen(ptr));
    }

    Char15Type(const Char15Type& other);
    Char15Type(Char15Type&& other) noexcept;
    Char15Type& operator=(const Char15Type& other);
    Char15Type& operator=(Char15Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return length;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, length);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    i8     length{0};
    char_t value[CHAR_LENGTH]{};
};

struct Char31Type {
    static constexpr size_t CHAR_LENGTH = 31;
public:
    Char31Type() = default;
    ~Char31Type() = default;

    explicit inline
    Char31Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    explicit inline
    Char31Type(const char* ptr) {
        // we assume the string length won't exceed the limitation;
        Initialize(ptr, strlen(ptr));
    }

    Char31Type(const Char31Type& other);
    Char31Type(Char31Type&& other) noexcept;
    Char31Type& operator=(const Char31Type& other);
    Char31Type& operator=(Char31Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return length;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, length);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    i8     length{0};
    char_t value[CHAR_LENGTH]{};
};

struct Char63Type {
    static constexpr size_t CHAR_LENGTH = 63;
public:
    Char63Type() = default;
    ~Char63Type() = default;

    explicit inline
    Char63Type(const String& str) {
        // we assume the string length won't exceed the limitation;
        Initialize(str.c_str(), str.length());
    }

    explicit inline
    Char63Type(const char* ptr) {
        // we assume the string length won't exceed the limitation;
        Initialize(ptr, strlen(ptr));
    }

    Char63Type(const Char63Type& other);
    Char63Type(Char63Type&& other) noexcept;
    Char63Type& operator=(const Char63Type& other);
    Char63Type& operator=(Char63Type&& other) noexcept;

    void
    Initialize(const char* ptr, size_t length);

    [[nodiscard]] size_t
    Length() const {
        return length;
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(value, length);
    }

    inline void
    Reset() {
        memset(value, 0, CHAR_LENGTH);
    }

public:
    i8     length{0};
    char_t value[CHAR_LENGTH]{};
};

}
