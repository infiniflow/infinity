//
// Created by jinhai on 23-3-13.
//

#include <utility>

#include "planner/logical_node.h"
#include "parser/statement/explain_statement.h"

namespace infinity {

class LogicalExplain : public LogicalNode {
public:
    explicit
    LogicalExplain(u64 node_id, ExplainType type)
    : LogicalNode(node_id, LogicalNodeType::kExplain), explain_type_(type) {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>>();
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalExplain";
    }

    [[nodiscard]] inline ExplainType
    explain_type() const {
        return explain_type_;
    }

    inline void
    SetText(const SharedPtr<Vector<SharedPtr<String>>>& texts) {
        texts_ = texts;
    }

    [[nodiscard]] inline const SharedPtr<Vector<SharedPtr<String>>>&
    TextArray() const {
        return texts_;
    }

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    SharedPtr<Vector<SharedPtr<String>>> texts_{nullptr};
};

}
