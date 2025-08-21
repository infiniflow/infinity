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

export module infinity_core:bind_context;

import :column_binding;
import :infinity_exception;
import :column_expression;
import :column_identifier;
import :binding;
import :base_expression;
import :meta_info;

import third_party;

import internal_types;
import select_statement;
import parsed_expr;
import search_expr;
import data_type;
import global_resource_usage;

namespace infinity {

struct BlockIndex;
export class ExpressionBinder;

export struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(std::string alias, SelectStatement *select_stmt, std::unordered_set<std::string> masked_name_set)
        : alias_(std::move(alias)), select_statement_(select_stmt), masked_name_set_(std::move(masked_name_set)) {}

    std::string alias_;
    SelectStatement *select_statement_;
    std::unordered_set<std::string> masked_name_set_;
};

export class BindContext {
public:
    static inline std::shared_ptr<BindContext> Make(const std::shared_ptr<BindContext> &parent) { return std::make_shared<BindContext>(parent); }

    static inline std::shared_ptr<BindContext> Make(BindContext *parent) { return std::make_shared<BindContext>(parent); }

public:
    explicit BindContext(const std::shared_ptr<BindContext> &parent) : parent_(parent.get()) {
        binding_context_id_ = GenerateBindingContextIndex();
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BindContext");
#endif
    }

    explicit BindContext(BindContext *parent) : parent_(parent) {
        binding_context_id_ = GenerateBindingContextIndex();
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BindContext");
#endif
    }

    virtual ~BindContext();

    void Destroy();

    // Parent bind context
    BindContext *parent_{nullptr};

    // Left and right child bind context
    std::shared_ptr<BindContext> left_child_;
    std::shared_ptr<BindContext> right_child_;

    // CTE from CTE alias -> CTE statement
    std::unordered_map<std::string, std::shared_ptr<CommonTableExpressionInfo>> CTE_map_;

    // Binding, all bindings include subquery, cte, view, table ...
    std::unordered_map<std::string, std::shared_ptr<Binding>> binding_by_name_;
    std::unordered_map<std::string, std::vector<std::string>> binding_names_by_column_;

    // Binding Table
    std::vector<std::string> table_names_;
    std::unordered_map<std::string, u64> table_name2table_index_;
    std::unordered_map<u64, std::string> table_table_index2table_name_;

    // Select list
    std::vector<ParsedExpr *> select_expression_; // ParsedExpr won't be free here.

    // Following will be initialized at step 5 of bind select statement
    std::unordered_map<std::string, i64> select_alias2index_;     // Some select expr has alias, alias_name -> index
    std::unordered_map<std::string, i64> select_expr_name2index_; // Select expr name -> index

    std::unordered_map<i64, i64> select_index_to_group_by_index_;  // select index -> group by index
    std::unordered_map<i64, i64> select_index_to_aggregate_index_; // select index -> aggregate index

    // Bound expr in group by list
    u64 group_by_table_index_{0};
    std::string group_by_table_name_{};
    //    std::vector<std::string> group_names_;
    std::vector<std::shared_ptr<BaseExpression>> group_exprs_;
    //    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> group_by_name_;
    std::unordered_map<std::string, i64> group_index_by_name_;

    u64 unnest_table_index_{0};
    std::string unnest_table_name_{};
    std::vector<std::shared_ptr<BaseExpression>> unnest_exprs_;
    std::unordered_map<std::string, i64> unnest_index_by_name_;

    // Bound aggregate function expr
    u64 aggregate_table_index_{0};
    std::string aggregate_table_name_{};
    std::vector<std::shared_ptr<BaseExpression>> aggregate_exprs_;
    //    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> aggregate_by_name_;
    std::unordered_map<std::string, i64> aggregate_index_by_name_;

    // Bound expr in select list
    u64 project_table_index_{0};
    std::string project_table_name_{};
    std::vector<std::shared_ptr<BaseExpression>> project_exprs_;
    std::unordered_map<std::string, i64> project_index_by_name_;
    //    std::vector<std::string> project_names_;
    //    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> project_by_name_;
    u64 result_index_{0};

    u64 knn_table_index_{};

    // Bound CTE
    std::unordered_set<std::shared_ptr<CommonTableExpressionInfo>> bound_cte_set_{};

    // Bound View
    std::unordered_set<std::string> bound_view_set_{};

    // Bound Table (base table)
    std::unordered_set<std::string> bound_table_set_{};

    // Bound subquery (TODO: How to get the subquery name?)
    std::unordered_set<std::string> bound_subquery_set_;

    // Bound correlated column
    std::vector<std::shared_ptr<ColumnExpression>> correlated_column_exprs_;
    std::unordered_map<ColumnBinding, size_t> correlated_column_map_;

    // An sequence id
    u64 binding_context_id_{0};

    bool single_row = false;

    bool allow_distance = false;
    bool allow_similarity = false;
    bool allow_score = false;

public:
    void AddLeftChild(const std::shared_ptr<BindContext> &left_child);

    void AddRightChild(const std::shared_ptr<BindContext> &right_child);

    u64 GetNewLogicalNodeId();

    u64 GenerateBindingContextIndex();

    u64 GenerateTableIndex();

    [[nodiscard]] std::shared_ptr<CommonTableExpressionInfo> GetCTE(const std::string &name) const;

    [[nodiscard]] bool IsCTEBound(const std::shared_ptr<CommonTableExpressionInfo> &cte) const;

    void BoundCTE(const std::shared_ptr<CommonTableExpressionInfo> &cte) { bound_cte_set_.insert(cte); }

    [[nodiscard]] bool IsViewBound(const std::string &view_name) const;

    void BoundView(const std::string &view_name) { bound_view_set_.insert(view_name); }

    [[nodiscard]] bool IsTableBound(const std::string &table_name) const;

    void BoundTable(const std::string &table_name) { bound_table_set_.insert(table_name); }

    void BoundSearch(ParsedExpr *expr);

    void AddSubqueryBinding(const std::string &name,
                            u64 table_index,
                            std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                            std::shared_ptr<std::vector<std::string>> column_names);

    void AddCTEBinding(const std::string &name,
                       u64 table_index,
                       std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                       std::shared_ptr<std::vector<std::string>> column_names);

    void AddViewBinding(const std::string &name,
                        u64 table_index,
                        std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                        std::shared_ptr<std::vector<std::string>> column_names);

    void AddTableBinding(const std::string &name,
                         u64 table_index,
                         std::shared_ptr<TableInfo> table_info,
                         std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                         std::shared_ptr<std::vector<std::string>> column_names,
                         std::shared_ptr<BlockIndex> block_index);

    // Merge input bind context into this bind context
    void AddBindContext(const std::shared_ptr<BindContext> &other_ptr);

    std::shared_ptr<ColumnExpression> ResolveColumnId(const ColumnIdentifier &column_identifier, i64 depth);

    void AddCorrelatedColumnExpr(const std::shared_ptr<ColumnExpression> &correlated_column);

    inline bool HasCorrelatedColumn() const { return !correlated_column_exprs_.empty(); }

    const Binding *GetBindingFromCurrentOrParentByName(const std::string &binding_name) const;

private:
    void AddBinding(const std::shared_ptr<Binding> &binding);

private:
    size_t next_logical_node_id_{1};
    size_t next_bind_context_index_{1};
    size_t next_table_index_{1};

public:
    // !!! TODO: Below need to be refactored !!!

public:
    // Binder, different binder have different expression build behavior.
    std::shared_ptr<ExpressionBinder> expression_binder_{nullptr};
};

} // namespace infinity
