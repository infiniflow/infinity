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
import parser;
import buffer_handle;
import buffer_obj;
import buffer_manager;

export module column_buffer;

namespace infinity {

struct OutlineBuffer {
    BufferManager *buffer_mgr_{};

    SizeT current_file_idx_{u64_max};

    SharedPtr<String> base_dir_{};

    BufferHandle outline_ele_{};

    OutlineBuffer(BufferManager *buffer_mgr, SharedPtr<String> base_dir) : buffer_mgr_(buffer_mgr), base_dir_(Move(base_dir)) {}
};

export class ColumnBuffer {
    BufferHandle inline_col_{};

    // is null, if the column is inline
    UniquePtr<OutlineBuffer> outline_buffer_{};

public:
    ColumnBuffer(BufferObj *buffer, BufferManager *buffer_mgr){};

    explicit ColumnBuffer(BufferObj *buffer, BufferManager *buffer_mgr = nullptr, SharedPtr<String> base_dir = nullptr)
        : inline_col_(buffer->Load()) {
        if (buffer_mgr) {
            outline_buffer_ = MakeUnique<OutlineBuffer>(buffer_mgr, Move(base_dir));
        }
    }

    const_ptr_t GetAll();

    const_ptr_t GetValueAt(SizeT row_idx, const DataType &data_type);

    Pair<const_ptr_t, SizeT> GetVarcharAt(SizeT row_idx);

    Pair<const_ptr_t, SizeT> GetVarcharAtPrefix(SizeT row_idx);

    ptr_t GetAllMut();

    ptr_t GetValueAtMut(SizeT row_idx, const DataType &data_type);

    Pair<ptr_t, SizeT> GetVarcharAtMut(SizeT row_idx);

    Pair<ptr_t, SizeT> GetVarcharAtPrefixMut(SizeT row_idx);
};

} // namespace infinity
