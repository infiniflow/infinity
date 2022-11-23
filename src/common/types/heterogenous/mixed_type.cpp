//
// Created by JinHai on 2022/11/4.
//

#include "mixed_type.h"
#include "mixed_array_value.h"
#include "mixed_tuple_value.h"
#include "common/utility/infinity_assert.h"
#include "main/logger.h"

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
    if(str_len <= BaseMixedType::SHORT_STR_LIMIT) {
        // Short str
        MixedType value(MixedValueType::kShortStr);

        // ShortStrMixedType* short_mixed_ptr
        auto* short_mixed_ptr = (ShortStrMixedType*)(&value);
        memcpy(short_mixed_ptr->ptr, str.c_str(), str_len);
        if(str_len < BaseMixedType::SHORT_STR_LIMIT) {
            memset(short_mixed_ptr->ptr + str_len, 0, BaseMixedType::SHORT_STR_LIMIT - str_len);
        }
        short_mixed_ptr->length = static_cast<i8>(str_len);
        return value;
    } else {
        // Long str
        MixedType value(MixedValueType::kLongStr);

        // LongStrMixedType* long_mixed_str
        auto* long_mixed_str = (LongStrMixedType*)(&value);

        TypeAssert(str.size() <= 65535, "String length exceeds 65535.");
        long_mixed_str->length = static_cast<u16>(str_len);
        long_mixed_str->ptr = new char_t[str_len];
        memcpy(long_mixed_str->ptr, str.c_str(), str_len);
        // Fill long string prefix
        memcpy(long_mixed_str->header, str.c_str(), BaseMixedType::LONG_STR_HEADER);

        return value;
    }
}

MixedType
MixedType::MakeTuple(u16 count) {
    MixedType value(MixedValueType::kTuple);
    auto* mixed_tuple_ptr = (TupleMixedType*)(&value);
    mixed_tuple_ptr->count = count;

    // Parent ptr and count * sizeof(mixed tuple value: key + value)
    mixed_tuple_ptr->ptr = new char_t[count * BaseMixedType::TUPLE_SIZE * 2 + sizeof(ptr_t)];
    auto* tuple_value_ptr = (MixedTupleValue*)(mixed_tuple_ptr->ptr);

    // Set the tuple values parent point
    tuple_value_ptr->parent_ptr = mixed_tuple_ptr->ptr;

    return value;
}

MixedType
MixedType::MakeArray(u16 count) {
    MixedType value(MixedValueType::kArray);

    auto* mixed_array_ptr = (ArrayMixedType*)(&value);
    mixed_array_ptr->count = count;

    // Parent ptr and count * sizeof(mixed array value)
    mixed_array_ptr->ptr = new char_t[count * BaseMixedType::ELEMENT_SIZE + sizeof(ptr_t)];
    auto* array_value_ptr = (MixedArrayValue*)(mixed_array_ptr->ptr);

    // Set the array values parent point
    array_value_ptr->parent_ptr = mixed_array_ptr->ptr;
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
    if(str_len <= BaseMixedType::SHORT_NESTED_STR_LIMIT) {
        // Short str
        NestedShortStrMixedType* nested_short_mixed_ptr = (NestedShortStrMixedType*)(position);
        memcpy(nested_short_mixed_ptr->ptr, str.c_str(), str_len);
        if(str_len < BaseMixedType::SHORT_NESTED_STR_LIMIT) {
            memset(nested_short_mixed_ptr->ptr + str_len, 0, BaseMixedType::SHORT_NESTED_STR_LIMIT - str_len);
        }
    } else {
        // Long str
        NestedLongStrMixedType* nested_long_mixed_str = (NestedLongStrMixedType*)(position);
        TypeAssert(str_len <= 65535, "String length exceeds 65535.");
        nested_long_mixed_str->length = str_len;
        nested_long_mixed_str->ptr = new char_t[str_len];
        memcpy(nested_long_mixed_str->ptr, str.c_str(), str_len);
        // Fill long string prefix
        memcpy(nested_long_mixed_str->header, str.c_str(), BaseMixedType::LONG_NESTED_STR_HEADER);
    }

}

// input should have map: key/value
void
MixedType::MakeNestedTuple(u16 count, ptr_t position, u16 index) {
    NestedTupleMixedType* object_type_ptr = (NestedTupleMixedType*)position;
    object_type_ptr->count = count;

    // TODO this memory need to reallocate to hash map
    object_type_ptr->ptr = new char_t[count * BaseMixedType::TUPLE_SIZE + sizeof(ptr_t)] {0};
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
    array_type_ptr->ptr = new char_t[count * BaseMixedType::ELEMENT_SIZE + sizeof(ptr_t)] {0};
    MixedArrayValue* nested_array_ptr = (MixedArrayValue*)(array_type_ptr->ptr);

    // Store the parent
    nested_array_ptr->parent_ptr = position;
}

void
MixedType::InsertNestedNull(ptr_t position, u16 index) {

}

// Non-static member method
MixedType::MixedType(const MixedType& other) {
    Copy(other, *this);
}

MixedType::MixedType(MixedType&& other) noexcept {
    Move(std::forward<MixedType>(other), *this);
}

MixedType&
MixedType::operator=(const MixedType& other) {
    if(this == &other) return *this;
    Copy(other, *this);
    return *this;
}

MixedType&
MixedType::operator=(MixedType&& other) noexcept {
    if(this == &other) return *this;
    Move(std::forward<MixedType>(other), *this);
    return *this;
}

