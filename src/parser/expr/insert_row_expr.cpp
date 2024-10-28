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

#include "insert_row_expr.h"
#include <sstream>

namespace infinity {

std::string InsertRowExpr::ToString() const {
    std::ostringstream oss;
    oss << "InsertRowExpr(columns: [";
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << columns_[i];
    }
    oss << "], values: [";
    for (size_t i = 0; i < values_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << values_[i]->ToString();
    }
    oss << "])";
    return std::move(oss).str();
}

} // namespace infinity
