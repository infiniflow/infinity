//
// Created by jinhai on 23-9-14.
//

module;

import stl;
import fragment_data;
import third_party;

export module fragment_data_queue;

namespace infinity {

export struct FragmentDataQueue {
    void Enqueue(const SharedPtr<FragmentData> &fragment_data);

    template <class It>
    inline void EnqueueBulk(It iter, SizeT count) {
        queue_.enqueue_bulk(Forward<It>(iter), count);
    }

    bool TryDequeue(SharedPtr<FragmentData> &fragment_data);

    void Dequeue(SharedPtr<FragmentData> &fragment_data);

    inline SizeT ApproxSize() const { return queue_.size_approx(); }

    BlockingConcurrentQueue<SharedPtr<FragmentData>> queue_;
};

} // namespace infinity