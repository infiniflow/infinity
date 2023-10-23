//
// Created by JinHai on 2022/8/10.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_view_scan;

namespace infinity {

class View;

export class LogicalViewScan : public LogicalNode {

public:
    explicit LogicalViewScan(u64 node_id, SharedPtr<View> view_ptr)
        : LogicalNode(node_id, LogicalNodeType::kViewScan), view_ptr_(Move(view_ptr)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] SharedPtr<View> view_ptr() const { return view_ptr_; }

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalViewScan"; }

private:
    SharedPtr<View> view_ptr_{nullptr};
};

} // namespace infinity
