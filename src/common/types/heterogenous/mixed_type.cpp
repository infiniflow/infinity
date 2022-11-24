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
        auto* long_mixed_str_ptr = (LongStrMixedType*)(&value);

        TypeAssert(str.size() <= 65535, "String length exceeds 65535.");
        long_mixed_str_ptr->length = static_cast<u16>(str_len);
        long_mixed_str_ptr->ptr = new char_t[str_len];
        memcpy(long_mixed_str_ptr->ptr, str.c_str(), str_len);
        // Fill long string prefix
        memcpy(long_mixed_str_ptr->header, str.c_str(), BaseMixedType::LONG_STR_HEADER);

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

void
MixedType::CopyIntoTuple(const String& key, const MixedType& value) {
    TypeAssert(this->type == MixedValueType::kTuple, "Not tuple type, can't set value.");
    auto* tuple_ptr = (TupleMixedType*)(this);
    TypeAssert(tuple_ptr->count > 0, "The tuple isn't initialized");

    auto* tuple_value_ptr = (MixedTupleValue*)(this->ptr);

    // TODO: currently, we use for loop to store the tuple. In the future, we will use hash table
    for(u16 i = 0; i < tuple_ptr->count; i += 2) {
        MixedType& key_ref = tuple_value_ptr->array[i];
        MixedType& value_ref = tuple_value_ptr->array[i + 1];
        if(key_ref.type == MixedValueType::kInvalid) {
            // Assign the input to the tuple slot
            key_ref = MakeString(key);
            value_ref = value;
        }
    }
}

void
MixedType::MoveIntoTuple(const String& key, MixedType&& value) {
    TypeAssert(this->type == MixedValueType::kTuple, "Not tuple type, can't set value.");
    auto* tuple_ptr = (TupleMixedType*)(this);
    TypeAssert(tuple_ptr->count > 0, "The tuple isn't initialized");

    auto* tuple_value_ptr = (MixedTupleValue*)(this->ptr);

    // TODO: currently, we use for loop to store the tuple. In the future, we will use hash table
    for(u16 i = 0; i < tuple_ptr->count; i += 2) {
        MixedType& key_ref = tuple_value_ptr->array[i];
        MixedType& value_ref = tuple_value_ptr->array[i + 1];
        if(key_ref.type == MixedValueType::kInvalid) {
            // Assign the input to the tuple slot
            key_ref = MakeString(key);
            value_ref = std::forward<MixedType>(value);
        }
    }
}

MixedType*
MixedType::GetFromTuple(const String& key) {
    TypeAssert(this->type == MixedValueType::kTuple, "Not tuple type, can't get value.");
    auto* tuple_ptr = (TupleMixedType*)(this);
    TypeAssert(tuple_ptr->count > 0, "The tuple isn't initialized");

    auto* tuple_value_ptr = (MixedTupleValue*)(this->ptr);

    // TODO: currently, we use for loop to get the tuple. In the future, we will use hash table
    for(u16 i = 0; i < tuple_ptr->count; i += 2) {
        MixedType& key_ref = tuple_value_ptr->array[i];
        MixedType& value_ref = tuple_value_ptr->array[i + 1];
        switch(key_ref.type) {
            case MixedValueType::kInvalid: {
                return nullptr;
            }
            case MixedValueType::kShortStr: {
                auto* key_ptr = (ShortStrMixedType*)(&key_ref);
                if(key_ptr->Equal(key)) {
                    return &value_ref;
                }
                break;
            }
            case MixedValueType::kLongStr: {
                auto* key_ptr = (LongStrMixedType*)(&key_ref);
                if(key_ptr->Equal(key)) {
                    return &value_ref;
                }
                break;
            }
            default: {
                TypeError("Unexpected heterogeneous type");
            }
        }
    }
    return nullptr;
}

void
MixedType::CopyIntoArray(const MixedType& value, u16 index) {

    // FIXME: need nested type
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    array_value_ptr->array[index] = value;
}

void
MixedType::MoveIntoArray(MixedType&& value, u16 index) {

    // FIXME: need nested type
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    array_value_ptr->array[index] = std::forward<MixedType>(value);
}

void
MixedType::InsertIntegerIntoArray(i64 value, u16 index) {
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    MixedType& slot_ref = array_value_ptr->array[index];
    slot_ref.Reset();
    auto* integer_value_ptr = (IntegerMixedType*)(&slot_ref);
    integer_value_ptr->type = MixedValueType::kInteger;
    integer_value_ptr->value = value;
}

void
MixedType::InsertFloatIntoArray(f64 value, u16 index) {
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    MixedType& slot_ref = array_value_ptr->array[index];
    slot_ref.Reset();
    slot_ref.type = MixedValueType::kFloat;
    auto* float_value_ptr = (FloatMixedType*)(&slot_ref);
    float_value_ptr->value = value;
}

void
MixedType::InsertStringIntoArray(const String& value, u16 index) {
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    MixedType& slot_ref = array_value_ptr->array[index];
    slot_ref.Reset();

    size_t str_len = value.size();
    if(str_len <= BaseMixedType::SHORT_STR_LIMIT) {
        // ShortStrMixedType* short_mixed_ptr
        auto* short_mixed_ptr = (ShortStrMixedType*)(&slot_ref);
        short_mixed_ptr->type = MixedValueType::kShortStr;
        memcpy(short_mixed_ptr->ptr, value.c_str(), str_len);
        if(str_len < BaseMixedType::SHORT_STR_LIMIT) {
            memset(short_mixed_ptr->ptr + str_len, 0, BaseMixedType::SHORT_STR_LIMIT - str_len);
        }
        short_mixed_ptr->length = static_cast<i8>(str_len);
    } else {
        // LongStrMixedType* long_mixed_str
        auto* long_mixed_str_ptr = (LongStrMixedType*)(&array_value_ptr->array[index]);
        if(long_mixed_str_ptr->type == MixedValueType::kLongStr) {
            // Not empty, reset exist value
            long_mixed_str_ptr->Reset();
        }

        TypeAssert(value.size() <= 65535, "String length exceeds 65535.");
        long_mixed_str_ptr->length = static_cast<u16>(str_len);
        long_mixed_str_ptr->ptr = new char_t[str_len];
        memcpy(long_mixed_str_ptr->ptr, value.c_str(), str_len);
        // Fill long string prefix
        memcpy(long_mixed_str_ptr->header, value.c_str(), BaseMixedType::LONG_STR_HEADER);
    }
}

void
MixedType::InsertNullIntoArray(u16 index) {
    auto* array_mixed_ptr = (ArrayMixedType*)this;
    TypeAssert(index < array_mixed_ptr->count, "Index is invalid");

    auto* array_value_ptr = (MixedArrayValue*)(array_mixed_ptr->ptr);
    MixedType& slot_ref = array_value_ptr->array[index];
    slot_ref.Reset();
    slot_ref.type = MixedValueType::kNull;
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
    }
}

void
MixedType::Reset() {
    switch (this->type) {
        case MixedValueType::kInvalid:
        case MixedValueType::kInteger:
        case MixedValueType::kFloat:
        case MixedValueType::kShortStr:
        case MixedValueType::kNull:
        case MixedValueType::kMissing:
        case MixedValueType::kDummy:
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
    }
}

}
