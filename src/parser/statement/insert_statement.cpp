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

#include "insert_statement.h"

namespace infinity {

InsertStatement::~InsertStatement() {
    if (select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }

    if (columns_ != nullptr) {
        delete columns_;
        columns_ = nullptr;
    }

    if (values_ != nullptr) {
        for (auto &value_ptr : *values_) {
            delete value_ptr;
        }
        delete values_;
        values_ = nullptr;
    }
}

std::string InsertStatement::ToString() const { return "insert statement"; }

} // namespace infinity