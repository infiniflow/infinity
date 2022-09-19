//
// Created by JinHai on 2022/9/19.
//

#include "subquery_expression.h"

#include <utility>

namespace infinity {

SubqueryExpression::SubqueryExpression(std::shared_ptr<PlanBuildingContext> plan_building_context)
    : BaseExpression(ExpressionType::kSubQuery, {}),
    plan_building_context_(std::move(plan_building_context)) {}


}

