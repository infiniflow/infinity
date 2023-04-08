//
// Created by JinHai on 2022/9/26.
//

#include "subquery_unnest.h"
#include "expression/expression_transformer.h"

#include "common/utility/infinity_assert.h"
#include "planner/node/logical_limit.h"
#include "expression/value_expression.h"
#include "main/infinity.h"
#include "function/aggregate_function_set.h"
#include "expression/aggregate_expression.h"
#include "planner/node/logical_aggregate.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"
#include "expression/function_expression.h"
#include "function/scalar_function_set.h"
#include "expression/cast_expression.h"

namespace infinity {

void
SubqueryUnnest::UnnestSubqueries(SharedPtr<BaseExpression> &expr_ptr,
                                 SharedPtr<LogicalNode> &root,
                                 const SharedPtr<QueryContext>& query_context_ptr,
                                 const SharedPtr<BindContext>& bind_context) {
    // 2. Call Unnest Subquery to resolve subquery
    if (expr_ptr->type() == ExpressionType::kSubQuery) {
        // Subquery, need to be unnested.
        UnnestSubquery(expr_ptr, root, query_context_ptr, bind_context);
    }
}

SharedPtr<BaseExpression>
SubqueryUnnest::UnnestSubquery(SharedPtr<BaseExpression>& expr_ptr,
                               SharedPtr<LogicalNode>& root,
                               const SharedPtr<QueryContext>& query_context_ptr,
                               const SharedPtr<BindContext>& bind_context) {
    // 1. Check the subquery type: uncorrelated subquery or correlated subquery.
    auto subquery_expr = std::static_pointer_cast<SubqueryExpression>(expr_ptr);

    auto right = subquery_expr->bound_select_statement_ptr_->BuildPlan(query_context_ptr, bind_context);
    // TODO: if the correlated information of the subquery should be stored in bind context.
    // Check the correlated information
    auto result = UnnestUncorrelated(subquery_expr.get(), root, right, query_context_ptr, bind_context);
    // If it isn't a correlated subquery

    // 2. Call different function to resolve uncorrelated subquery and correlated subquery.
    // 3. If the subquery also has another subquery nested, we need to resolve it recursively.

    return result;
}

SharedPtr<BaseExpression>
SubqueryUnnest::UnnestUncorrelated(SubqueryExpression* expr_ptr,
                                   SharedPtr<LogicalNode>& root,
                                   SharedPtr<LogicalNode>& subquery_plan,
                                   const SharedPtr<QueryContext>& query_context_ptr,
                                   const SharedPtr<BindContext>& bind_context) {
    switch(expr_ptr->subquery_type_) {

        case SubqueryType::kScalar: {
            // Step1 Generate limit operator on the subquery
            SharedPtr<ValueExpression> limit_expression = MakeShared<ValueExpression>(Value::MakeBigInt(1));
            SharedPtr<ValueExpression> offset_expression = MakeShared<ValueExpression>(Value::MakeBigInt(0));
            SharedPtr<LogicalLimit> limit_node = MakeShared<LogicalLimit>(bind_context->GetNewLogicalNodeId(),
                                                                          limit_expression,
                                                                          offset_expression);

            limit_node->set_left_node(subquery_plan);
            // Step2 Generate aggregate first operator on the limit operator
            auto& catalog = Infinity::instance().catalog();
            SharedPtr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName("first");
            ColumnBinding limit_column_binding = limit_node->GetColumnBindings()[0];

            SharedPtr<ColumnExpression> argument = ColumnExpression::Make(expr_ptr->Type(),
                                                                          subquery_plan->name(),
                                                                          limit_column_binding.table_idx,
                                                                          "0",
                                                                          limit_column_binding.column_idx,
                                                                          0);

            auto aggregate_function_set_ptr = std::static_pointer_cast<AggregateFunctionSet>(function_set_ptr);
            AggregateFunction first_function = aggregate_function_set_ptr->GetMostMatchFunction(argument);

            Vector<SharedPtr<BaseExpression>> arguments;
            arguments.emplace_back(argument);
            auto first_function_expr = MakeShared<AggregateExpression>(first_function, arguments);

            Vector<SharedPtr<BaseExpression>> groups; // empty group by list
            Vector<SharedPtr<BaseExpression>> aggregates;
            aggregates.emplace_back(first_function_expr);
            u64 group_by_index = bind_context->GenerateTableIndex();
            u64 aggregate_index = bind_context->GenerateTableIndex();
            SharedPtr<LogicalAggregate> aggregate_node = MakeShared<LogicalAggregate>(
                    bind_context->GetNewLogicalNodeId(),
                    groups,
                    group_by_index,
                    aggregates,
                    aggregate_index);

            aggregate_node->set_left_node(limit_node);

            // Step3 Generate cross product on the root and subquery plan
            u64 cross_product_table_index = bind_context->GenerateTableIndex();
            String alias = "cross_product" + std::to_string(cross_product_table_index);
            SharedPtr<LogicalCrossProduct> cross_product_node = MakeShared<LogicalCrossProduct>(
                    bind_context->GetNewLogicalNodeId(),
                    alias,
                    cross_product_table_index,
                    root,
                    aggregate_node);

            root = cross_product_node;
            // Step4 Return the first column of the cross product as the result
            SharedPtr<ColumnExpression> result = ColumnExpression::Make(expr_ptr->Type(),
                                                                          "",
                                                                          aggregate_index,
                                                                          first_function_expr->Name(),
                                                                          0,
                                                                          0);


            return result;
        }
        case SubqueryType::kExists: {
            // Construct following plan tree:
            // CrossProduct
            // |-> left plan tree
            // |-> Project (count_star = 1)
            //     |-> Aggregate( count(*) as count_start)
            //         |-> Limit (1)
            //             |-> right plan tree
            PlannerError("Plan EXISTS uncorrelated subquery");
            break;
        }
        case SubqueryType::kNotExists: {
            PlannerError("Plan not EXISTS uncorrelated subquery");
            break;
        }
        case SubqueryType::kNotIn:
        case SubqueryType::kIn: {
            // 1. Generate right column expression
            ColumnBinding right_column_binding = subquery_plan->GetColumnBindings()[0];
            SharedPtr<ColumnExpression> right_column = ColumnExpression::Make(expr_ptr->left_->Type(),
                                                                              subquery_plan->name(),
                                                                              right_column_binding.table_idx,
                                                                              "0",
                                                                              right_column_binding.column_idx,
                                                                              0);

            // 2. Generate condition expression;
            Vector<SharedPtr<BaseExpression>> function_arguments;
            function_arguments.reserve(2);
            function_arguments.emplace_back(expr_ptr->left_);
            SharedPtr<BaseExpression> right_expr = CastExpression::AddCastToType(right_column, expr_ptr->left_->Type());
            function_arguments.emplace_back(right_expr);

            auto& catalog = Infinity::instance().catalog();

            SharedPtr<FunctionSet> function_set_ptr = nullptr;
            if(expr_ptr->subquery_type_ == SubqueryType::kIn) {
                function_set_ptr = catalog->GetFunctionSetByName("=");
            } else {
                function_set_ptr = catalog->GetFunctionSetByName("<>");
            }
            auto scalar_function_set_ptr = std::static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
            ScalarFunction equi_function = scalar_function_set_ptr->GetMostMatchFunction(function_arguments);

            SharedPtr<FunctionExpression> function_expr_ptr = MakeShared<FunctionExpression>(equi_function,
                                                                                             function_arguments);

            Vector<SharedPtr<BaseExpression>> conditions;
            conditions.emplace_back(function_expr_ptr);

            // 3. Generate mark join
            idx_t mark_table_index = bind_context->GenerateTableIndex();
            String alias = "mark_join" + std::to_string(mark_table_index);

            SharedPtr<LogicalJoin> join_node = MakeShared<LogicalJoin>(bind_context->GetNewLogicalNodeId(),
                                                                       JoinType::kMark,
                                                                       alias,
                                                                       mark_table_index,
                                                                       conditions,
                                                                       root,
                                                                       subquery_plan);

            root = join_node;

            // 4. Generate output expression
            SharedPtr<ColumnExpression> result = ColumnExpression::Make(expr_ptr->Type(),
                                                                        function_expr_ptr->Name(),
                                                                        mark_table_index,
                                                                        "0",
                                                                        0,
                                                                        0);

            return result;
        }
        case SubqueryType::kAny:
            PlannerError("Plan ANY uncorrelated subquery");
            break;
        default:
            PlannerError("Unknown subquery type.");
    }

    PlannerError("Not implement to unnest uncorrelated subquery.");
}

SharedPtr<BaseExpression>
SubqueryUnnest::UnnestCorrelated(SubqueryExpression* expr_ptr,
                                 SharedPtr<LogicalNode>& root,
                                 SharedPtr<LogicalNode>& subquery_plan,
                                 const SharedPtr<QueryContext>& query_context_ptr,
                                 const SharedPtr<BindContext>& bind_context) {
    PlannerError("Not implement to unnest correlated subquery.");
}

}

