//
// Created by jinhai on 23-2-28.
//

#pragma once

#include "base_table_reference.h"

namespace infinity {

enum class JoinType {
    kInner,
    kFull,
    kLeft,
    kRight,
    kCross,
    kNatural,
    kSemi,
    kMark,
    kAnti,
};

std::string ToString(JoinType type);

class JoinReference : public BaseTableReference {

public:
    explicit JoinReference() : BaseTableReference(TableRefType::kJoin) {}

    ~JoinReference() override;

    std::string ToString() override;

    BaseTableReference *left_{nullptr};
    BaseTableReference *right_{nullptr};

    ParsedExpr *condition_{nullptr};
    JoinType join_type_{JoinType::kInner};

    std::vector<char *> *using_columns_{nullptr};
};

} // namespace infinity
