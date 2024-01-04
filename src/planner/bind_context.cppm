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
import parser;
import column_binding;
import third_party;

import infinity_exception;
import column_expression;
import column_identifer;
import binding;
import base_expression;
import block_index;
import catalog;

export module bind_context;

namespace infinity {

class ExpressionBinder;

export struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(String alias, SelectStatement *select_stmt, HashSet<String> masked_name_set)
        : alias_(Move(alias)), select_statement_(select_stmt), masked_name_set_(Move(masked_name_set)) {}

    String alias_;
    SelectStatement *select_statement_;
    HashSet<String> masked_name_set_;
};

export class BindContext {
public:
    static inline SharedPtr<BindContext> Make(const SharedPtr<BindContext> &parent) { return MakeShared<BindContext>(parent); }

    static inline SharedPtr<BindContext> Make(BindContext *parent) { return MakeShared<BindContext>(parent); }

public:
    explicit BindContext(const SharedPtr<BindContext> &parent) : parent_(parent.get()) { binding_context_id_ = GenerateBindingContextIndex(); }

    explicit BindContext(BindContext *parent) : parent_(parent) { binding_context_id_ = GenerateBindingContextIndex(); }

    virtual ~BindContext();

    void Destroy();

    // Parent bind context
    BindContext *parent_{nullptr};

    // Left and right child bind context
    SharedPtr<BindContext> left_child_;
    SharedPtr<BindContext> right_child_;

    // CTE from CTE alias -> CTE statement
    HashMap<String, SharedPtr<CommonTableExpressionInfo>> CTE_map_;

    // Binding, all bindings include subquery, cte, view, table ...
    HashMap<String, SharedPtr<Binding>> binding_by_name_;
    HashMap<String, Vector<String>> binding_names_by_column_;

    // Binding Table
    Vector<String> table_names_;
    HashMap<String, u64> table_name2table_index_;
    HashMap<u64, String> table_table_index2table_name_;

    // Select list
    Vector<ParsedExpr *> select_expression_; // ParsedExpr won't be free here.

    // Following will be initialized at step 5 of bind select statement
    HashMap<String, i64> select_alias2index_;     // Some select expr has alias, alias_name -> index
    HashMap<String, i64> select_expr_name2index_; // Select expr name -> index

    HashMap<i64, i64> select_index_to_group_by_index_;  // select index -> group by index
    HashMap<i64, i64> select_index_to_aggregate_index_; // select index -> aggregate index

    // Bound expr in group by list
    u64 group_by_table_index_{0};
    String group_by_table_name_{};
    //    Vector<String> group_names_;
    Vector<SharedPtr<BaseExpression>> group_exprs_;
    //    HashMap<String, SharedPtr<BaseExpression>> group_by_name_;
    HashMap<String, i64> group_index_by_name_;

    // Bound aggregate function expr
    u64 aggregate_table_index_{0};
    String aggregate_table_name_{};
    Vector<SharedPtr<BaseExpression>> aggregate_exprs_;
    //    HashMap<String, SharedPtr<BaseExpression>> aggregate_by_name_;
    HashMap<String, i64> aggregate_index_by_name_;

    // Bound expr in select list
    u64 project_table_index_{0};
    String project_table_name_{};
    Vector<SharedPtr<BaseExpression>> project_exprs_;
    HashMap<String, i64> project_index_by_name_;
    //    Vector<String> project_names_;
    //    HashMap<String, SharedPtr<BaseExpression>> project_by_name_;
    u64 result_index_{0};

    u64 knn_table_index_{};

    // Bound CTE
    HashSet<SharedPtr<CommonTableExpressionInfo>> bound_cte_set_;

    // Bound View
    HashSet<String> bound_view_set_;

    // Bound Table (base table)
    HashSet<String> bound_table_set_;

    // Bound subquery (TODO: How to get the subquery name?)
    HashSet<String> bound_subquery_set_;

    // Bound correlated column
    Vector<SharedPtr<ColumnExpression>> correlated_column_exprs_;
    HashMap<ColumnBinding, SizeT> correlated_column_map_;

    // An sequence id
    u64 binding_context_id_{0};

    bool single_row = false;

    bool allow_distance = false;
    bool allow_score = false;

public:
    void AddLeftChild(const SharedPtr<BindContext> &left_child);

    void AddRightChild(const SharedPtr<BindContext> &right_child);

    u64 GetNewLogicalNodeId();

    u64 GenerateBindingContextIndex();

    u64 GenerateTableIndex();

    [[nodiscard]] SharedPtr<CommonTableExpressionInfo> GetCTE(const String &name) const;

    [[nodiscard]] bool IsCTEBound(const SharedPtr<CommonTableExpressionInfo> &cte) const;

    void BoundCTE(const SharedPtr<CommonTableExpressionInfo> &cte) { bound_cte_set_.insert(cte); }

    [[nodiscard]] bool IsViewBound(const String &view_name) const;

    void BoundView(const String &view_name) { bound_view_set_.insert(view_name); }

    [[nodiscard]] bool IsTableBound(const String &table_name) const;

    void BoundTable(const String &table_name) { bound_table_set_.insert(table_name); }

    void BoundSearch(ParsedExpr *expr) {
        if (expr == nullptr) {
            return;
        }
        auto search_expr = (SearchExpr *)expr;

        allow_distance = !search_expr->knn_exprs_.empty() && search_expr->fusion_expr_ == nullptr;
        allow_score = !search_expr->match_exprs_.empty() || search_expr->fusion_expr_ != nullptr;
    }

    void AddSubqueryBinding(const String &name,
                            u64 table_index,
                            SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                            SharedPtr<Vector<String>> column_names);

    void
    AddCTEBinding(const String &name, u64 table_index, SharedPtr<Vector<SharedPtr<DataType>>> column_types, SharedPtr<Vector<String>> column_names);

    void
    AddViewBinding(const String &name, u64 table_index, SharedPtr<Vector<SharedPtr<DataType>>> column_types, SharedPtr<Vector<String>> column_names);

    void AddTableBinding(const String &name,
                         u64 table_index,
                         TableEntry *table_collection_entry_ptr,
                         SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                         SharedPtr<Vector<String>> column_names,
                         SharedPtr<BlockIndex> block_index);

    // Merge input bind context into this bind context
    void AddBindContext(const SharedPtr<BindContext> &other_ptr);

    SharedPtr<ColumnExpression> ResolveColumnId(const ColumnIdentifier &column_identifier, i64 depth);

    void AddCorrelatedColumnExpr(const SharedPtr<ColumnExpression> &correlated_column);

    inline bool HasCorrelatedColumn() const { return !correlated_column_exprs_.empty(); }

    const Binding *GetBindingFromCurrentOrParentByName(const String &binding_name) const;

private:
    void AddBinding(const SharedPtr<Binding> &binding);

private:
    SizeT next_logical_node_id_{1};
    SizeT next_bind_context_index_{1};
    SizeT next_table_index_{1};

public:
    // !!! TODO: Below need to be refactored !!!

public:
    // Binder, different binder have different expression build behavior.
    SharedPtr<ExpressionBinder> expression_binder_{nullptr};
};

} // namespace infinity
