//
// Created by JinHai on 2022/8/4.
//

module;

import stl;
import expression_type;
import parser;

export module base_expression;

namespace infinity {

export enum class ExprSourceType {
    kInvalid,
    kAggregate,
    kGroupBy,
    kProjection,
    kBinding,
};

export struct SourcePosition {
    SourcePosition() = default;

    explicit SourcePosition(u64 bind_context_id, ExprSourceType source_type) : bind_context_id_(bind_context_id), source_type_(source_type) {}

    u64 bind_context_id_{u64_max};
    ExprSourceType source_type_{ExprSourceType::kInvalid};
    String binding_name_;
};

export class BaseExpression : public EnableSharedFromThis<BaseExpression> {
public:
    explicit BaseExpression(ExpressionType type, Vector<SharedPtr<BaseExpression>> arguments) : type_(type), arguments_(Move(arguments)){};

    explicit BaseExpression(ExpressionType type, Vector<SharedPtr<BaseExpression>> arguments, String alias)
        : type_(type), arguments_(Move(arguments)), alias_(Move(alias)){};

    virtual ~BaseExpression() = default;

    inline String Name() const {
        if (alias_.empty()) {
            return ToString();
        } else {
            return alias_;
        }
    }

    virtual DataType Type() const = 0;

    [[nodiscard]] inline ExpressionType type() const { return type_; }

    inline Vector<SharedPtr<BaseExpression>> &arguments() { return arguments_; }

    SourcePosition source_position_;
    String alias_;

protected:
    [[nodiscard]] virtual String ToString() const = 0;

    ExpressionType type_;
    Vector<SharedPtr<BaseExpression>> arguments_;
};

} // namespace infinity
