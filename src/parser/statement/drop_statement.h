//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/extra/drop_schema_info.h"
#include "parser/statement/extra/drop_collection_info.h"
#include "parser/statement/extra/drop_table_info.h"
#include "parser/statement/extra/drop_index_info.h"
#include "parser/statement/extra/drop_view_info.h"

namespace infinity {

class DropStatement : public BaseStatement {
public:
    DropStatement() : BaseStatement(StatementType::kDrop) {}

    [[nodiscard]] String
    ToString() const final;

    [[nodiscard]] inline DDLType
    ddl_type() const {
        return drop_info_->type_;
    }

    UniquePtr<ExtraDDLInfo> drop_info_;
};

}
