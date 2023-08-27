//
// Created by jinhai on 23-8-26.
//

#pragma once

#include "planner/logical_node.h"
#include "parser/statement/flush_statement.h"

namespace infinity {

class LogicalFlush : public LogicalNode {
public:
    explicit
    LogicalFlush(u64 node_id,
                 FlushType type)
                 : LogicalNode(node_id, LogicalNodeType::kFlush),
                 flush_type_(type) {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>> ();
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return MakeShared<Vector<SharedPtr<DataType>>>();
    }

    String
    ToString(i64 &space) final;

    inline String
    name() final {
        return "LogicalFlush";
    }

    [[nodiscard]] FlushType
    flush_type() const {
        return flush_type_;
    }

private:
    FlushType flush_type_ {FlushType::kData};
};

}
