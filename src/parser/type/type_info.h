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

#include "json.hpp"
#include <cstdint>
#include <string>

namespace infinity {

enum class TypeInfoType : uint8_t {
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
    kSparse,
};

class TypeInfo {
public:
    explicit TypeInfo(TypeInfoType type) : type_(type){};

    virtual ~TypeInfo() = default;

    virtual bool operator==(const TypeInfo &other) const = 0;

    bool operator!=(const TypeInfo &other) const;

    [[nodiscard]] virtual size_t Size() const = 0;

    [[nodiscard]] inline TypeInfoType type() const noexcept { return type_; }

    [[nodiscard]] virtual nlohmann::json Serialize() const = 0;

    [[nodiscard]] virtual std::string ToString() const = 0;

protected:
    TypeInfoType type_{TypeInfoType::kInvalid};
};

} // namespace infinity
