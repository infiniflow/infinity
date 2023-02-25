//
// Created by jinhai on 23-2-23.
//

#pragma once

#include "extra_ddl_info.h"

namespace infinity {

class CreateSchemaInfo : public ExtraDDLInfo {
public:
    explicit
    CreateSchemaInfo() : ExtraDDLInfo(DDLType::kSchema) {}

    [[nodiscard]] virtual String
    ToString() const final;

    String schema_name_;
};

}
