//
// Created by jinhai on 23-10-16.
//

module;

import stl;
import table_collection_type;
import infinity_assert;
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
            Error<ExecutorException>("Invalid base table type", __FILE_NAME__, __LINE__);
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
