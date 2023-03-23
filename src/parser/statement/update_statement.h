//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/parsed_expr/parsed_expr.h"
#include "parser/base_statement.h"

namespace infinity {

struct UpdateExpr {
    inline
    ~UpdateExpr() {
        if(value != nullptr) {
            delete value;
            value = nullptr;
        }
    }

    String column_name;
    ParsedExpr* value;
};

class UpdateStatement final : public BaseStatement {
public:
    UpdateStatement() : BaseStatement(StatementType::kUpdate) {}

    ~UpdateStatement() final;

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{"default"};
    String table_name_{};

    ParsedExpr* where_expr_{nullptr};

    Vector<UpdateExpr*>* update_expr_array_{nullptr};
};

}
