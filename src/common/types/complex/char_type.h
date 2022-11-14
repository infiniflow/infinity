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
    static constexpr size_t CHAR2LENGTH = 2;
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
        return String(value, CHAR2LENGTH);
    }

public:
    char_t value[CHAR2LENGTH]{};
};

struct Char4Type {
    static constexpr size_t CHAR4LENGTH = 4;
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
        return String(value, CHAR4LENGTH);
    }

public:
    char_t value[CHAR4LENGTH]{};

};

struct Char8Type {
    static constexpr size_t CHAR8LENGTH = 8;
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
        return String(value, CHAR8LENGTH);
    }

public:
    char_t value[CHAR8LENGTH]{};
};

struct Char15Type {
    i8     length{0};
    char_t value[15]{};
};

struct Char31Type {
    i8     length{0};
    char_t value[31]{};
};

struct Char63Type {
    i8     length{0};
    char_t value[63]{};
};

}
