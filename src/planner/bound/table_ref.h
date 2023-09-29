//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "parser/table_reference/base_table_reference.h"

namespace infinity {

class TableRef {
public:
    explicit
    TableRef(TableRefType type,
             String alias) :
            type_(type),
            alias_(std::move(alias)) {}

    TableRefType type_{TableRefType::kTable};
    String alias_{};
};

}
