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

#include "admin_statement.h"
#include <sstream>

namespace infinity {

std::string AdminStatement::ToString() const {
    std::stringstream ss;
    switch (admin_type_) {
        case AdminStmtType::kListLogFiles: {
            ss << "Admin list log files";
            break;
        }
        case AdminStmtType::kShowLogFile: {
            ss << "Admin list log file";
            break;
        }
        case AdminStmtType::kListLogIndexes: {
            ss << "Admin list log indexes";
            break;
        }
        case AdminStmtType::kShowLogIndex: {
            ss << "Admin list log index";
            break;
        }
        case AdminStmtType::kListCatalogs: {
            ss << "Admin list catalog files";
            break;
        }
        case AdminStmtType::kShowCatalog: {
            ss << "Admin show catalog file: ";
            break;
        }
        case AdminStmtType::kListDatabases: {
            ss << "Admin list databases: ";
            break;
        }
        case AdminStmtType::kShowDatabase: {
            ss << "Admin show database";
            break;
        }
        case AdminStmtType::kListTables: {
            ss << "Admin list tables";
            break;
        }
        case AdminStmtType::kShowTable: {
            ss << "Admin show table";
            break;
        }
        case AdminStmtType::kListSegments: {
            ss << "Admin list segments";
            break;
        }
        case AdminStmtType::kShowSegment: {
            ss << "Admin show segment";
            break;
        }
        case AdminStmtType::kListBlocks: {
            ss << "Admin list blocks";
            break;
        }
        case AdminStmtType::kShowBlock: {
            ss << "Admin show block";
            break;
        }
        case AdminStmtType::kListColumns: {
            ss << "Admin list columns";
            break;
        }
        case AdminStmtType::kShowColumn: {
            ss << "Admin show column";
            break;
        }
        case AdminStmtType::kListIndexes: {
            ss << "Admin list indexes";
            break;
        }
        case AdminStmtType::kShowIndex: {
            ss << "Admin show index";
            break;
        }
        case AdminStmtType::kListIndexSegments: {
            ss << "Admin list index segments";
            break;
        }
        case AdminStmtType::kShowIndexSegment: {
            ss << "Admin show index segment";
            break;
        }
        case AdminStmtType::kListConfigs: {
            ss << "Admin list configs";
            break;
        }
        case AdminStmtType::kListVariables: {
            ss << "Admin list variables";
            break;
        }
        case AdminStmtType::kShowVariable: {
            ss << "Admin show variable";
            break;
        }
        case AdminStmtType::kListNodes: {
            ss << "Admin list nodes";
            break;
        }
        case AdminStmtType::kShowNode: {
            ss << "Admin show node";
            break;
        }
        case AdminStmtType::kShowCurrentNode: {
            ss << "Admin show current node";
            break;
        }
        case AdminStmtType::kRemoveNode: {
            ss << "Admin remove node";
            break;
        }
        case AdminStmtType::kSetRole: {
            ss << "Admin set role";
            break;
        }
        case AdminStmtType::kCreateSnapshot: {
            ss << "Admin create snapshot";
            break;
        }
        case AdminStmtType::kListSnapshots: {
            ss << "Admin list snapshots";
            break;
        }
        case AdminStmtType::kShowSnapshot: {
            ss << "Admin show snapshot";
            break;
        }
        case AdminStmtType::kDeleteSnapshot: {
            ss << "Admin delete snapshot";
            break;
        }
        case AdminStmtType::kExportSnapshot: {
            ss << "Admin export snapshot";
            break;
        }
        case AdminStmtType::kRecoverFromSnapshot: {
            ss << "Admin recover from snapshot";
            break;
        }
        case AdminStmtType::kInvalid: {
            ss << "Invalid admin statement";
            break;
        }
    }
    return ss.str();
}

std::string ToString(NodeRole role) {
    switch (role) {
        case NodeRole::kUnInitialized:
            return "uninitialized";
        case NodeRole::kAdmin:
            return "admin";
        case NodeRole::kStandalone:
            return "standalone";
        case NodeRole::kLeader:
            return "leader";
        case NodeRole::kFollower:
            return "follower";
        case NodeRole::kLearner:
            return "learner";
    }
}

} // namespace infinity
