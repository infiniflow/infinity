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

export module block_column_iter;

import stl;
import buffer_handle;
import catalog;

namespace infinity {

export class BlockColumnIter {
public:
    BlockColumnIter(BlockColumnEntry *entry, u16 row_count)
        : buffer_handle_(entry->buffer()->Load()), ele_size_(entry->column_type()->Size()), row_count_(row_count), offset_(0) {}

    Optional<const void *> Next() {
        if (offset_ >= row_count_) {
            return None;
        }
        auto ret = static_cast<const char *>(buffer_handle_.GetData()) + offset_ * ele_size_;
        ++offset_;
        return ret;
    }

private:
    BufferHandle buffer_handle_;
    SizeT ele_size_;
    SizeT row_count_;

    SizeT offset_;
};

} // namespace infinity