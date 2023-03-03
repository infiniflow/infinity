//
// Created by jinhai on 23-3-3.
//

#pragma once

#include "extra_ddl_info.h"
#include "parser/statement/select_statement.h"

namespace infinity {

class CreateViewInfo final : public ExtraDDLInfo {
public:
    explicit
    CreateViewInfo() : ExtraDDLInfo(DDLType::kView) {}

    ~CreateViewInfo() final;

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{};
    String view_name_{};

    Vector<String>* view_columns_{};
    SelectStatement* select_{nullptr};
};

}

