//
// Created by jinhai on 23-10-22.
//

module;

#include "blockingconcurrentqueue.h"

import stl;
import fragment_data;

module fragment_data_queue;

namespace infinity {

void FragmentDataQueue::Enqueue(const SharedPtr<FragmentData> &fragment_data) { queue_.enqueue(fragment_data); }

bool FragmentDataQueue::TryDequeue(SharedPtr<FragmentData> &fragment_data) { return queue_.try_dequeue(fragment_data); }

void FragmentDataQueue::Dequeue(SharedPtr<FragmentData> &fragment_data) { queue_.wait_dequeue(fragment_data); }

} // namespace infinity