//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include "query_context.h"

#include "prepared_plan.h"

#include <unordered_map>

namespace infinity {

class Session {
public:
    explicit Session();

private:
    // session config such as memory limitation, concurrency limitation

    // query context for current query
    std::unique_ptr<QueryContext> current_query_context_;

    // prepared plan for PREPARE and EXECUTE statement
    std::unordered_map<std::string, std::shared_ptr<PreparedPlan>> prepared_plans_;

    // Current schema
    std::string current_schema_;

    // User / Tenant information
    std::string tenant_name_;
    std::string user_name_;

    uint64_t tenant_id_{0};
    uint64_t user_id_{0};
};

}
