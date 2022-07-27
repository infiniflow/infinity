//
// Created by JinHai on 2022/7/28.
//

#include "task.h"

namespace infinity {

void
Task::SetPredecessorOf(const std::shared_ptr<Task> &successor) {
    // Prevents add a duplicated successor.
    if(std::find(successors_.cbegin(), successors_.cend(), successor) != successors_.cend()) return ;

    successors_.emplace_back(successor);
    successor->predecessors_.emplace_back(shared_from_this());

    // If this task isn't finished, its successor will have one more dependent predecessor.
    // I suppose the top is set before running, so no race protection for the state change.
    // pending_predecessors is an atomic value, which is safe for change in any cases.
    if (!IsDone()) successor->pending_predecessors_ ++;
}

void
Task::Schedule() {

}

void
Task::Execute() {

}

void
Task::OnPredecessorDone() {

}


}


