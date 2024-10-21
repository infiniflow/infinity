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
import base_expression;
import logical_match_tensor_scan;

namespace infinity {

struct LoadMeta;
class ResultCacheManager;
class DataBlock;

export class PhysicalMergeMatchTensor final : public PhysicalOperator {
public:
    PhysicalMergeMatchTensor(u64 id,
                             UniquePtr<PhysicalOperator> left,
                             u64 table_index,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<MatchTensorExpression> match_tensor_expr,
                             SharedPtr<BaseExpression> filter_expression,
                             u32 topn,
                             const SharedPtr<MatchTensorScanIndexOptions> &index_options,
                             SharedPtr<Vector<LoadMeta>> load_metas,
                             bool cache_result);

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

    [[nodiscard]] inline const BaseTableRef *base_table_ref() const { return base_table_ref_.get(); }

    [[nodiscard]] inline const SharedPtr<MatchTensorExpression> &match_tensor_expr() const { return match_tensor_expr_; }

    [[nodiscard]] inline const SharedPtr<BaseExpression> &filter_expression() const { return filter_expression_; }

    [[nodiscard]] inline u32 GetTopN() const { return topn_; }

    const SharedPtr<MatchTensorScanIndexOptions> &index_options() const { return index_options_; }

    void AddCache(QueryContext *query_context, ResultCacheManager *cache_mgr, const Vector<UniquePtr<DataBlock>> &output_data_blocks) const;

private:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<MatchTensorExpression> match_tensor_expr_;
    SharedPtr<BaseExpression> filter_expression_{};
    // extra options
    // inited by Init()
    u32 topn_ = 0;
    SharedPtr<MatchTensorScanIndexOptions> index_options_;

    void ExecuteInner(QueryContext *query_context, MergeMatchTensorOperatorState *operator_state) const;
};

} // namespace infinity
