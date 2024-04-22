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

module;

#include <sstream>

module logical_show;

import stl;
import internal_types;
import infinity_exception;
import column_binding;


namespace infinity {

String ToString(ShowType type) {
    switch (type) {
        case ShowType::kShowDatabase:
            return "Show database";
        case ShowType::kShowTable:
            return "Show table";
        case ShowType::kShowIndex:
            return "Show index";
        case ShowType::kShowTables:
            return "Show tables";
        case ShowType::kShowViews:
            return "Show views";
        case ShowType::kShowColumn:
            return "Show column";
        case ShowType::kShowIndexes:
            return "Show indexes";
        case ShowType::kShowDatabases:
            return "Show databases";
        case ShowType::kShowConfigs:
            return "Show configs";
        case ShowType::kShowProfiles:
            return "Show profiles";
        case ShowType::kShowSegments:
            return "Show segments";
        case ShowType::kShowSegment:
            return "Show segment";
        case ShowType::kShowBlocks:
            return "Show blocks";
        case ShowType::kShowBlock:
            return "Show block";
        case ShowType::kShowBlockColumn:
            return "Show block column";
        case ShowType::kShowSessionStatus:
            return "Show session status";
        case ShowType::kShowGlobalStatus:
            return "Show global status";
        case ShowType::kShowVar:
            return "Show variables";
        case ShowType::kInvalid: {
            UnrecoverableError("Invalid chunk scan type");
        }
    }
    return {};
}

Vector<ColumnBinding> LogicalShow::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalShow::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalShow::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalShow::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "Show: ";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
