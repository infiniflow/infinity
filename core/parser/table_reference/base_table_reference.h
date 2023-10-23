//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "expr/expr.h"
#include <string>
#include <vector>

namespace infinity {

class TableAlias {
public:
    ~TableAlias();

    char *alias_{};
    std::vector<std::string> *column_alias_array_{nullptr};
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
    explicit BaseTableReference(TableRefType type) : type_(type) {}

    virtual ~BaseTableReference();

    virtual std::string ToString() = 0;

    TableRefType type_{TableRefType::kTable};
    TableAlias *alias_{nullptr};
};

} // namespace infinity
