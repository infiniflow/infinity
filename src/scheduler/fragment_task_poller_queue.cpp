// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "concurrentqueue.h"

import stl;
import fragment_task;
import third_party;

module fragment_task_poller_queue;

namespace infinity {

void FragmentTaskPollerQueue::Enqueue(FragmentTask *task_ptr) { queue_.enqueue(task_ptr); }

SizeT FragmentTaskPollerQueue::DequeueBulk(List<FragmentTask *>::iterator &output_iter) {
    return queue_.try_dequeue_bulk(output_iter, queue_.size_approx());
}

FragmentTask* FragmentTaskPollerQueue::Dequeue() {
    FragmentTask* result{};
//    queue_.wait_dequeue(result);
    queue_.try_dequeue(result);
    return result;
}

SizeT FragmentTaskPollerQueue::Size() const { return queue_.size_approx(); }

} // namespace infinity
