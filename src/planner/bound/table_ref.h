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
             u64 table_index,
             String alias):
             type_(type),
             table_index_(table_index),
             alias_(std::move(alias))
             {}

    TableRefType type_{TableRefType::kTable};
    String alias_{};
    u64 table_index_{};
};

}
