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
    //      TODO: WHY WE NEED last_visited_pipeline_id ?
    //    uint64_t last_visited_pipeline_id = std::numeric_limits<uint64_t>::max();
    pipeline_stack.push(pipeline);
    while(!pipeline_stack.empty()) {
        current_pipeline = pipeline_stack.top();

        if( !current_pipeline->predecessors().empty() ) {
            // this pipeline has predecessors
            const std::vector<std::weak_ptr<Pipeline>>& childs = current_pipeline->predecessors();
            for(const std::weak_ptr<Pipeline>& weak_child: childs) {
                auto shared_child = weak_child.lock();
                pipeline_stack.push(shared_child);
            }
        } else {
            current_pipeline->Execute(query_context);
//            last_visited_pipeline_id = current_pipeline->Id();
            pipeline_stack.pop();
        }
//      TODO: WHY WE NEED last_visited_pipeline_id ?
//        if( (!current_pipeline->predecessors().empty())
//            && current_pipeline->predecessors().begin()->lock()->Id() != last_visited_pipeline_id) {
//            const std::vector<std::weak_ptr<Pipeline>>& childs = current_pipeline->predecessors();
//            for(const std::weak_ptr<Pipeline>& child: childs) {
//                pipeline_stack.push(child.lock());
//            }
//        } else {
//            current_pipeline->Execute(query_context);
//            last_visited_pipeline_id = current_pipeline->Id();
//            pipeline_stack.pop();
//        }
    }
}


}
