//
// Created by JinHai on 2022/8/4.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "common/types/expression_type.h"

#include <memory>
#include <utility>
#include <vector>

namespace infinity {

enum class ExprSourceType {
    kInvalid,
    kAggregate,
    kGroupBy,
    kProjection,
    kBinding,
};

struct SourcePosition {
    SourcePosition() = default;

    explicit SourcePosition(u64 bind_context_id, ExprSourceType source_type)
            : bind_context_id_(bind_context_id), source_type_(source_type) {}

    u64 bind_context_id_{std::numeric_limits<u64>::max()};
    ExprSourceType source_type_{ExprSourceType::kInvalid};
    String binding_name_;
};

class BaseExpression : public std::enable_shared_from_this<BaseExpression> {
public:
    explicit
    BaseExpression(ExpressionType type,
                   Vector<SharedPtr<BaseExpression>> arguments)
            : type_(type), arguments_(std::move(arguments)) {};

    explicit
    BaseExpression(ExpressionType type,
                   Vector<SharedPtr<BaseExpression>> arguments,
                   String alias)
            : type_(type),
              arguments_(std::move(arguments)),
              alias_(std::move(alias)) {};

    virtual
    ~BaseExpression() = default;

    inline String
    Name() const {
        if(alias_.empty()) {
            return ToString();
        } else {
            return alias_;
        }
    }

    virtual DataType
    Type() const = 0;

    [[nodiscard]] inline ExpressionType
    type() const {
        return type_;
    }

    inline Vector<SharedPtr<BaseExpression>>&
    arguments() {
        return arguments_;
    }

    SourcePosition source_position_;
    String alias_;

protected:
    [[nodiscard]] virtual String
    ToString() const = 0;

    ExpressionType type_;
    Vector<SharedPtr<BaseExpression>> arguments_;
};

}
