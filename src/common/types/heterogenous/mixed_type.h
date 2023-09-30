//
// Created by JinHai on 2022/11/4.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/utility/infinity_assert.h"

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

namespace infinity {

// Not nested data type
struct __attribute__((packed)) MixedType : public BaseMixedType {

    // non-static member variable
public:
    char_t ptr[ELEMENT_SIZE - 1]{};
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

    void
    InsertIntegerIntoTuple(const String& key, i64 value);

    void
    InsertFloatIntoTuple(const String& key, f64 value);

    void
    InsertStringIntoTuple(const String& key, const String& value);

    void
    InsertNullIntoTuple(const String& key);

    MixedType*
    GetFromTuple(const String& key) const;

private:
    static void
    CheckKeyConflict(MixedType& key_ref, const String& key_str);

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

    MixedType*
    GetByIndex(u16) const;

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

    MixedType&
    operator=(const MixedType& other);

    MixedType&
    operator=(MixedType&& other) noexcept;

    bool
    operator==(const MixedType& other) const;

    inline bool
    operator!=(const MixedType& other) const {
        return !operator==(other);
    }

    void
    Reset(bool in_constructor = false);

    [[nodiscard]] inline String
    ToString() const {
        TypeError("ToString() isn't implemented");
    }

private:
    static void
    Copy(const MixedType& from, MixedType& to);

    static void
    Move(MixedType&& from, MixedType& to);
};


}
