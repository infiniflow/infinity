//
// Created by jinhai on 23-2-17.
//

module;

import stl;
import expression_type;
import base_expression;
import parser;

export module reference_expression;

namespace infinity {

export class ReferenceExpression : public BaseExpression {
public:
    static inline SharedPtr<ReferenceExpression> Make(DataType data_type, String table_name, String column_name, String alias, SizeT column_idx) {
        return MakeShared<ReferenceExpression>(Move(data_type), Move(table_name), Move(column_name), Move(alias), column_idx);
    }

public:
    explicit ReferenceExpression(DataType data_type, String table_name, String column_name, String alias, SizeT column_idx)
        : BaseExpression(ExpressionType::kReference, {}, Move(alias)), data_type_(Move(data_type)), table_name_(Move(table_name)),
          column_name_(Move(column_name)), column_index_(column_idx) {}

    inline SizeT column_index() const { return column_index_; }

    inline DataType Type() const override { return data_type_; };

    String ToString() const override;

private:
    DataType data_type_;

    String table_name_{};

    String column_name_{};

    SizeT column_index_{};
};

} // namespace infinity
