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

module emvb_shared_vec;
import stl;

namespace infinity {

// It seems that 'MakeShared' cannot be put into the cppm file.
template <typename T>
void EMVBSharedVec<T>::ReserveUnderLock(u32 new_capacity) {
    if (new_capacity <= capacity_) {
        return;
    }
    auto new_data = MakeShared<T[]>(new_capacity);
    std::copy_n(data_.get(), size_, new_data.get());
    data_ = std::move(new_data);
    capacity_ = new_capacity;
}

template class EMVBSharedVec<u32>;

} // namespace infinity
