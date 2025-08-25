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

module infinity_core:logical_show.impl;

import :logical_show;
import :infinity_exception;
import :column_binding;

import std;

import show_statement;
import data_type;
import internal_types;

namespace infinity {

std::string ToString(ShowStmtType type) {
    switch (type) {
        case ShowStmtType::kDatabase:
            return "Show database";
        case ShowStmtType::kTable:
            return "Show table";
        case ShowStmtType::kIndex:
            return "Show index";
        case ShowStmtType::kIndexSegment:
            return "Show index segment";
        case ShowStmtType::kIndexChunk:
            return "Show index chunk";
        case ShowStmtType::kTables:
            return "Show tables";
        case ShowStmtType::kTasks:
            return "Show views";
        case ShowStmtType::kColumns:
            return "Show column";
        case ShowStmtType::kIndexes:
            return "Show indexes";
        case ShowStmtType::kDatabases:
            return "Show databases";
        case ShowStmtType::kConfigs:
            return "Show configs";
        case ShowStmtType::kProfiles:
            return "Show profiles";
        case ShowStmtType::kSegments:
            return "Show segments";
        case ShowStmtType::kSegment:
            return "Show segment";
        case ShowStmtType::kBlocks:
            return "Show blocks";
        case ShowStmtType::kBlock:
            return "Show block";
        case ShowStmtType::kBlockColumn:
            return "Show block column";
        case ShowStmtType::kSessionVariable:
            return "Show session variable";
        case ShowStmtType::kSessionVariables:
            return "Show session variables";
        case ShowStmtType::kGlobalVariable:
            return "Show global variable";
        case ShowStmtType::kGlobalVariables:
            return "Show global variables";
        case ShowStmtType::kConfig:
            return "Show config";
        case ShowStmtType::kBuffer:
            return "Show buffer";
        case ShowStmtType::kMemIndex:
            return "Show memindex";
        case ShowStmtType::kQueries:
            return "Show queries";
        case ShowStmtType::kQuery:
            return "Show query";
        case ShowStmtType::kTransactions:
            return "Show transactions";
        case ShowStmtType::kTransaction:
            return "Show transaction";
        case ShowStmtType::kTransactionHistory:
            return "Show transaction history";
        case ShowStmtType::kLogs:
            return "Show logs";
        case ShowStmtType::kCatalog:
            return "Show catalog";
        case ShowStmtType::kCatalogToFile:
            return "Show catalog to file";
        case ShowStmtType::kPersistenceFiles:
            return "Show persistence files";
        case ShowStmtType::kPersistenceObjects:
            return "Show persistence objects";
        case ShowStmtType::kPersistenceObject:
            return "Show persistence object";
        case ShowStmtType::kMemory:
            return "Show memory";
        case ShowStmtType::kMemoryObjects:
            return "Show memory objects";
        case ShowStmtType::kMemoryAllocation:
            return "Show memory allocation";
        case ShowStmtType::kFunction:
            return "Show function";
        case ShowStmtType::kCollections:
            return "Show collection";
        case ShowStmtType::kListSnapshots:
            return "List snapshots";
        case ShowStmtType::kShowSnapshot:
            return "Show snapshot";
        case ShowStmtType::kListCaches:
            return "List caches";
        case ShowStmtType::kShowCache:
            return "Show cache";
        case ShowStmtType::kInvalid: {
            UnrecoverableError("Invalid chunk scan type");
        }
    }
    return {};
}

std::vector<ColumnBinding> LogicalShow::GetColumnBindings() const { return {}; }

std::shared_ptr<std::vector<std::string>> LogicalShow::GetOutputNames() const { return std::make_shared<std::vector<std::string>>(); }

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalShow::GetOutputTypes() const {
    return std::make_shared<std::vector<std::shared_ptr<DataType>>>();
}

std::string LogicalShow::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << "-> "
       << "Show: ";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
