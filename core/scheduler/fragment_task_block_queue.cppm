//
// Created by jinhai on 23-9-8.
//

module;

import stl;
import third_party;
import fragment_task;

export module fragment_task_block_queue;

namespace infinity {

struct FragmentTaskBlockQueue {
    void Enqueue(FragmentTask *task);

    template <class It>
    void EnqueueBulk(It iter, SizeT count);

    void Dequeue(FragmentTask *&task);

    BlockingConcurrentQueue<FragmentTask *> queue_;
};

} // namespace infinity
