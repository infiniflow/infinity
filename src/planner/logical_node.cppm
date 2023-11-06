// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

public:
    template <class TARGET>
    TARGET &Cast() {
        // FIXME: Assert Check
        return reinterpret_cast<TARGET &>(*this);
    }

    template <class TARGET>
    const TARGET &Cast() const {
        // FIXME: Assert Check
        return reinterpret_cast<const TARGET &>(*this);
    }
};

} // namespace infinity
