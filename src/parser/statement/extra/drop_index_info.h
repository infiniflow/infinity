//
// Created by jinhai on 23-3-5.
//

#pragma once

#include "extra_ddl_info.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class DropIndexInfo : public ExtraDDLInfo {
public:
    explicit DropIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    [[nodiscard]] String ToString() const final;

    String schema_name_{"default"};
    String table_name_{};
    String index_name_{};
};

} // namespace infinity
