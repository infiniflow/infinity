//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/alias/strings.h"
#include "common/types/alias/primitives.h"
#include "json.hpp"
#include <iostream>

namespace infinity {

enum class TypeInfoType : u8 {
    kInvalid,

    // Primitive
    kDecimal,
    kVarchar,

    // Nested
    kArray,
    kTuple, // Dictionary

    // Geography
    kPoint,
    kLine,
    kLineSeg,
    kBox,
    kPath,
    kPolygon,
    kCircle,

    // Other
    kBitmap,
    kUUID,
    kBlob,
    kEmbedding,

};


class TypeInfo {
public:
    explicit
    TypeInfo(TypeInfoType type) : type_(type) {
    };

    virtual
    ~TypeInfo() = default;

    virtual bool
    operator==(const TypeInfo& other) const = 0;

    bool
    operator!=(const TypeInfo& other) const;

    [[nodiscard]] virtual size_t
    Size() const = 0;

    [[nodiscard]] inline TypeInfoType
    type() const noexcept {
        return type_;
    }

    [[nodiscard]] virtual nlohmann::json
    Serialize() const = 0;

    [[nodiscard]] virtual String
    ToString() const = 0;

protected:
    TypeInfoType type_{TypeInfoType::kInvalid};
};

}
