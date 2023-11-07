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

#include "update_statement.h"

namespace infinity {

UpdateStatement::~UpdateStatement() {
    if (where_expr_ != nullptr) {
        delete where_expr_;
        where_expr_ = nullptr;
    }

    if (update_expr_array_ != nullptr) {
        for (auto update_expr : *update_expr_array_) {
            delete update_expr;
        }
        delete update_expr_array_;
    }
}

std::string UpdateStatement::ToString() const { return "Update statement"; }

} // namespace infinity