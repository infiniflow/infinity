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

class JoinReference: public BaseTableReference {

public:

    explicit
    JoinReference() : BaseTableReference(TableRefType::kJoin) {}
    ~JoinReference() override;

    String
    ToString() override;

    BaseTableReference* left_{nullptr};
    BaseTableReference* right_{nullptr};

    ParsedExpr* condition_{nullptr};
    JoinType join_type_{JoinType::kInner};

    Vector<char*>* using_columns_{nullptr};
};

}
