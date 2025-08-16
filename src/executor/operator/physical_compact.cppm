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

export module infinity_core:physical_compact;

import :physical_operator;
import :physical_operator_type;
import :base_table_ref;
import :query_context;
import :operator_state;
import :load_meta;

import data_type;
import compact_statement;

namespace infinity {

export class PhysicalCompact : public PhysicalOperator {
public:
    PhysicalCompact(u64 id,
                    std::shared_ptr<BaseTableRef> base_table_ref,
                    CompactStatementType compact_type,
                    std::shared_ptr<std::vector<std::string>> output_names,
                    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                    std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCompact, nullptr, nullptr, id, load_metas), base_table_ref_(base_table_ref),
          compact_type_(compact_type), output_names_(output_names), output_types_(output_types) {}

    ~PhysicalCompact() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    size_t TaskletCount() override { return 1; }

    // std::vector<std::vector<std::vector<SegmentEntry *>>> PlanCompact(size_t parallel_count);

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override { return output_names_; }

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    std::shared_ptr<BaseTableRef> base_table_ref_;

private:
    CompactStatementType compact_type_;
    // std::vector<std::vector<SegmentEntry *>> compactible_segments_group_;

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity