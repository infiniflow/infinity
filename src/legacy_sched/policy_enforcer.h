#pragma once

#include "main/query_context.h"

#include <queue>

namespace infinity {

class PolicyEnforcer {
public:
    PolicyEnforcer() {}

    ~PolicyEnforcer() noexcept = default;

    bool
    AdmitQuery(QueryContext* query_context);

    bool
    HasWaitingQueries() const {
        return !(waiting_queries_.empty());
    }
private:
    static constexpr std::size_t kMaxConcurrentQueries = 1;

    std::queue<QueryContext*> waiting_queries_;
};

}