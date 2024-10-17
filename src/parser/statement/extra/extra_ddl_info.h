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

#include <string>

namespace infinity {

enum class DDLType {
    kInvalid,
    kDatabase,
    kTable,
    kCollection,
    kView,
    kIndex,
};

enum class ConflictType { kInvalid, kIgnore, kError, kReplace };

std::string ConflictTypeToStr(ConflictType type);

class ExtraDDLInfo {
public:
    explicit ExtraDDLInfo(DDLType type) : type_(type) {}

    virtual ~ExtraDDLInfo() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    DDLType type_{DDLType::kInvalid};

    ConflictType conflict_type_{ConflictType::kInvalid};
    std::string schema_name_{};
    std::string comment_{};
};

} // namespace infinity
