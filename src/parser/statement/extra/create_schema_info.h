//
// Created by jinhai on 23-2-23.
//

#pragma once

#include "extra_ddl_info.h"

namespace infinity {

class CreateSchemaInfo final : public ExtraDDLInfo {
public:
    explicit CreateSchemaInfo() : ExtraDDLInfo(DDLType::kSchema) {}

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_;
};

} // namespace infinity
