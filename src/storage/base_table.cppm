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

export module infinity_core:base_table;

import :stl;
import :infinity_exception;

import global_resource_usage;

namespace infinity {

enum class BaseTableType {
    kInvalid,
    kTable,
    kCollection,
};

inline std::string ToString(BaseTableType type) {
    switch (type) {
        case BaseTableType::kTable: {
            return "Table";
        }
        case BaseTableType::kCollection: {
            return "Collection";
        }
        default: {
            UnrecoverableError("Invalid base table type");
        }
    }

    return std::string();
}

export class BaseTable {
public:
    explicit BaseTable(std::shared_ptr<std::string> schema_name, std::shared_ptr<std::string> table_name)
        : schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BaseTable");
#endif
    }

    BaseTable(const BaseTable &other_table) = delete;
    BaseTable(BaseTable &&other) = delete;

    virtual ~BaseTable() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BaseTable");
#endif
    }

    [[nodiscard]] inline const std::shared_ptr<std::string> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const std::shared_ptr<std::string> &table_name() const { return table_name_; }

    const std::shared_ptr<std::string> schema_name_{};
    const std::shared_ptr<std::string> table_name_{};
};

} // namespace infinity
