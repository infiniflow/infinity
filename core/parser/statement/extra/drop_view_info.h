//
// Created by jinhai on 23-3-5.
//

#pragma once

#include "extra_ddl_info.h"
#include "statement/statement_common.h"

namespace infinity {

class DropViewInfo : public ExtraDDLInfo {
public:
    explicit DropViewInfo() : ExtraDDLInfo(DDLType::kView) {}

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string view_name_{};
};

} // namespace infinity
