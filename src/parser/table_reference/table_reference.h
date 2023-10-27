//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "base_table_reference.h"

namespace infinity {

class TableReference : public BaseTableReference {

public:
    TableReference() : BaseTableReference(TableRefType::kTable) {}

    ~TableReference() override = default;

    std::string ToString() override;

    std::string db_name_{"default"};
    std::string table_name_{};

    [[nodiscard]] std::string GetTableName() const;
};

} // namespace infinity
