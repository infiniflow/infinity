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
import logical_node;
import parser;

export module logical_command;

namespace infinity {

export class LogicalCommand : public LogicalNode {
public:
    LogicalCommand(u64 node_id, UniquePtr<CommandInfo> command_info)
        : LogicalNode(node_id, LogicalNodeType::kCommand), command_info_(Move(command_info)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCommand"; }

    UniquePtr<CommandInfo> command_info_{};
};

} // namespace infinity
