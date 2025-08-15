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

export module infinity_core:physical_create_view;

import :stl;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :index_base;
import :load_meta;
import :infinity_exception;
import :logger;

import internal_types;
import create_view_info;
import data_type;

namespace infinity {

export class PhysicalCreateView final : public PhysicalOperator {
public:
    explicit inline PhysicalCreateView(u64 id,
                                       std::shared_ptr<std::vector<std::string>> names_ptr,
                                       std::shared_ptr<std::vector<std::shared_ptr<DataType>>> types_ptr,
                                       std::shared_ptr<CreateViewInfo> create_view_info,
                                       std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCreateView, nullptr, nullptr, id, load_metas), create_view_info_(std::move(create_view_info)),
          output_names_(std::move(names_ptr)), output_types_(std::move(types_ptr)) {}

    ~PhysicalCreateView() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline const std::shared_ptr<CreateViewInfo> &bound_select_statement() const { return create_view_info_; };

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline const std::shared_ptr<CreateViewInfo> &create_view_info() const { return create_view_info_; }

private:
    std::shared_ptr<CreateViewInfo> create_view_info_{nullptr};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
