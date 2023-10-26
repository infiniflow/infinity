//
// Created by jinhai on 23-2-22.
//

#include "select_statement.h"
#include "parser_assert.h"

namespace infinity {

SelectStatement::~SelectStatement() {
    if (table_ref_ != nullptr) {
        delete table_ref_;
        table_ref_ = nullptr;
    }

    if (select_list_ != nullptr) {
        for (auto &expr_ptr : *select_list_) {
            delete expr_ptr;
        }
        delete select_list_;
        select_list_ = nullptr;
    }

    if (where_expr_ != nullptr) {
        delete where_expr_;
        where_expr_ = nullptr;
    }

    if (group_by_list_ != nullptr) {
        for (auto &expr_ptr : *group_by_list_) {
            delete expr_ptr;
        }
        delete group_by_list_;
        group_by_list_ = nullptr;
    }

    if (having_expr_ != nullptr) {
        delete having_expr_;
        having_expr_ = nullptr;
    }

    if (order_by_list != nullptr) {
        for (auto &expr_ptr : *order_by_list) {
            delete expr_ptr;
        }
        delete order_by_list;
        order_by_list = nullptr;
    }

    if (limit_expr_ != nullptr) {
        delete limit_expr_;
        limit_expr_ = nullptr;
    }

    if (offset_expr_ != nullptr) {
        delete offset_expr_;
        offset_expr_ = nullptr;
    }

    if (with_exprs_ != nullptr) {
        for (auto &with_expr_ptr : *with_exprs_) {
            delete with_expr_ptr;
        }
        delete with_exprs_;
        with_exprs_ = nullptr;
    }

    if (nested_select_ != nullptr) {
        delete nested_select_;
        nested_select_ = nullptr;
    }
}

std::string SelectStatement::ToString() const { return "Select Statement"; }

std::string ToString(OrderType type) {
    switch (type) {
        case OrderType::kAsc:
            return "ASC";
        case OrderType::kDesc:
            return "DESC";
    }
    ParserError("Unexpected order type");
}

} // namespace infinity
