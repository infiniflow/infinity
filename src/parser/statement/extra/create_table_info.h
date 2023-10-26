//
// Created by jinhai on 23-2-25.
//

#pragma once

#include <utility>

#include "type/data_type.h"
#include "type/info/bitmap_info.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include "type/info/varchar_info.h"
#include "extra_ddl_info.h"
#include "statement/select_statement.h"
#include "statement/statement_common.h"
#include "definition/column_def.h"
#include <unordered_set>

namespace infinity {

class CreateTableInfo final : public ExtraDDLInfo {
public:
    explicit CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    ~CreateTableInfo() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string table_name_{};

    std::vector<ColumnDef *> column_defs_{};
    std::vector<TableConstraint *> constraints_{};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
