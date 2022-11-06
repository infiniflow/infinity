//
// Created by JinHai on 2022/11/4.
//

#include "common/types/internal_types.h"

namespace infinity {

enum class MixedValueType : i8 {
    // Not nested data type
    kInteger,
    kFloat,
    kLongStr,
    kShortStr,
    kObject,
    kArray,
    kNull,
    kMissing,

    // Nested in array data type
    kNestedMissing,
    kNestedInteger,
    kNestedFloat,
    kNestedLongStr,
    kNestedShortStr,
    kNestedObject,
    kNestedArray,
    kNestedNull,

    kDummy,
};

struct BaseMixedType {
    explicit
    BaseMixedType(MixedValueType value_type): type(value_type) {}

    MixedValueType type;
};

// Not nested data type
struct __attribute__((packed)) MixedType : public BaseMixedType {
    MixedType() : BaseMixedType(MixedValueType::kDummy) {}

    char_t ptr[15] {};
};


// Not nested data type
struct __attribute__((packed)) IntegerMixedType : public BaseMixedType {
    IntegerMixedType() : BaseMixedType(MixedValueType::kInteger) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 value {0};
};

struct __attribute__((packed)) FloatMixedType : public BaseMixedType {
    FloatMixedType() : BaseMixedType(MixedValueType::kFloat) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    f64 value {0};
};

struct __attribute__((packed)) LongStrMixedType : public BaseMixedType {
    LongStrMixedType() : BaseMixedType(MixedValueType::kLongStr) {}

    char_t header[5] {}; // 5 bytes is used to store the header of the string to shortcut some computation.
    i16 length {0};  // Max string length will be 65535
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) ShortStrMixedType : public BaseMixedType {
    ShortStrMixedType() : BaseMixedType(MixedValueType::kShortStr) {}

    char_t ptr[15] {};  // not more than 15 bytes string will use short string type
};

struct __attribute__((packed)) ObjectMixedType : public BaseMixedType {
    ObjectMixedType() : BaseMixedType(MixedValueType::kObject) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 _dummy4{};
};

struct __attribute__((packed)) ArrayMixedType : public BaseMixedType {
    ArrayMixedType() : BaseMixedType(MixedValueType::kArray) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 count {0};
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) NullMixedType : public BaseMixedType {
    NullMixedType() : BaseMixedType(MixedValueType::kNull) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 _dummy4{};
};

struct __attribute__((packed)) MissingMixedType : public BaseMixedType {
    MissingMixedType() : BaseMixedType(MixedValueType::kMissing) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i32 _dummy3{};
    i64 _dummy4{};
};

// Nested in array data type
struct __attribute__((packed)) NestedIntegerMixedType : public BaseMixedType {
    NestedIntegerMixedType() : BaseMixedType(MixedValueType::kNestedInteger) {}
    i16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 value {0};
};

struct __attribute__((packed)) NestedFloatMixedType : public BaseMixedType {
    NestedFloatMixedType() : BaseMixedType(MixedValueType::kNestedFloat) {}
    i16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    f64 value {0};
};

struct __attribute__((packed)) NestedLongStrMixedType : public BaseMixedType {
    NestedLongStrMixedType() : BaseMixedType(MixedValueType::kNestedLongStr) {}
    i16 array_index{};

    char_t header[3] {}; // 3 bytes is used to store the header of the string to shortcut some computation.
    i16 length {0};   // Max string length will be 65535
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) NestedShortStrMixedType : public BaseMixedType {
    NestedShortStrMixedType() : BaseMixedType(MixedValueType::kNestedShortStr) {}
    i16 array_index{};

    char_t ptr[13] {}; // not more than 13 bytes string will use short string type
};

struct __attribute__((packed)) NestedObjectMixedType : public BaseMixedType {
    NestedObjectMixedType() : BaseMixedType(MixedValueType::kNestedObject) {}
    i16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

struct __attribute__((packed)) NestedArrayMixedType : public BaseMixedType {
    NestedArrayMixedType() : BaseMixedType(MixedValueType::kNestedArray) {}
    i16 array_index{};

    i8 _dummy{};
    i32 count {0};
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) NestedNullMixedType : public BaseMixedType {
    NestedNullMixedType() : BaseMixedType(MixedValueType::kNestedNull) {}
    i16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

struct __attribute__((packed)) NestedMissingMixedType : public BaseMixedType {
    NestedMissingMixedType() : BaseMixedType(MixedValueType::kNestedMissing) {}
    i16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

}
