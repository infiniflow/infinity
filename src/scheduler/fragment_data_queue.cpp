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

#include "blockingconcurrentqueue.h"

import stl;
import fragment_data;

module fragment_data_queue;

namespace infinity {

void FragmentDataQueue::Enqueue(const SharedPtr<FragmentData> &fragment_data) { queue_.enqueue(fragment_data); }

bool FragmentDataQueue::TryDequeue(SharedPtr<FragmentData> &fragment_data) { return queue_.try_dequeue(fragment_data); }

void FragmentDataQueue::Dequeue(SharedPtr<FragmentData> &fragment_data) { queue_.wait_dequeue(fragment_data); }

} // namespace infinity