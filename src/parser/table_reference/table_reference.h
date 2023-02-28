//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "base_table_reference.h"

namespace infinity {

class TableReference: public BaseTableReference {

public:

    TableReference() : BaseTableReference(TableRefType::kTable) {}
    ~TableReference() override = default;

    String
    ToString() override;

    String schema_name_{};
    String table_name_{};
};

}
