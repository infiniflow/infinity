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

module;

export module table_ref;

import stl;

import table_reference;

namespace infinity {

export class TableRef {
public:
    explicit TableRef(TableRefType type, String alias) : type_(type), alias_(std::move(alias)) {}
    virtual ~TableRef() = default;

    [[nodiscard]] inline TableRefType type() const { return type_; }
    [[nodiscard]] inline const String &alias() const { return alias_; }

    TableRefType type_{TableRefType::kTable};
    String alias_{};
};

} // namespace infinity
