//
// Created by jinhai on 23-2-22.
//

#pragma once

#include <string>

namespace infinity {

enum class StatementType {
    kInvalidStmt, // unused
    kSelect,
    kCopy,
    kInsert,
    kUpdate,
    kDelete,
    kCreate,
    kDrop,
    kPrepare,
    kExecute,
    kAlter,
    kShow,
    kExplain,
    kFlush,
};

class BaseStatement {
public:
    explicit BaseStatement(StatementType type) : type_(type) {}

    virtual ~BaseStatement() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    [[nodiscard]] inline StatementType Type() const { return type_; }

    StatementType type_{StatementType::kInvalidStmt};
    size_t stmt_location_{0};
    size_t stmt_length_ = {0};
    std::string text_{};
};

} // namespace infinity