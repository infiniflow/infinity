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

import stl;
import table_collection_type;

import infinity_exception;

export module base_table;

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
            Error<ExecutorException>("Invalid base table type");
        }
    }
}

export class BaseTable {
public:
    explicit BaseTable(TableCollectionType kind, SharedPtr<String> schema_name, SharedPtr<String> table_name)
            : kind_(kind), schema_name_(Move(schema_name)), table_name_(Move(table_name)) {}

    [[nodiscard]] inline TableCollectionType kind() const { return kind_; }

    TableCollectionType kind_{TableCollectionType::kTableEntry};

    [[nodiscard]] inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const SharedPtr<String> &table_name() const { return table_name_; }

    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
};

} // namespace infinity
