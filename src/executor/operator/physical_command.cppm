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

export module physical_command;

import stl;
import internal_types;
import physical_operator;
import physical_operator_type;
import query_context;

import operator_state;
import load_meta;
import infinity_exception;
import table_entry;
import command_statement;
import data_type;

namespace infinity {

export class PhysicalCommand final : public PhysicalOperator {
public:
    explicit PhysicalCommand(u64 id,
                             SharedPtr<CommandInfo> command_info,
                             SharedPtr<Vector<String>> output_names,
                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                             SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCommand, nullptr, nullptr, id, load_metas), command_info_(std::move(command_info)),
          output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

    ~PhysicalCommand() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SizeT TaskletCount() override {
        UnrecoverableError("Not implement: TaskletCount not Implement");
        return 0;
    }

    inline SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const SharedPtr<CommandInfo> command_info_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity