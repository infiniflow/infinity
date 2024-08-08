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

export module var_buffer;

import stl;
import buffer_obj;

namespace infinity {

export class VarBuffer {
public:
    VarBuffer() = default;

    VarBuffer(BufferObj *buffer_obj) : buffer_obj_(buffer_obj) {}

    SizeT Append(UniquePtr<char[]> buffer, SizeT size);

    SizeT Append(const char *data, SizeT size);

    const char *Get(SizeT offset, SizeT size) const;

private:
    Vector<UniquePtr<char[]>> buffers_;
    Vector<SizeT> buffer_size_prefix_sum_ = {0};

    BufferObj *buffer_obj_ = nullptr;
};

} // namespace infinity