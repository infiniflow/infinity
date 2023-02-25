//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/extra/drop_schema_info.h"
#include "parser/statement/extra/drop_collection_info.h"

namespace infinity {

class DropStatement : public BaseStatement {
public:
    DropStatement() : BaseStatement(StatementType::kDrop) {}

    [[nodiscard]] String
    ToString() const final;

    UniquePtr<ExtraDDLInfo> drop_info_;
};

}
