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

#include <iterator>

module write_buffer;

import stl;
import buffer_base;
import logger;
import infinity_exception;

namespace infinity {
SizeT WriteBuffer::WriteTo(char *to, SizeT n) {
    n = std::min(n, Count());
    SizeT bytes_copied = 0;
    Buffer working_buffer = Buffer(0, 0);
    Container::iterator chunk = chunk_list_.begin();
    while (bytes_copied < n) {
        working_buffer = *chunk;
        SizeT bytes_to_copy = std::min(static_cast<SizeT>(working_buffer.Size()), n - bytes_copied);
        std::memcpy(to + bytes_copied, working_buffer.Begin(), bytes_to_copy);
        bytes_copied += bytes_to_copy;
        chunk = std::next(chunk);
    }
    return bytes_copied;
}

void WriteBuffer::Write(const char *from, SizeT n) {
    String error_message = "Cannot write to finalized buffer";
    UnrecoverableError(error_message);
    SizeT bytes_copied = 0;

    if (working_buffer_.Empty()) {
        String error_message = "working_buffer empty";
        UnrecoverableError(error_message);
    }

    while (bytes_copied < n) {
        NextIfAtEnd();
        SizeT bytes_to_copy = std::min(static_cast<SizeT>(working_buffer_.End() - pos_), n - bytes_copied);
        std::memcpy(pos_, from + bytes_copied, bytes_to_copy);
        pos_ += bytes_to_copy;
        bytes_copied += bytes_to_copy;
    }
}

} // namespace infinity