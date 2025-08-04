// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "array_mixed_type.h"
#include "base_mixed_type.h"
#include "float_mixed_type.h"
#include "integer_mixed_type.h"
#include "long_string_mixed_type.h"
#include "missing_mixed_type.h"
#include "mixed_value_type.h"
#include "null_mixed_type.h"
#include "parser_assert.h"
#include "short_string_mixed_type.h"
#include "tuple_mixed_type.h"
#include <cstdint>

namespace infinity {

// Not nested data type
struct __attribute__((packed)) MixedType : public BaseMixedType {

    // non-static member variable
public:
    char ptr[ELEMENT_SIZE - 1]{};

public:
    static MixedType MakeInteger(int64_t value);

    static MixedType MakeFloat(double value);

    static MixedType MakeString(const std::string &str);

    static MixedType MakeTuple(uint16_t count);

    static MixedType MakeArray(uint16_t count);

    static MixedType MakeNull();

    static MixedType MakeMissing();

    // Tuple related method
public:
    void CopyIntoTuple(const std::string &key, const MixedType &value);

    void MoveIntoTuple(const std::string &key, MixedType &&value);

    void InsertIntegerIntoTuple(const std::string &key, int64_t value);

    void InsertFloatIntoTuple(const std::string &key, double value);

    void InsertStringIntoTuple(const std::string &key, const std::string &value);

    void InsertNullIntoTuple(const std::string &key);

    MixedType *GetFromTuple(const std::string &key) const;

private:
    static void CheckKeyConflict(MixedType &key_ref, const std::string &key_str);

    // Array related method
public:
    void CopyIntoArray(const MixedType &value, uint16_t index);

    void MoveIntoArray(MixedType &&value, uint16_t index);

    void InsertIntegerIntoArray(int64_t value, uint16_t index);

    void InsertFloatIntoArray(double value, uint16_t index);

    void InsertStringIntoArray(const std::string &value, uint16_t index);

    void InsertNullIntoArray(uint16_t index);

    MixedType *GetByIndex(uint16_t) const;

    // non-static member
public:
    MixedType() = default;

    explicit inline MixedType(MixedValueType data_type) : BaseMixedType(data_type) {}

    inline ~MixedType() { Reset(); }

    MixedType(const MixedType &other);

    MixedType(MixedType &&other) noexcept;

    MixedType &operator=(const MixedType &other);

    MixedType &operator=(MixedType &&other) noexcept;

    bool operator==(const MixedType &other) const;

    inline bool operator!=(const MixedType &other) const { return !operator==(other); }

    void Reset(bool in_constructor = false);

    [[nodiscard]] inline std::string ToString() const {
        // This is a simplified implementation for MixedType
        // A full implementation would need to handle all the different value types
        switch (type) {
            case MixedValueType::kInteger: {
                const auto *int_type = reinterpret_cast<const IntegerMixedType *>(this);
                return std::to_string(int_type->value);
            }
            case MixedValueType::kFloat: {
                const auto *float_type = reinterpret_cast<const FloatMixedType *>(this);
                return std::to_string(float_type->value);
            }
            case MixedValueType::kShortStr: {
                const auto *str_type = reinterpret_cast<const ShortStrMixedType *>(this);
                return std::string(str_type->ptr, str_type->length);
            }
            case MixedValueType::kLongStr: {
                const auto *str_type = reinterpret_cast<const LongStrMixedType *>(this);
                return std::string(str_type->ptr, str_type->length);
            }
            case MixedValueType::kNull:
                return "NULL";
            case MixedValueType::kMissing:
                return "MISSING";
            case MixedValueType::kArray:
                return "[ARRAY]";
            case MixedValueType::kTuple:
                return "(TUPLE)";
            default:
                return "UNKNOWN";
        }
    }

private:
    static void Copy(const MixedType &from, MixedType &to);

    static void Move(MixedType &&from, MixedType &to);
};

} // namespace infinity
