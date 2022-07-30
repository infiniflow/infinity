//
// Created by JinHai on 2022/7/28.
//

#include "naive_scheduler.h"

#include <stack>

namespace infinity {


void
NaiveScheduler::Schedule(const std::shared_ptr<Pipeline> &pipeline) {
    std::stack<std::shared_ptr<Pipeline>> pipeline_stack;

    std::shared_ptr<Pipeline> current_pipeline = nullptr;
    uint64_t last_visited_pipeline_id = std::numeric_limits<uint64_t>::max();
    pipeline_stack.push(pipeline);
    while(!pipeline_stack.empty()) {
        current_pipeline = pipeline_stack.top();

        if( (!current_pipeline->predecessors().empty())
            && current_pipeline->predecessors().begin()->lock()->Id() != last_visited_pipeline_id) {
            const std::vector<std::weak_ptr<Pipeline>>& childs = current_pipeline->predecessors();
            for(const std::weak_ptr<Pipeline>& child: childs) {
                pipeline_stack.push(child.lock());
            }
        } else {
            current_pipeline->Execute();
            last_visited_pipeline_id = current_pipeline->Id();
            pipeline_stack.pop();
        }
    }
}


}
