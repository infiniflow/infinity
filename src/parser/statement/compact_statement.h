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

struct TableEntry;
struct SegmentEntry;

enum class CompactStatementType {
    kManual,
    kAuto,
    kInvalid
};

class CompactStatement : public BaseStatement {
public:
    CompactStatement(CompactStatementType compact_type) : BaseStatement(StatementType::kCompact), compact_type_(compact_type) {}

    [[nodiscard]] std::string ToString() const final { return "Compact Table {}.{}"; }

public:
    CompactStatementType compact_type_;
};

class ManualCompactStatement : public CompactStatement {
public:
    ManualCompactStatement(std::string schema_name, std::string table_name)
        : CompactStatement{CompactStatementType::kManual}, schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

public:
    std::string schema_name_{};
    std::string table_name_{};
};

struct AutoCompactStatement : public CompactStatement {
public:
    AutoCompactStatement(TableEntry *table_entry, std::vector<SegmentEntry *> compactible_segments)
        : CompactStatement{CompactStatementType::kAuto}, table_entry_(table_entry), compactible_segments_(std::move(compactible_segments)) {}

public:
    TableEntry *table_entry_;
    std::vector<SegmentEntry *> compactible_segments_;
};

} // namespace infinity