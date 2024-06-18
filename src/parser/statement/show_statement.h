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
#include <optional>

namespace infinity {

enum class ShowStmtType {
    kDatabase,
    kTable,
    kIndex,
    kDatabases,
    kColumns,
    kTables,
    kCollections,
    kViews,
    kIndexes,
    kConfigs,
    kProfiles,
    kSegments,
    kSegment,
    kBlocks,
    kBlock,
    kBlockColumn,
    kSessionVariable,
    kSessionVariables,
    kGlobalVariable,
    kGlobalVariables,
    kConfig,
    kBuffer,
    kQueries,
    kQuery,
    kTransactions,
    kTransaction,
    kIndexSegment,
    kIndexChunk,
};

class ShowStatement : public BaseStatement {
public:
    explicit ShowStatement() : BaseStatement(StatementType::kShow) {}

    [[nodiscard]] std::string ToString() const final;

    ShowStmtType show_type_{ShowStmtType::kTables};
    std::string schema_name_{};
    std::string table_name_{};
    std::optional<std::string> index_name_{};
    std::optional<int64_t> segment_id_{};
    std::optional<int64_t> block_id_{};
    std::optional<int64_t> chunk_id_{};
    std::optional<u_int64_t> column_id_{};
    std::optional<u_int64_t> session_id_{};
    std::optional<u_int64_t> txn_id_{};
    std::string var_name_{};
};

} // namespace infinity
