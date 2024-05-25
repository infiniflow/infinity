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

namespace infinity {
struct DataBlock;

export class PhysicalFusion final : public PhysicalOperator {
public:
    explicit PhysicalFusion(u64 id,
                            SharedPtr<BaseTableRef> base_table_ref,
                            UniquePtr<PhysicalOperator> left,
                            UniquePtr<PhysicalOperator> right,
                            SharedPtr<FusionExpression> fusion_expr,
                            SharedPtr<Vector<LoadMeta>> load_metas);
    ~PhysicalFusion() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

    SizeT TaskletCount() override {
        UnrecoverableError("Not implement: TaskletCount not Implement");
        return 0;
    }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    String ToString(i64 &space) const;

    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<FusionExpression> fusion_expr_;

private:
    bool ExecuteFirstOp(QueryContext *query_context, FusionOperatorState *fusion_operator_state) const;
    bool ExecuteNotFirstOp(QueryContext *query_context, OperatorState *operator_state) const;
    // RRF has multiple input source, must be first op
    void ExecuteRRF(const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks, Vector<UniquePtr<DataBlock>> &output_data_block_array) const;
    // MatchTensor may have multiple or single input source, can be first or not first op
    void ExecuteMatchTensor(QueryContext *query_context,
                            const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                            Vector<UniquePtr<DataBlock>> &output_data_block_array) const;

    String to_lower_method_;
    SharedPtr<Vector<String>> output_names_;
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_;
};

} // namespace infinity
