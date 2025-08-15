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

export module infinity_core:physical_fusion;

import :stl;
import :physical_operator;
import :base_table_ref;
import :fusion_expression;
import :operator_state;

import row_id;

namespace infinity {
// struct FusionOperatorState;
// class FusionExpression;
// struct DataBlock;

export enum class FusionMethod { kRRF, kWeightedSum, kMatchTensor };

export class PhysicalFusion final : public PhysicalOperator {
public:
    explicit PhysicalFusion(u64 id,
                            std::shared_ptr<BaseTableRef> base_table_ref,
                            std::unique_ptr<PhysicalOperator> left,
                            std::unique_ptr<PhysicalOperator> right,
                            std::vector<std::unique_ptr<PhysicalOperator>> other_children,
                            std::shared_ptr<FusionExpression> fusion_expr,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas);
    ~PhysicalFusion() override;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override { return output_names_; }

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override { return output_types_; }

    size_t TaskletCount() override { return 1; }

    void FillingTableRefs(std::unordered_map<size_t, std::shared_ptr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    std::string ToString(i64 &space) const;

    std::vector<std::unique_ptr<PhysicalOperator>> other_children_{};
    std::shared_ptr<BaseTableRef> base_table_ref_{};
    std::shared_ptr<FusionExpression> fusion_expr_;

private:
    bool ExecuteFirstOp(QueryContext *query_context, FusionOperatorState *fusion_operator_state) const;
    bool ExecuteNotFirstOp(QueryContext *query_context, OperatorState *operator_state) const;
    // RRF and WeightedSum have multiple input sources, must be first fusion op
    void ExecuteRRFWeighted(const std::map<u64, std::vector<std::unique_ptr<DataBlock>>> &input_data_blocks,
                            std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) const;
    // MatchTensor may have multiple or single input source, can be first or not first fusion op
    void ExecuteMatchTensor(QueryContext *query_context,
                            const std::map<u64, std::vector<std::unique_ptr<DataBlock>>> &input_data_blocks,
                            std::vector<std::unique_ptr<DataBlock>> &output_data_block_array) const;

    FusionMethod fusion_method_;
    std::shared_ptr<std::vector<std::string>> output_names_;
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_;
};

export struct MatchTensorRerankDoc {
    RowID row_id_;
    u64 from_input_data_block_id_;
    u32 from_block_idx_;
    u32 from_row_idx_;
    float score_ = 0.0f;
    MatchTensorRerankDoc(const RowID row_id, const u64 from_input_data_block_id, const u32 from_block_idx, const u32 from_row_idx)
        : row_id_(row_id), from_input_data_block_id_(from_input_data_block_id), from_block_idx_(from_block_idx), from_row_idx_(from_row_idx) {}
};

} // namespace infinity
