//
// Created by JinHai on 2022/7/23.
//

module;

import stl;
import logical_node_type;
import column_binding;

export module logical_node;

namespace infinity {

class DataType;

export class LogicalNode {
public:
    inline explicit LogicalNode(u64 node_id, LogicalNodeType node_type) : node_id_(node_id), operator_type_(node_type) {}

    virtual ~LogicalNode() = default;

    [[nodiscard]] virtual Vector<ColumnBinding> GetColumnBindings() const = 0;

    virtual SharedPtr<Vector<String>> GetOutputNames() const = 0;

    virtual SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const = 0;

    [[nodiscard]] inline SharedPtr<LogicalNode> &left_node() { return left_node_; }

    [[nodiscard]] inline const SharedPtr<LogicalNode> &left_node() const { return left_node_; }

    [[nodiscard]] inline SharedPtr<LogicalNode> &right_node() { return right_node_; }

    [[nodiscard]] inline const SharedPtr<LogicalNode> &right_node() const { return right_node_; }

    void set_left_node(const SharedPtr<LogicalNode> &left) { left_node_ = left; }

    void set_right_node(const SharedPtr<LogicalNode> &right) { right_node_ = right; };

    [[nodiscard]] u64 node_id() const { return node_id_; }

    void set_node_id(u64 node_id) { node_id_ = node_id; }

    virtual String ToString(i64 &space) const = 0;

    virtual String name() = 0;

    [[nodiscard]] LogicalNodeType operator_type() const { return operator_type_; }

protected:
    LogicalNodeType operator_type_ = LogicalNodeType::kInvalid;

    SharedPtr<LogicalNode> left_node_{};
    SharedPtr<LogicalNode> right_node_{};
    u64 node_id_{};
};

} // namespace infinity
