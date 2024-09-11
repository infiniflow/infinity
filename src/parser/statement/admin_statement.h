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

enum class AdminStmtType {
    kListLogFiles,
    kShowLogFile,
    kListLogIndexes,
    kShowLogIndex,
    kListCatalogs,
    kShowCatalog,
    kListDatabases,
    kShowDatabase,
    kListTables,
    kShowTable,
    kListSegments,
    kShowSegment,
    kListBlocks,
    kShowBlock,
    kListColumns,
    kShowColumn,
    kListIndexes,
    kShowIndex,
    kListIndexSegments,
    kShowIndexSegment,
    kListConfigs,
    kListVariables,
    kShowVariable,
    kListNodes,
    kShowNode,
    kShowCurrentNode,
    kSetRole,
    kInvalid,
};

enum class AdminNodeRole {
    kAdmin,
    kStandalone,
    kLeader,
    kFollower,
    kLearner
};

class AdminStatement : public BaseStatement {
public:
    explicit AdminStatement() : BaseStatement(StatementType::kAdmin) {}

    [[nodiscard]] std::string ToString() const final;

    AdminStmtType admin_type_{AdminStmtType::kInvalid};
    std::optional<int64_t> catalog_file_index_{};
    std::optional<int64_t> catalog_file_start_index_{};
    std::optional<int64_t> catalog_file_end_index_{};
    std::optional<int64_t> database_meta_index_{};
    std::optional<int64_t> database_entry_index_{};
    std::optional<int64_t> table_meta_index_{};
    std::optional<int64_t> table_entry_index_{};
    std::optional<int64_t> segment_index_{};
    std::optional<int64_t> block_index_{};
    std::optional<int64_t> column_index_{};
    std::optional<int64_t> index_meta_index_{};
    std::optional<int64_t> index_entry_index_{};
    std::optional<int64_t> log_file_index_{};
    std::optional<int64_t> log_index_in_file_{};
    std::optional<AdminNodeRole> admin_node_role_{};
    std::optional<std::string> leader_address_{};
    std::optional<std::string> variable_name_{};
    std::optional<std::string> node_name_{};
};

} // namespace infinity
