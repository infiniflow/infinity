//
// Created by JinHai on 2022/7/28.
//

#include "naive_scheduler.h"

#include <limits>
#include <stack>

namespace infinity {

void NaiveScheduler::Schedule(QueryContext *query_context, const SharedPtr<Pipeline> &pipeline) {
    std::stack<SharedPtr<Pipeline>> pipeline_stack;

    SharedPtr<Pipeline> current_pipeline = nullptr;
    std::set<uint64_t> visited;
    //    uint64_t last_visited_pipeline_id = std::numeric_limits<uint64_t>::max();
    pipeline_stack.push(pipeline);
    while (!pipeline_stack.empty()) {
        current_pipeline = pipeline_stack.top();

        if (!current_pipeline->predecessors().empty()) {
            // this pipeline has predecessors
            const std::vector<SharedPtr<Pipeline>> &children = current_pipeline->predecessors();

            bool has_child = false;
            for (const SharedPtr<Pipeline> &shared_child : children) {
                if (visited.contains(shared_child->Id())) {
                    continue;
                }
                pipeline_stack.push(shared_child);
                has_child = true;
            }

            if (has_child) {
                continue;
            }
        }

        current_pipeline->Execute(query_context);
        visited.insert(current_pipeline->Id());
        pipeline_stack.pop();
    }
}

} // namespace infinity
