//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "common/types/internal_types.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"
#include "parser/parsed_expr/parsed_expr.h"

namespace infinity {

class TableAlias {
public:
    ~TableAlias();

    char *alias_{};
    Vector<String>* column_alias_array_{nullptr};
};

enum class TableRefType {
    kCrossProduct,
    kJoin,
    kTable,
    kSubquery,
    kDummy,
};

class BaseTableReference {
public:
    explicit
    BaseTableReference(TableRefType type) : type_(type) {}

    virtual
    ~BaseTableReference();

    virtual String
    ToString() = 0;

    TableRefType type_{TableRefType::kTable};
    TableAlias* alias_{nullptr};
};

}

