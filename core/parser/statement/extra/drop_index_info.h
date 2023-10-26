//
// Created by jinhai on 23-3-5.
//

#pragma once

#include "extra_ddl_info.h"
#include "statement/statement_common.h"

namespace infinity {

class DropIndexInfo : public ExtraDDLInfo {
public:
    explicit DropIndexInfo() : ExtraDDLInfo(DDLType::kIndex) {}

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string table_name_{};
    std::string index_name_{};
};

} // namespace infinity
