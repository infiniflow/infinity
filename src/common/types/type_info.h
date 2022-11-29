//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

enum class TypeInfoType {
    kInvalid,

    // Primitive
    kDecimal16,
    kDecimal32,
    kDecimal64,
    kDecimal128,
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
        GlobalResourceUsage::IncrObjectCount();
    };

    virtual
    ~TypeInfo() {
        GlobalResourceUsage::DecrObjectCount();
    }

    bool
    operator==(const TypeInfo& other) const;

    bool
    operator!=(const TypeInfo& other) const;

    [[nodiscard]] virtual size_t
    Size() const = 0;

    [[nodiscard]] inline TypeInfoType
    type() const noexcept {
        return type_;
    }

protected:
    TypeInfoType type_ { TypeInfoType::kInvalid };
};

}
