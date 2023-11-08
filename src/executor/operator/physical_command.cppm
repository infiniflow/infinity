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

import physical_operator;
import physical_operator_type;
import query_context;
import parser;
import operator_state;

export module physical_command;

namespace infinity {

export class PhysicalCommand final : public PhysicalOperator {
public:
    explicit PhysicalCommand(u64 id,
                             SharedPtr<CommandInfo> command_info,
                             SharedPtr<Vector<String>> output_names,
                             SharedPtr<Vector<SharedPtr<DataType>>> output_types)
        : PhysicalOperator(PhysicalOperatorType::kCommand, nullptr, nullptr, id), command_info_(Move(command_info)),
          output_names_(Move(output_names)), output_types_(Move(output_types)) {}

    ~PhysicalCommand() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) override;

    inline SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const SharedPtr<CommandInfo> command_info_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity