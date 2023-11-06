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