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

#include "show_statement.h"
#include <sstream>

namespace infinity {

std::string ShowStatement::ToString() const {
    std::stringstream ss;
    switch (show_type_) {
        case ShowStmtType::kTables: {
            ss << "Show tables, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kCollections: {
            ss << "Show collections, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kViews: {
            ss << "Show views, schema: " << schema_name_;
            break;
        }
        case ShowStmtType::kColumns: {
            ss << "Describe table: " << schema_name_ << "." << table_name_;
            break;
        }
        case ShowStmtType::kIndexes: {
            ss << "Describe index of table: " << schema_name_ << ", table: " << table_name_;
            break;
        }
        case ShowStmtType::kDatabases: {
            ss << "Show databases";
            break;
        }
        case ShowStmtType::kSegments : {
            ss << "Show Segments";
            break;
        }
        case ShowStmtType::kConfigs: {
            ss << "Show configs";
            break;
        }
        case ShowStmtType::kProfiles: {
            ss << "Show profiles";
            break;
        }
        case ShowStmtType::kSessionStatus: {
            ss << "Show session status";
            break;
        }
        case ShowStmtType::kGlobalStatus: {
            ss << "Show global status";
            break;
        }
    }
    return ss.str();
}

} // namespace infinity
