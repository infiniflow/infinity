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

#include <utility>

#include "definition/column_def.h"
#include "extra_ddl_info.h"
#include "statement/select_statement.h"
#include "statement/statement_common.h"
#include "type/data_type.h"
#include "type/info/bitmap_info.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include <unordered_set>

namespace infinity {

class CreateTableInfo final : public ExtraDDLInfo {
public:
    explicit CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    ~CreateTableInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string table_name_{};

    std::vector<ColumnDef *> column_defs_{};
    std::vector<TableConstraint *> constraints_{};

    std::vector<infinity::InitParameter *> properties_{};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
