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

#include <cctype>

import stl;
import third_party;

module reference_expression;

namespace infinity {

String ReferenceExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        SizeT name_len = column_name_.size();
        for (SizeT idx = 0; idx < name_len; ++idx) {
            char a = column_name_[idx];
            if (!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return fmt::format("{}.{}", table_name_, column_name_);
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

} // namespace infinity