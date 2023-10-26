//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"
#include "expr/expr.h"
#include <vector>

namespace infinity {

struct UpdateExpr {
    inline ~UpdateExpr() {
        if (value != nullptr) {
            delete value;
            value = nullptr;
        }
    }

    std::string column_name;
    ParsedExpr *value;
};

class UpdateStatement final : public BaseStatement {
public:
    UpdateStatement() : BaseStatement(StatementType::kUpdate) {}

    ~UpdateStatement() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string table_name_{};

    ParsedExpr *where_expr_{nullptr};

    std::vector<UpdateExpr *> *update_expr_array_{nullptr};
};

} // namespace infinity
