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

module infinity_core:logical_check.impl;

import :logical_check;
import :stl;
import :infinity_exception;
import :column_binding;
import :logical_check;

import std;

import data_type;
import check_statement;
import internal_types;

namespace infinity {

String ToString(CheckStmtType type) {
    switch (type) {
        case CheckStmtType::kSystem:
            return "Check system";
        case CheckStmtType::kTable:
            return "Check table";
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid chunk scan type");
        }
    }
    return {};
}

Vector<ColumnBinding> LogicalCheck::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalCheck::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCheck::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalCheck::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "Check: ";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
