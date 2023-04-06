//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"
#include "planner/column_binding.h"

namespace infinity {


class ColumnExpression final : public BaseExpression {
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

    inline DataType
    Type() const override {
        return data_type_;
    };

    const String&
    table_name() const {
        return table_name_;
    }

//    inline u64
//    table_index() const {
//        return binding_.table_idx;
//    }

    const String&
    column_name() const {
        return column_name_;
    }

//    inline i64
//    column_index() const {
//        return binding_.column_idx;
//    }

    inline ColumnBinding
    binding() const {
        return binding_;
    }

    inline bool
    IsCorrelated() const {
        return depth_ > 0;
    }

private:
    String
    ToString() const final;

private:

    DataType data_type_;

    ColumnBinding binding_;

    String table_name_{};

    String column_name_{};

    i64 depth_;
};


}