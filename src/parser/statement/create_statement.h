//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/extra/create_collection_info.h"
#include "parser/statement/extra/create_index_info.h"
#include "parser/statement/extra/create_schema_info.h"
#include "parser/statement/extra/create_table_info.h"
#include "parser/statement/extra/create_view_info.h"

namespace infinity {

class CreateStatement : public BaseStatement {
public:
    CreateStatement() : BaseStatement(StatementType::kCreate) {}

    [[nodiscard]] String ToString() const final;

    [[nodiscard]] inline DDLType ddl_type() const { return create_info_->type_; }

    SharedPtr<ExtraDDLInfo> create_info_{};
};

} // namespace infinity
