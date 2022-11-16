//
// Created by JinHai on 2022/11/4.
//

#include "mixed_type.h"

namespace infinity {
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
    }
}

MixedType
MixedType::MakeObject() {
    MixedType value(MixedValueType::kObject);
    return value;
}

MixedType
MixedType::MakeArray(ptr_t ptr, i32 count) {

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

// Non-static member method
MixedType::MixedType(const MixedType& other) {

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
