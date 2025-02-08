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
#include <memory>
#include <vector>

namespace infinity {

struct SegmentEntry;

enum class CompactStatementType { kManual, kAuto, kInvalid };

class CompactStatement : public BaseStatement {
public:
    CompactStatement(CompactStatementType compact_type, const std::string &db_name, const std::string &table_name)
        : BaseStatement(StatementType::kCompact), compact_type_(compact_type), db_name_(db_name), table_name_(table_name) {}

    [[nodiscard]] std::string ToString() const final { return "Compact Table {}.{}"; }

public:
    CompactStatementType compact_type_;
    std::string db_name_{};
    std::string table_name_{};
};

class ManualCompactStatement : public CompactStatement {
public:
    ManualCompactStatement(const std::string &db_name, const std::string &table_name)
        : CompactStatement{CompactStatementType::kManual, db_name, table_name} {}

public:
};

struct AutoCompactStatement : public CompactStatement {
public:
    AutoCompactStatement(const std::string &db_name, const std::string &table_name, std::vector<SegmentEntry *> segments_to_compact)
        : CompactStatement{CompactStatementType::kAuto, db_name, table_name}, segments_to_compact_(std::move(segments_to_compact)) {}

public:
    std::vector<SegmentEntry *> segments_to_compact_;
};

} // namespace infinity