//
// Created by JinHai on 2022/11/14.
//

#include "char_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// Char2Type

Char2Type::Char2Type(const String& str) {
    Initialize(str.c_str());
}

Char2Type::Char2Type(const char* ptr) {
    Initialize(ptr);
}

Char2Type::Char2Type(const Char2Type& other) {
    value[0] = other.value[0];
    value[1] = other.value[1];
}

Char2Type::Char2Type(Char2Type&& other) noexcept {
    value[0] = other.value[0];
    value[1] = other.value[1];
}

Char2Type& Char2Type::operator=(const Char2Type& other) {
    if(this == &other) return *this;

    this->value[0] = other.value[0];
    this->value[1] = other.value[1];

    return *this;
}

Char2Type&
Char2Type::operator=(Char2Type&& other) noexcept {
    if(this == &other) return *this;

    this->value[0] = std::move(other.value[0]);
    this->value[1] = std::move(other.value[1]);

    return *this;
}

void
Char2Type::Initialize(const char* ptr) {
    // Assume the str size is OK;
    value[0] = ptr[0];
    value[1] = ptr[1];
}

// Char4Type
Char4Type::Char4Type(const String& str) {
    Initialize(str.c_str());
}

Char4Type::Char4Type(const char* ptr) {
    Initialize(ptr);
}

Char4Type::Char4Type(const Char4Type& other) {
    value[0] = other.value[0];
    value[1] = other.value[1];
    value[2] = other.value[2];
    value[3] = other.value[3];
}

Char4Type::Char4Type(Char4Type&& other) noexcept {
    value[0] = other.value[0];
    value[1] = other.value[1];
    value[2] = other.value[2];
    value[3] = other.value[3];
}

Char4Type&
Char4Type::operator=(const Char4Type& other) {
    if(this == &other) return *this;

    value[0] = other.value[0];
    value[1] = other.value[1];
    value[2] = other.value[2];
    value[3] = other.value[3];

    return *this;
}

Char4Type& Char4Type::operator=(Char4Type&& other) noexcept {
    if(this == &other) return *this;

    value[0] = std::move(other.value[0]);
    value[1] = std::move(other.value[1]);
    value[2] = std::move(other.value[2]);
    value[3] = std::move(other.value[3]);

    return *this;
}

void
Char4Type::Initialize(const char* ptr) {
    // Assume the str size is OK;
    value[0] = ptr[0];
    value[1] = ptr[1];
    value[2] = ptr[2];
    value[3] = ptr[3];
}

// Char8Type
Char8Type::Char8Type(const String& str) {
    Initialize(str.c_str());
}

Char8Type::Char8Type(const char* ptr) {
    Initialize(ptr);
}

Char8Type::Char8Type(const Char8Type& other) {
    value[0] = other.value[0];
    value[1] = other.value[1];
    value[2] = other.value[2];
    value[3] = other.value[3];

    value[4] = other.value[4];
    value[5] = other.value[5];
    value[6] = other.value[6];
    value[7] = other.value[7];

}
Char8Type::Char8Type(Char8Type&& other) noexcept {
    value[0] = std::move(other.value[0]);
    value[1] = std::move(other.value[1]);
    value[2] = std::move(other.value[2]);
    value[3] = std::move(other.value[3]);

    value[4] = std::move(other.value[4]);
    value[5] = std::move(other.value[5]);
    value[6] = std::move(other.value[6]);
    value[7] = std::move(other.value[7]);
}

Char8Type&
Char8Type::operator=(const Char8Type& other) {
    if(this == &other) return *this;

    value[0] = other.value[0];
    value[1] = other.value[1];
    value[2] = other.value[2];
    value[3] = other.value[3];

    value[4] = other.value[4];
    value[5] = other.value[5];
    value[6] = other.value[6];
    value[7] = other.value[7];

    return *this;
}

Char8Type&
Char8Type::operator=(Char8Type&& other) noexcept {
    if(this == &other) return *this;

    value[0] = std::move(other.value[0]);
    value[1] = std::move(other.value[1]);
    value[2] = std::move(other.value[2]);
    value[3] = std::move(other.value[3]);

    value[4] = std::move(other.value[4]);
    value[5] = std::move(other.value[5]);
    value[6] = std::move(other.value[6]);
    value[7] = std::move(other.value[7]);

    return *this;
}

void
Char8Type::Initialize(const char* ptr) {
    // Assume the str size is OK;
    value[0] = ptr[0];
    value[1] = ptr[1];
    value[2] = ptr[2];
    value[3] = ptr[3];
    value[4] = ptr[4];
    value[5] = ptr[5];
    value[6] = ptr[6];
    value[7] = ptr[7];
}

// Char16Type
Char16Type::Char16Type(const Char16Type& other) {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char16Type::Char16Type(Char16Type&& other) noexcept {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char16Type&
Char16Type::operator=(const Char16Type& other) {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}
Char16Type&
Char16Type::operator=(Char16Type&& other) noexcept {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}

void
Char16Type::Initialize(const char* ptr, size_t len) {
//    TypeAssert(len <= CHAR16LENGTH,
//               "Attempt to insert too large string into char16 type variable: " + std::to_string(len));
    size_t length = (len <= CHAR_LENGTH) ? len : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

// Char32Type
Char32Type::Char32Type(const Char32Type& other) {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char32Type::Char32Type(Char32Type&& other) noexcept {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char32Type&
Char32Type::operator=(const Char32Type& other) {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}

Char32Type&
Char32Type::operator=(Char32Type&& other) noexcept {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}

void
Char32Type::Initialize(const char* ptr, size_t len) {
    // Assume the var won't overflow
    size_t length = (len <= CHAR_LENGTH) ? static_cast<size_t>(len) : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

// Char64Type
Char64Type::Char64Type(const Char64Type& other) {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char64Type::Char64Type(Char64Type&& other) noexcept {
    memcpy(value, other.value, CHAR_LENGTH);
}

Char64Type&
Char64Type::operator=(const Char64Type& other) {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}

Char64Type&
Char64Type::operator=(Char64Type&& other) noexcept {
    if(this == &other) return *this;
    memcpy(value, other.value, CHAR_LENGTH);
    return *this;
}

void
Char64Type::Initialize(const char* ptr, size_t len) {
    // Assume the var won't overflow
    size_t length = (len <= CHAR_LENGTH) ? static_cast<size_t>(len) : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

}
