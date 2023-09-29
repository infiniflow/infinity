//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/parsed_expr/constant_expr.h"
#include "parser/parsed_expr/column_expr.h"
#include "parser/base_statement.h"
#include "parser/table_reference/base_table_reference.h"

namespace infinity {

class SelectStatement;

enum class SetOperatorType {
    kUnion,
    kUnionAll,
    kIntersect,
    kExcept
};

struct WithExpr {
    ~WithExpr() {
        if(select_ != nullptr) {
            delete select_;
        }
    }
    String alias_{};
    BaseStatement* select_{};
};

enum OrderType {
    kAsc,
    kDesc
};

String
ToString(OrderType type);

struct OrderByExpr {
    ~OrderByExpr() {
        delete expr_;
    }
    ParsedExpr* expr_{};
    OrderType type_{OrderType::kAsc};
};

class SelectStatement final : public BaseStatement {
public:
    SelectStatement() : BaseStatement(StatementType::kSelect) {}

    ~SelectStatement() final;

    [[nodiscard]] String
    ToString() const final;

    BaseTableReference* table_ref_{nullptr};
    Vector<ParsedExpr*>* select_list_{nullptr};
    bool select_distinct_{false};
    ParsedExpr* where_expr_{nullptr};
    Vector<ParsedExpr*>* group_by_list_{nullptr};
    ParsedExpr* having_expr_{nullptr};
    Vector<OrderByExpr*>* order_by_list{nullptr};
    ParsedExpr* limit_expr_{nullptr};
    ParsedExpr* offset_expr_{nullptr};
    Vector<WithExpr*>* with_exprs_{nullptr};

    SetOperatorType set_op_{SetOperatorType::kUnion};
    SelectStatement* nested_select_{nullptr};
};

}
