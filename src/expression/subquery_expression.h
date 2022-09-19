//
// Created by JinHai on 2022/9/19.
//

#include "base_expression.h"
#include "planner/plan_builder.h"

namespace infinity {

class SubqueryExpression : public BaseExpression {
public:
    explicit SubqueryExpression(std::shared_ptr<PlanBuildingContext> plan_building_context);

    std::shared_ptr<PlanBuildingContext> plan_building_context_;
private:
};

}
