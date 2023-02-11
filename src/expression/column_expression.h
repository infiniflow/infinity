//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {


class ColumnExpression : public BaseExpression {
public:
    static inline SharedPtr<ColumnExpression>
    Make(DataType data_type,
         String table_name,
         u64 table_index,
         String column_name,
         i64 column_index,
         i64 depth) {
        return MakeShared<ColumnExpression>(std::move(data_type),
                                            std::move(table_name),
                                            table_index,
                                            std::move(column_name),
                                            column_index,
                                            depth);
    }

public:
    ColumnExpression(DataType data_type,
                     String table_name,
                     u64 table_index,
                     String column_name,
                     i64 column_index,
                     i64 depth);

    DataType
    Type() const override {
        return data_type_;
    };

    String
    ToString() const override;

    const String&
    table_name() const {
        return table_name_;
    }

    u64
    table_index() const {
        return table_index_;
    }

    const String&
    column_name() const {
        return column_name_;
    }

    i64
    column_index() const {
        return column_index_;
    }

private:

    DataType data_type_;

    String table_name_{};

    u64 table_index_{};

    String column_name_{};

    i64 column_index_{};

    i64 depth_;
};


}