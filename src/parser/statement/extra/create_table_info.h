//
// Created by jinhai on 23-2-25.
//

#pragma once

#include <utility>

#include "common/types/data_type.h"
#include "common/types/info/bitmap_info.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/embedding_info.h"
#include "common/types/info/varchar_info.h"
#include "extra_ddl_info.h"
#include "parser/statement/select_statement.h"
#include "parser/statement/statement_common.h"
#include "storage/meta/definition/column_def.h"

namespace infinity {

class CreateTableInfo final : public ExtraDDLInfo {
public:
    explicit CreateTableInfo() : ExtraDDLInfo(DDLType::kTable) {}

    ~CreateTableInfo() final;

    [[nodiscard]] String ToString() const final;

    String schema_name_{"default"};
    String table_name_{};

    Vector<ColumnDef *> column_defs_{};
    Vector<TableConstraint *> constraints_{};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
