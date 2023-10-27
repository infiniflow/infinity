//
// Created by JinHai on 2022/8/5.
//

module;

import column_binding;
import base_expression;
import parser;
import stl;

export module column_expression;

namespace infinity {

export class ColumnExpression final : public BaseExpression {
public:
    static inline SharedPtr<ColumnExpression>
    Make(DataType data_type, String table_name, u64 table_index, String column_name, i64 column_index, i64 depth) {
        return MakeShared<ColumnExpression>(Move(data_type), Move(table_name), table_index, Move(column_name), column_index, depth);
    }

public:
    ColumnExpression(DataType data_type, String table_name, u64 table_index, String column_name, i64 column_index, i64 depth);

    inline DataType Type() const override { return data_type_; };

    const String &table_name() const { return table_name_; }

    //    inline u64
    //    table_index() const {
    //        return binding_.table_idx;
    //    }

    const String &column_name() const { return column_name_; }

    inline void SetBinding(SizeT table_idx, SizeT column_idx) {
        binding_.table_idx = table_idx;
        binding_.column_idx = column_idx;
    }

    inline ColumnBinding binding() const { return binding_; }

    inline bool IsCorrelated() const { return depth_ > 0; }

    inline void SetDepth(i64 depth) { depth_ = depth; }

    inline i64 depth() const { return depth_; }

private:
    String ToString() const final;

private:
    DataType data_type_;

    ColumnBinding binding_;

    String table_name_{};

    String column_name_{};

    i64 depth_;
};

} // namespace infinity