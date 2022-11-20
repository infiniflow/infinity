//
// Created by JinHai on 2022/11/4.
//

#include "mixed_type.h"
#include "mixed_array_value.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// static member method for non-nested data
MixedType
MixedType::MakeInteger(i64 input) {
    MixedType value(MixedValueType::kInteger);
    IntegerMixedType* integer_mixed_ptr = (IntegerMixedType *)(&value);
    integer_mixed_ptr->value = input;
    return value;
}

MixedType
MixedType::MakeFloat(f64 input) {
    MixedType value(MixedValueType::kFloat);
    FloatMixedType* float_mixed_ptr = (FloatMixedType *)(&value);
    float_mixed_ptr->value = input;
    return value;
}

MixedType
MixedType::MakeString(const String& str) {
    size_t str_len = str.size();
    if(str_len <= MixedType::SHORT_STR_LIMIT) {
        // Short str
        MixedType value(MixedValueType::kShortStr);
        ShortStrMixedType* short_mixed_ptr = (ShortStrMixedType*)(&value);
        memcpy(short_mixed_ptr->ptr, str.c_str(), str_len);
        if(str_len < SHORT_STR_LIMIT) {
            memset(short_mixed_ptr->ptr + str_len, 0, MixedType::SHORT_STR_LIMIT - str_len);
        }
        return value;
    } else {
        // Long str
        MixedType value(MixedValueType::kLongStr);
        LongStrMixedType* long_mixed_str = (LongStrMixedType*)(&value);

        TypeAssert(str.size() <= 65535, "String length exceeds 65535.");
        u16 str_len = str.size();
        long_mixed_str->length = str_len;
        long_mixed_str->ptr = new char_t[str_len];
        memcpy(long_mixed_str->ptr, str.c_str(), str_len);
        // Fill long string prefix
        memcpy(long_mixed_str->header, str.c_str(), LONG_STR_HEADER);

        return value;
    }
}

MixedType
MixedType::MakeObject() {
    MixedType value(MixedValueType::kTuple);
    return value;
}

MixedType
MixedType::MakeArray(ptr_t ptr, i32 count) {
    MixedType value(MixedValueType::kArray);
    ArrayMixedType* mixed_array_ptr = (ArrayMixedType*)(&value);
    mixed_array_ptr->count = count;
    mixed_array_ptr->ptr = ptr;
    return value;
}

MixedType
MixedType::MakeNull() {
    MixedType value(MixedValueType::kNull);
    return value;
}

MixedType
MixedType::MakeMissing() {
    MixedType value(MixedValueType::kMissing);
    return value;
}

// static member method for nested data
void
MixedType::InsertNestedInteger(i64 value, ptr_t position, u16 index) {
    NestedIntegerMixedType* nested_integer_ptr = (NestedIntegerMixedType*)position;
    nested_integer_ptr->value = value;
}

void
MixedType::InsertNestedFloat(f64 value, ptr_t position, u16 index) {
    NestedFloatMixedType* nested_float_ptr = (NestedFloatMixedType*)position;
    nested_float_ptr->value = value;
}

void
MixedType::InsertNestedString(const String& str, ptr_t position, u16 index) {

    size_t str_len = str.size();
    if(str_len <= MixedType::SHORT_NESTED_STR_LIMIT) {
        // Short str
        NestedShortStrMixedType* nested_short_mixed_ptr = (NestedShortStrMixedType*)(position);
        memcpy(nested_short_mixed_ptr->ptr, str.c_str(), str_len);
        if(str_len < SHORT_NESTED_STR_LIMIT) {
            memset(nested_short_mixed_ptr->ptr + str_len, 0, MixedType::SHORT_NESTED_STR_LIMIT - str_len);
        }
    } else {
        // Long str
        NestedLongStrMixedType* nested_long_mixed_str = (NestedLongStrMixedType*)(position);
        TypeAssert(str_len <= 65535, "String length exceeds 65535.");
        nested_long_mixed_str->length = str_len;
        nested_long_mixed_str->ptr = new char_t[str_len];
        memcpy(nested_long_mixed_str->ptr, str.c_str(), str_len);
        // Fill long string prefix
        memcpy(nested_long_mixed_str->header, str.c_str(), LONG_NESTED_STR_HEADER);
    }

}

// input should have map: key/value
void
MixedType::MakeNestedObject(u16 count, ptr_t position, u16 index) {
    NestedTupleMixedType* object_type_ptr = (NestedTupleMixedType*)position;
    object_type_ptr->count = count;

    // TODO this memory need to reallocate to hash map
    object_type_ptr->ptr = new char_t[count * ELEMENT_SIZE * 2] {0};
}

void
MixedType::InsertIntoNestedObject(const String& key, MixedType value, ptr_t position, u16 index) {
    // Insert kv into nested object.

    // Find the hash slot by the key

    // Insert the value into the slot.
}

void
MixedType::InsertNestedArray(u64 count, ptr_t position, u16 index) {
    NestedArrayMixedType* array_type_ptr = (NestedArrayMixedType*)position;
    array_type_ptr->count = count;

    // Allocate count * ELEMENT SIZE for array element, another point size space for the parent ptr.
    array_type_ptr->ptr = new char_t[count * ELEMENT_SIZE + sizeof(ptr_t)] {0};
    MixedArrayValue* nested_array_ptr = (MixedArrayValue*)(array_type_ptr->ptr);

    // Store the parent
    nested_array_ptr->parent_ptr = position;
}

void
MixedType::InsertNestedNull(ptr_t position, u16 index) {

}


// Non-static member method
MixedType::MixedType(const MixedType& other) {
    this->type = other.type;
    switch(this->type) {
        case MixedValueType::kInteger:
        case MixedValueType::kFloat:
        case MixedValueType::kShortStr:
        case MixedValueType::kTuple:
        case MixedValueType::kNull:
        case MixedValueType::kMissing:
        case MixedValueType::kNestedMissing:
        case MixedValueType::kNestedInteger:
        case MixedValueType::kNestedFloat:
        case MixedValueType::kNestedShortStr:
        case MixedValueType::kNestedObject:
        case MixedValueType::kNestedNull:
            break;
        case MixedValueType::kLongStr:
            break;
        case MixedValueType::kArray:
            break;
        case MixedValueType::kNestedLongStr:
            break;
        case MixedValueType::kNestedArray:
            break;
        case MixedValueType::kDummy:
            break;
    }
}

MixedType::MixedType(MixedType&& other) noexcept {

}

MixedType&
MixedType::operator=(const MixedType& other) {

}

MixedType&
MixedType::operator=(MixedType&& other) noexcept {

}

void
MixedType::Reset() {

}

}
