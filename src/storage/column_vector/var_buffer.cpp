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

#include <vector>

module var_buffer;

import stl;
import infinity_exception;
import third_party;

namespace infinity {

SizeT VarBuffer::Append(UniquePtr<char[]> buffer, SizeT size) {
    buffers_.push_back(std::move(buffer));
    SizeT offset = buffer_size_prefix_sum_.back();
    buffer_size_prefix_sum_.push_back(offset + size);
    if (buffer_obj_ != nullptr) {
        buffer_obj_->AddBufferSize(size);
    }
    return offset;
}

SizeT VarBuffer::Append(const char *data, SizeT size) {
    auto buffer = MakeUnique<char[]>(size);
    std::memcpy(buffer.get(), data, size);
    return Append(std::move(buffer), size);
}

const char *VarBuffer::Get(SizeT offset, SizeT size) const {
    // find the last index i such that buffer_size_prefix_sum_[i] <= offset
    auto it = std::upper_bound(buffer_size_prefix_sum_.begin(), buffer_size_prefix_sum_.end(), offset);
    if (it == buffer_size_prefix_sum_.end()) {
        String error_msg = fmt::format("offset {} is out of range {}", offset, buffer_size_prefix_sum_.back());
        UnrecoverableError(error_msg);
    }
    if (it == buffer_size_prefix_sum_.begin()) {
        String error_msg = fmt::format("prefix_sum[0] should be 0, but got {}", *it);
        UnrecoverableError(error_msg);
    }
    SizeT i = std::distance(buffer_size_prefix_sum_.begin(), it) - 1;
    SizeT offset_in_buffer = offset - buffer_size_prefix_sum_[i];
    if (offset_in_buffer + size > buffer_size_prefix_sum_[i + 1]) {
        String error_msg =
            fmt::format("offset {} and size {} is out of range [{}, {})", offset, size, buffer_size_prefix_sum_[i], buffer_size_prefix_sum_[i + 1]);
        UnrecoverableError(error_msg);
    }
    return buffers_[i].get() + offset_in_buffer;
}

} // namespace infinity
