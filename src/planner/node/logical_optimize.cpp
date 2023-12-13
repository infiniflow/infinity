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

import stl;
import parser;
import column_binding;

module logical_optimize;

namespace infinity {

Vector<ColumnBinding> LogicalOptimize::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalOptimize::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalOptimize::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalOptimize::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    switch (optimize_type_) {
        case OptimizeType::kIRS:
            ss << String(space, ' ') << "-> "
               << "Optimize Index: ";
            break;
    }
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
