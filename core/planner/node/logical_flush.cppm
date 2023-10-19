//
// Created by jinhai on 23-8-26.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_flush;

namespace infinity {

export class LogicalFlush : public LogicalNode {
public:
    explicit LogicalFlush(u64 node_id, FlushType type) : LogicalNode(node_id, LogicalNodeType::kFlush), flush_type_(type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalFlush"; }

    [[nodiscard]] FlushType flush_type() const { return flush_type_; }

private:
    FlushType flush_type_{FlushType::kData};
};

} // namespace infinity
