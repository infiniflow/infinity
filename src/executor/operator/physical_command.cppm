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

export module infinity_core:physical_command;

import :physical_operator;
import :physical_operator_type;
import :operator_state;
import :load_meta;
import :infinity_exception;
import :logger;

import command_statement;
import data_type;
import internal_types;

namespace infinity {

export class PhysicalCommand final : public PhysicalOperator {
public:
    explicit PhysicalCommand(u64 id,
                             std::shared_ptr<CommandInfo> command_info,
                             std::shared_ptr<std::vector<std::string>> output_names,
                             std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCommand, nullptr, nullptr, id, load_metas), command_info_(std::move(command_info)),
          output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

    ~PhysicalCommand() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const override { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const std::shared_ptr<CommandInfo> command_info_{};

    const std::shared_ptr<std::vector<std::string>> output_names_{};
    const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};
} // namespace infinity