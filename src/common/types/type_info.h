//
// Created by JinHai on 2022/10/28.
//

#pragma once

namespace infinity {

enum class TypeInfoType {
    kInvalid,

    // Primitive
    kDecimal,
    kVarchar,

    // Nested
    kArray,
    kObject, // Dictionary

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
    TypeInfo(TypeInfoType type) : type_(type) {};

    virtual
    ~TypeInfo() = default;

    bool
    operator==(const TypeInfo& other) const;

    bool
    operator!=(const TypeInfo& other) const;

private:
    TypeInfoType type_ { TypeInfoType::kInvalid };
};

}
