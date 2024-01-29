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

#include "base_statement.h"

namespace infinity {

class CompactStatement : public BaseStatement {
public:
    CompactStatement(std::string &&schema_name, std::string &&table_name)
        : BaseStatement(StatementType::kCompact), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

    CompactStatement(std::string &&table_name) : BaseStatement(StatementType::kCompact), table_name_(std::move(table_name)) {}

    [[nodiscard]] std::string ToString() const final;

    const std::string schema_name_{"default"};
    const std::string table_name_;
};
} // namespace infinity