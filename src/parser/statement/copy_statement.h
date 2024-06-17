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
#include "statement_common.h"
#include "expr/parsed_expr.h"

namespace infinity {

enum class CopyOptionType {
    kFormat,
    kDelimiter,
    kHeader,
};

struct CopyOption {
    CopyOptionType option_type_{CopyOptionType::kFormat};
    bool header_{false};
    CopyFileType file_type_{CopyFileType::kCSV};
    char delimiter_{','};
};

class CopyStatement final : public BaseStatement {
public:
    CopyStatement() : BaseStatement(StatementType::kCopy) {}

    ~CopyStatement() final;

    [[nodiscard]] std::string ToString() const final;

    bool copy_from_{false};
    std::string file_path_{};
    std::string table_name_{};
    std::string schema_name_{};
    bool header_{false};
    CopyFileType copy_file_type_{CopyFileType::kCSV};
    char delimiter_{','};

    // EXPORT columns
    std::vector<ParsedExpr *> *expr_array_{nullptr};
};

} // namespace infinity
