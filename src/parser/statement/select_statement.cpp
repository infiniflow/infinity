// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

    if (highlight_list_ != nullptr) {
        for (auto &expr_ptr : *highlight_list_) {
            delete expr_ptr;
        }
        delete highlight_list_;
        highlight_list_ = nullptr;
    }

    if (search_expr_ != nullptr) {
        delete search_expr_;
        search_expr_ = nullptr;
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

std::string SelectStatement::ToString(OrderType type) {
    switch (type) {
        case OrderType::kAsc:
            return "ASC";
        case OrderType::kDesc:
            return "DESC";
    }
    ParserError("Unexpected order type");
}

} // namespace infinity
