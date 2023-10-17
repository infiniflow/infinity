//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"
#include "statement/extra/create_collection_info.h"
#include "statement/extra/create_index_info.h"
#include "statement/extra/create_schema_info.h"
#include "statement/extra/create_table_info.h"
#include "statement/extra/create_view_info.h"

#include <string>
#include <memory>

namespace infinity {

class CreateStatement : public BaseStatement {
public:
    CreateStatement() : BaseStatement(StatementType::kCreate) {}

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] inline DDLType ddl_type() const { return create_info_->type_; }

    std::shared_ptr<ExtraDDLInfo> create_info_{};
};

} // namespace infinity
