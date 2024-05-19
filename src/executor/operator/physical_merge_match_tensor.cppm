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

export module physical_merge_match_tensor;

import stl;
import query_context;
import operator_state;
import physical_operator;
import table_entry;
import match_tensor_expression;
import base_table_ref;
import data_type;

namespace infinity {
struct LoadMeta;

export class PhysicalMergeMatchTensor final : public PhysicalOperator {
public:
    PhysicalMergeMatchTensor(u64 id,
                             UniquePtr<PhysicalOperator> left,
                             u64 table_index,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchTensorExpression> match_tensor_expr,
                             u32 topn,
                             SharedPtr<Vector<LoadMeta>> load_metas);

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this); }

    SizeT TaskletCount() override;

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableEntry *table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline MatchTensorExpression *tensor_maxsim_expr() const { return match_tensor_expr_.get(); }

    [[nodiscard]] inline u32 GetTopN() const { return topn_; }

private:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<MatchTensorExpression> match_tensor_expr_;
    // extra options from tensor_maxsim_expr
    // inited by Init()
    u32 topn_ = 0;

    void ExecuteInner(QueryContext *query_context, MergeMatchTensorOperatorState *operator_state) const;
};

} // namespace infinity
