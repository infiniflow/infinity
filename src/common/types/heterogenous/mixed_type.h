//
// Created by JinHai on 2022/11/4.
//
#pragma once

#include "common/types/internal_types.h"
#include "mixed_value_type.h"
#include "base_mixed_type.h"
#include "integer_mixed_type.h"
#include "float_mixed_type.h"
#include "long_string_mixed_type.h"
#include "short_string_mixed_type.h"
#include "tuple_mixed_type.h"
#include "array_mixed_type.h"
#include "null_mixed_type.h"
#include "missing_mixed_type.h"
#include "nested_integer_mixed_type.h"
#include "nested_float_mixed_type.h"
#include "nested_long_string_mixed_type.h"
#include "nested_short_string_mixed_type.h"
#include "nested_tuple_mixed_type.h"
#include "nested_array_mixed_type.h"
#include "nested_null_mixed_type.h"
#include "nested_missing_mixed_type.h"

namespace infinity {

// Not nested data type
struct __attribute__((packed)) MixedType : public BaseMixedType {

    // non-static member variable
public:
    char_t ptr[ELEMENT_SIZE - 1] {};
public:
    static MixedType
    MakeInteger(i64 value);

    static MixedType
    MakeFloat(f64 value);

    static MixedType
    MakeString(const String& str);

    static MixedType
    MakeTuple(u16 count);

    static MixedType
    MakeArray(u16 count);

    static MixedType
    MakeNull();

    static MixedType
    MakeMissing();

    // Tuple related method
public:
    void
    CopyIntoTuple(const String& key, const MixedType& value);

    void
    MoveIntoTuple(const String& key, MixedType&& value);

    MixedType*
    GetFromTuple(const String& key);

    // Array related method
public:
    void
    CopyIntoArray(const MixedType& value, u16 index);

    void
    MoveIntoArray(MixedType&& value, u16 index);

    void
    InsertIntegerIntoArray(i64 value, u16 index);

    void
    InsertFloatIntoArray(f64 value, u16 index);

    void
    InsertStringIntoArray(const String& value, u16 index);

    void
    InsertNullIntoArray(u16 index);
    // static member method for array data
public:
    static void
    InsertNestedInteger(i64 value, ptr_t position, u16 index);

    static void
    InsertNestedFloat(f64 value, ptr_t position, u16 index);

    static void
    InsertNestedString(const String& value, ptr_t position, u16 index);

    // input should have map: key/value, most 65536 kv pair in one object.
    static void
    MakeNestedTuple(u16 count, ptr_t position, u16 index);

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
        Reset();
    }

    MixedType(const MixedType& other);
    MixedType(MixedType&& other) noexcept;
    MixedType& operator=(const MixedType& other);
    MixedType& operator=(MixedType&& other) noexcept;

    void
    Reset();

private:
    static void
    Copy(const MixedType& from, MixedType& to);

    static void
    Move(MixedType&& from, MixedType& to);
};





}
