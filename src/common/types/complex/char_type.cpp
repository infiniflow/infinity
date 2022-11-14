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

// Char15Type
Char15Type::Char15Type(const Char15Type& other) {
    length = other.length;
    memcpy(value, other.value, length);
}

Char15Type::Char15Type(Char15Type&& other) noexcept {
    length = std::move(other.length);
    memcpy(value, other.value, length);
}

Char15Type&
Char15Type::operator=(const Char15Type& other) {
    if(this == &other) return *this;
    length = other.length;
    memcpy(value, other.value, length);
    return *this;
}
Char15Type&
Char15Type::operator=(Char15Type&& other) noexcept {
    if(this == &other) return *this;
    length = std::move(other.length);
    memcpy(value, other.value, length);
    return *this;
}

void
Char15Type::Initialize(const char* ptr, size_t len) {
//    TypeAssert(len <= CHAR15LENGTH,
//               "Attempt to insert too large string into char15 type variable: " + std::to_string(len));
    length = (len <= CHAR_LENGTH) ? static_cast<i8>(len) : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

// Char31Type
Char31Type::Char31Type(const Char31Type& other) {
    length = other.length;
    memcpy(value, other.value, length);
}

Char31Type::Char31Type(Char31Type&& other) noexcept {
    length = std::move(other.length);
    memcpy(value, other.value, length);
}

Char31Type&
Char31Type::operator=(const Char31Type& other) {
    if(this == &other) return *this;
    length = other.length;
    memcpy(value, other.value, length);
    return *this;
}

Char31Type&
Char31Type::operator=(Char31Type&& other) noexcept {
    if(this == &other) return *this;
    length = std::move(other.length);
    memcpy(value, other.value, length);
    return *this;
}

void
Char31Type::Initialize(const char* ptr, size_t len) {
    // Assume the var won't overflow
    length = (len <= CHAR_LENGTH) ? static_cast<i8>(len) : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

// Char63Type
Char63Type::Char63Type(const Char63Type& other) {
    length = other.length;
    memcpy(value, other.value, length);
}

Char63Type::Char63Type(Char63Type&& other) noexcept {
    length = std::move(other.length);
    memcpy(value, other.value, length);
}

Char63Type&
Char63Type::operator=(const Char63Type& other) {
    if(this == &other) return *this;
    length = other.length;
    memcpy(value, other.value, length);
    return *this;
}

Char63Type&
Char63Type::operator=(Char63Type&& other) noexcept {
    if(this == &other) return *this;
    length = std::move(other.length);
    memcpy(value, other.value, length);
    return *this;
}

void
Char63Type::Initialize(const char* ptr, size_t len) {
    // Assume the var won't overflow
    length = (len <= CHAR_LENGTH) ? static_cast<i8>(len) : CHAR_LENGTH;
    memcpy(value, ptr, length);
}

}
