//
// Created by jinhai on 23-3-16.
//

#pragma once

#include "common/types/alias/smart_ptr.h"
#include "storage/meta/entry/table_collecton_type.h"

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
            ExecutorError("Invalid base table type")
        }
    }
}

class BaseTable {
public:
    explicit BaseTable(TableCollectionType kind, SharedPtr<String> schema_name, SharedPtr<String> table_name)
        : kind_(kind), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

    [[nodiscard]] inline TableCollectionType kind() const { return kind_; }

    TableCollectionType kind_{TableCollectionType::kTableEntry};

    [[nodiscard]] inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const SharedPtr<String> &table_name() const { return table_name_; }

    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
};

} // namespace infinity
