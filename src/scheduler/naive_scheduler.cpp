//
// Created by JinHai on 2022/7/28.
//

#include "naive_scheduler.h"

#include <stack>
#include <limits>

namespace infinity {


void
NaiveScheduler::Schedule(std::shared_ptr<QueryContext>& query_context, const std::shared_ptr<Pipeline> &pipeline) {
    std::stack<std::shared_ptr<Pipeline>> pipeline_stack;

    std::shared_ptr<Pipeline> current_pipeline = nullptr;
    std::set<uint64_t> visited;
//    uint64_t last_visited_pipeline_id = std::numeric_limits<uint64_t>::max();
    pipeline_stack.push(pipeline);
    while(!pipeline_stack.empty()) {
        current_pipeline = pipeline_stack.top();

        if( !current_pipeline->predecessors().empty() ) {
            // this pipeline has predecessors
            const std::vector<std::shared_ptr<Pipeline>>& children = current_pipeline->predecessors();

            bool has_child = false;
            for(const std::shared_ptr<Pipeline>& shared_child: children) {
                if(visited.contains(shared_child->Id())) {
                    continue;
                }
                pipeline_stack.push(shared_child);
                has_child = true;
            }

            if(has_child) {
                continue;
            }
        }

        current_pipeline->Execute(query_context);
        visited.insert(current_pipeline->Id());
        pipeline_stack.pop();

    }
}


}
