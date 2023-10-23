//
// Created by JinHai on 2022/9/25.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_cross_product;

namespace infinity {

export class LogicalCrossProduct : public LogicalNode {
public:
    explicit LogicalCrossProduct(u64 node_id, String alias, const SharedPtr<LogicalNode> &left, const SharedPtr<LogicalNode> &right);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCrossProduct"; }

    String alias_{};
};

} // namespace infinity