void
MixedType::Move(MixedType&& from, MixedType& to) {
    to.Reset();
    memcpy((void*)&to, (void*)&from, BaseMixedType::ELEMENT_SIZE);
    memset((void*)&from, 0, BaseMixedType::ELEMENT_SIZE);
    LOG_TRACE("MixedType::Move, {}", BaseMixedType::GetTypeName(to.type));
}

void
MixedType::Copy(const MixedType& from, MixedType& to) {
    to.Reset();
    memcpy((void*)&to, (void*)&from, BaseMixedType::ELEMENT_SIZE);
    LOG_TRACE("MixedType::Copy, {}", BaseMixedType::GetTypeName(to.type));
    switch(to.type) {
        case MixedValueType::kInvalid:
        case MixedValueType::kInteger:
        case MixedValueType::kFloat:
        case MixedValueType::kShortStr:
        case MixedValueType::kNull:
        case MixedValueType::kMissing:
        case MixedValueType::kNestedMissing:
        case MixedValueType::kNestedInteger:
        case MixedValueType::kNestedFloat:
        case MixedValueType::kNestedShortStr:
        case MixedValueType::kNestedNull:
        case MixedValueType::kDummy: {
            return ;
        }
        case MixedValueType::kTuple: {
            auto* from_ptr = (TupleMixedType*)(&from);
            auto* to_ptr = (TupleMixedType*)(&to);
//            to_ptr->ptr = new char_t[to_ptr->length];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->count * BaseMixedType::TUPLE_SIZE + sizeof(ptr_t));
            return ;
        }
        case MixedValueType::kLongStr: {
            auto* from_ptr = (LongStrMixedType*)(&from);
            auto* to_ptr = (LongStrMixedType*)(&to);
            u16 size_len = from_ptr->length;
            to_ptr->ptr = new char_t[size_len];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->length);
            return ;
        }
        case MixedValueType::kArray: {
            auto* from_ptr = (ArrayMixedType*)(&from);
            auto* to_ptr = (ArrayMixedType*)(&to);
//            to_ptr->ptr = new char_t[to_ptr->length];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->count * BaseMixedType::ELEMENT_SIZE + sizeof(ptr_t));
            return ;
        }
        case MixedValueType::kNestedTuple: {
            auto* from_ptr = (NestedTupleMixedType*)(&from);
            auto* to_ptr = (NestedTupleMixedType*)(&to);
//            to_ptr->ptr = new char_t[to_ptr->length];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->count * BaseMixedType::TUPLE_SIZE + sizeof(ptr_t));
            return ;
        }
        case MixedValueType::kNestedLongStr: {
            auto* from_ptr = (LongStrMixedType*)(&from);
            auto* to_ptr = (LongStrMixedType*)(&to);
            u16 size_len = to_ptr->length;
            to_ptr->ptr = new char_t[size_len];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->length);
            return ;
        }
        case MixedValueType::kNestedArray: {
            auto* from_ptr = (ArrayMixedType*)(&from);
            auto* to_ptr = (ArrayMixedType*)(&to);
//            to_ptr->ptr = new char_t[to_ptr->length];
            memcpy(to_ptr->ptr, from_ptr->ptr, from_ptr->count * BaseMixedType::ELEMENT_SIZE + sizeof(ptr_t));
            return ;
        }
    }
}

void
MixedType::Reset() {
    switch (this->type) {
        case MixedValueType::kInvalid:
        case MixedValueType::kInteger:
        case MixedValueType::kFloat:
        case MixedValueType::kNestedInteger:
        case MixedValueType::kNestedFloat:
        case MixedValueType::kShortStr:
        case MixedValueType::kNestedShortStr:
        case MixedValueType::kNull:
        case MixedValueType::kMissing:
        case MixedValueType::kNestedNull:
        case MixedValueType::kDummy:
        case MixedValueType::kNestedMissing:
            return;
        case MixedValueType::kLongStr: {
            // LongStrMixedType *long_str_mixed_ptr;
            auto *long_str_mixed_ptr = (LongStrMixedType *) this;
            long_str_mixed_ptr->Reset();
            return ;
        }
        case MixedValueType::kTuple: {
            // TupleMixedType *tuple_mixed_ptr;
            auto *tuple_mixed_ptr = (TupleMixedType *) this;
            tuple_mixed_ptr->Reset();
            return ;
        }
        case MixedValueType::kArray: {
            // ArrayMixedType *array_mixed_ptr;
            auto *array_mixed_ptr = (ArrayMixedType *) this;
            array_mixed_ptr->Reset();
            return ;
        }
        case MixedValueType::kNestedLongStr: {
            // NestedLongStrMixedType *nested_long_str_mixed_ptr;
            auto *nested_long_str_mixed_ptr = (NestedLongStrMixedType *) this;
            nested_long_str_mixed_ptr->Reset();
            return ;
        }
        case MixedValueType::kNestedTuple: {
            // NestedTupleMixedType *nested_tuple_mixed_ptr;
            auto *nested_tuple_mixed_ptr = (NestedTupleMixedType *) this;
            nested_tuple_mixed_ptr->Reset();
            return ;
        }
        case MixedValueType::kNestedArray: {
            // NestedArrayMixedType *nested_array_mixed_ptr;
            auto *nested_array_mixed_ptr = (NestedArrayMixedType *) this;
            nested_array_mixed_ptr->Reset();
            return ;
        }
    }
}

}
