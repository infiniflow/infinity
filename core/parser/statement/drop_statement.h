//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"
#include "statement/extra/drop_collection_info.h"
#include "statement/extra/drop_index_info.h"
#include "statement/extra/drop_schema_info.h"
#include "statement/extra/drop_table_info.h"
#include "statement/extra/drop_view_info.h"

#include <memory>

namespace infinity {

class DropStatement : public BaseStatement {
public:
    DropStatement() : BaseStatement(StatementType::kDrop) {}

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] inline DDLType ddl_type() const { return drop_info_->type_; }

    std::shared_ptr<ExtraDDLInfo> drop_info_;
};

} // namespace infinity
