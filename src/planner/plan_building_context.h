//
// Created by JinHai on 2022/9/19.
//

#pragma once

#include "logical_node.h"


namespace infinity {
struct PlanBuildingContext {
    std::shared_ptr<LogicalNode> plan;

    // Output of the last plan node.
    std::vector<LogicalType> types;
    std::vector<std::string> output_names;
};
}


