//
// Created by jinhai on 23-3-16.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

enum class BaseTableType {
    kInvalid,
    kTable,
    kCollection,
};

inline String
ToString(BaseTableType type) {
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
    explicit
    BaseTable(BaseTableType kind, String schema_name, String table_name)
        : kind_(kind), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

    [[nodiscard]] inline BaseTableType
    kind() const {
        return kind_;
    }

    BaseTableType kind_{BaseTableType::kInvalid};

    [[nodiscard]] inline String
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline String
    table_name() const {
        return table_name_;
    }

    const String schema_name_{};
    const String table_name_{};
};

}
