//
// Created by jinhai on 23-2-24.
//

#pragma once

#include "extra_ddl_info.h"

namespace infinity {

class DropSchemaInfo : public ExtraDDLInfo {
public:
    explicit DropSchemaInfo() : ExtraDDLInfo(DDLType::kSchema) {}

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_;
};

} // namespace infinity
