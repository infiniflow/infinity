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
    kTuple,
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

class MixedType;
class IntegerMixedType;
class FloatMixedType;
class LongStrMixedType;
class ShortStrMixedType;
class TupleMixedType;
class ArrayMixedType;
class NullMixedType;
class MissingMixedType;

class NestedIntegerMixedType;
class NestedFloatMixedType;
class NestedLongStrMixedType;
class NestedShortStrMixedType;
class NestedTupleMixedType;
class NestedArrayMixedType;
class NestedNullMixedType;
class NestedMissingMixedType;


struct BaseMixedType {
    BaseMixedType() = default;

    explicit
    BaseMixedType(MixedValueType value_type): type(value_type) {}

    MixedValueType type;
};

// Not nested data type
struct __attribute__((packed)) MixedType : public BaseMixedType {

    // non-static member variable
public:
    char_t ptr[15];

    // static member variable
public:
    static constexpr i64 ELEMENT_SIZE = 16;
    static constexpr i64 SHORT_STR_LIMIT = 15;
    static constexpr i64 SHORT_NESTED_STR_LIMIT = 13;
    static constexpr i64 LONG_STR_HEADER = 5;
    static constexpr i64 LONG_NESTED_STR_HEADER = 3;

    // static member method for non-nested data
public:
    static MixedType
    MakeInteger(i64 value);

    static MixedType
    MakeFloat(f64 value);

    static MixedType
    MakeString(const String& str);

    static MixedType
    MakeObject();

    static MixedType
    MakeArray(ptr_t ptr, i32 count);

    static MixedType
    MakeNull();

    static MixedType
    MakeMissing();

    // static member method for nested data
public:
    static void
    InsertNestedInteger(i64 value, ptr_t position, u16 index);

    static void
    InsertNestedFloat(f64 value, ptr_t position, u16 index);

    static void
    InsertNestedString(const String& value, ptr_t position, u16 index);

    // input should have map: key/value, most 65536 kv pair in one object.
    static void
    MakeNestedObject(u16 count, ptr_t position, u16 index);

    static void
    InsertIntoNestedObject(const String& key, MixedType value, ptr_t position, u16 index);

    static void
    InsertNestedArray(u64 count, ptr_t position, u16 index);

    static void
    InsertNestedNull(ptr_t position, u16 index);

    // non-static member
public:
    MixedType() = default;

    explicit inline
    MixedType(MixedValueType data_type) : BaseMixedType(data_type) {
    }

    inline
    ~MixedType() {
    }

    MixedType(const MixedType& other);
    MixedType(MixedType&& other) noexcept;
    MixedType& operator=(const MixedType& other);
    MixedType& operator=(MixedType&& other) noexcept;

    void
    Reset();
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

    char_t header[MixedType::LONG_STR_HEADER] {}; // 5 bytes is used to store the header of the string to shortcut some computation.
    u16 length {0};  // Max string length will be 65535
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) ShortStrMixedType : public BaseMixedType {
    ShortStrMixedType() : BaseMixedType(MixedValueType::kShortStr) {}

    char_t ptr[MixedType::SHORT_STR_LIMIT] {};  // not more than 15 bytes string will use short string type
};

struct __attribute__((packed)) TupleMixedType : public BaseMixedType {
    TupleMixedType() : BaseMixedType(MixedValueType::kTuple) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i16 _dummy3{};
    u16 count {0};        // count of the tuple, each tuple will have two MixedType member, one for key, another for value.
    ptr_t ptr {nullptr};  // pointer to mixed type of tuple
};

struct __attribute__((packed)) ArrayMixedType : public BaseMixedType {
    ArrayMixedType() : BaseMixedType(MixedValueType::kArray) {}

    i8 _dummy1{};
    i16 _dummy2{};
    i16 _dummy3{};
    u16 count {0};        // Array count, which limit is 65535;
    ptr_t ptr {nullptr};  // pointer to child mixed type
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
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 value {0};
};

struct __attribute__((packed)) NestedFloatMixedType : public BaseMixedType {
    NestedFloatMixedType() : BaseMixedType(MixedValueType::kNestedFloat) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    f64 value {0};
};

struct __attribute__((packed)) NestedLongStrMixedType : public BaseMixedType {
    NestedLongStrMixedType() : BaseMixedType(MixedValueType::kNestedLongStr) {}
    u16 array_index{};

    // 3 bytes is used to store the header of the string to shortcut some computation.
    char_t header[MixedType::LONG_NESTED_STR_HEADER] {};

    u16 length {0};   // Max string length will be 65535
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) NestedShortStrMixedType : public BaseMixedType {
    NestedShortStrMixedType() : BaseMixedType(MixedValueType::kNestedShortStr) {}
    u16 array_index{};

    // not more than 13 bytes string will use short string type
    char_t ptr[MixedType::SHORT_NESTED_STR_LIMIT] {};
};

struct __attribute__((packed)) NestedTupleMixedType : public BaseMixedType {
    NestedTupleMixedType() : BaseMixedType(MixedValueType::kNestedObject) {}
    i8 _dummy1{};
    u16 array_index{};
    i16 _dummy2{};
    u16 count{0};
    ptr_t ptr{nullptr};
};

struct __attribute__((packed)) NestedArrayMixedType : public BaseMixedType {
    NestedArrayMixedType() : BaseMixedType(MixedValueType::kNestedArray) {}
    i8 _dummy{};
    u16 array_index{};
    i16 _dummy1{};
    u16 count {0};
    ptr_t ptr {nullptr};
};

struct __attribute__((packed)) NestedNullMixedType : public BaseMixedType {
    NestedNullMixedType() : BaseMixedType(MixedValueType::kNestedNull) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

struct __attribute__((packed)) NestedMissingMixedType : public BaseMixedType {
    NestedMissingMixedType() : BaseMixedType(MixedValueType::kNestedMissing) {}
    u16 array_index{};

    i8 _dummy1{};
    i32 _dummy2{};
    i64 _dummy3{};
};

struct MixedArray {
public:
    ptr_t parent_ptr;
    MixedType array[0];
private:
    static ptr_t
    Make(i32 count);

    static void
    Set(i32 index, const MixedType& mixed_value);

    static MixedType&
    Get(i32 index);
};

struct NestedObject {
public:
    ptr_t parent_ptr;
    MixedType array[0];
private:
    static ptr_t
    Make(i32 count);

    static void
    Set(const String& key, const MixedType& mixed_value);

    static MixedType&
    Get(const String& key);
};

}
