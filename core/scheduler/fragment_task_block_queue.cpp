//
// Created by jinhai on 23-10-22.
//

module ;

#include "blockingconcurrentqueue.h"

import fragment_task;
import stl;

module fragment_task_block_queue;

namespace infinity {
void FragmentTaskBlockQueue::Enqueue(FragmentTask *task) { queue_.enqueue(task); }

template <class It>
void FragmentTaskBlockQueue::EnqueueBulk(It iter, SizeT count) {
    queue_.enqueue_bulk(Forward<It>(iter), count);
}

void FragmentTaskBlockQueue::Dequeue(FragmentTask *&task) { queue_.wait_dequeue(task); }

}