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

export module physical_fusion;

import stl;
import base_table_ref;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import fusion_expression;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import logger;

namespace infinity {
struct DataBlock;

export enum class FusionMethod { kRRF, kWeightedSum, kMatchTensor };

export class PhysicalFusion final : public PhysicalOperator {
public:
    explicit PhysicalFusion(u64 id,
                            SharedPtr<BaseTableRef> base_table_ref,
                            UniquePtr<PhysicalOperator> left,
                            UniquePtr<PhysicalOperator> right,
                            Vector<UniquePtr<PhysicalOperator>> other_children,
                            SharedPtr<FusionExpression> fusion_expr,
                            SharedPtr<Vector<LoadMeta>> load_metas);
    ~PhysicalFusion() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    String ToString(i64 &space) const;

    Vector<UniquePtr<PhysicalOperator>> other_children_{};
    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<FusionExpression> fusion_expr_;

private:
    bool ExecuteFirstOp(QueryContext *query_context, FusionOperatorState *fusion_operator_state) const;
    bool ExecuteNotFirstOp(QueryContext *query_context, OperatorState *operator_state) const;
    // RRF and WeightedSum have multiple input sources, must be first fusion op
    void ExecuteRRFWeighted(const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                            Vector<UniquePtr<DataBlock>> &output_data_block_array) const;
    // MatchTensor may have multiple or single input source, can be first or not first fusion op
    void ExecuteMatchTensor(QueryContext *query_context,
                            const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                            Vector<UniquePtr<DataBlock>> &output_data_block_array) const;

    FusionMethod fusion_method_;
    SharedPtr<Vector<String>> output_names_;
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_;
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
