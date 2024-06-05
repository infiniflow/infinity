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

export module base_table;

import stl;
import table_entry_type;
import logger;
import infinity_exception;

namespace infinity {

enum class BaseTableType {
    kInvalid,
    kTable,
    kCollection,
};

inline String ToString(BaseTableType type) {
    switch (type) {
        case BaseTableType::kTable: {
            return "Table";
        }
        case BaseTableType::kCollection: {
            return "Collection";
        }
        default: {
            String error_message = "Invalid base table type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    return String();
}

export class BaseTable {
public:
    explicit BaseTable(TableEntryType kind, SharedPtr<String> schema_name, SharedPtr<String> table_name)
            : kind_(kind), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

    [[nodiscard]] inline TableEntryType kind() const { return kind_; }

    TableEntryType kind_{TableEntryType::kTableEntry};

    [[nodiscard]] inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const SharedPtr<String> &table_name() const { return table_name_; }

    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
};

} // namespace infinity
