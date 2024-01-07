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

    SizeT current_file_idx_{std::numeric_limits<u64>::max()};

    SharedPtr<String> base_dir_{};

    BufferHandle outline_ele_{};

    OutlineBuffer(BufferManager *buffer_mgr) : buffer_mgr_(buffer_mgr), base_dir_(buffer_mgr->BaseDir()) {}

    OutlineBuffer(BufferManager *buffer_mgr, SharedPtr<String> base_dir) : buffer_mgr_(buffer_mgr), base_dir_(base_dir) {}
};

export class ColumnBuffer {
    u64 column_id_{0};

    BufferHandle inline_col_{};

    // is null, if the column is inline
    UniquePtr<OutlineBuffer> outline_buffer_{};

public:
    ColumnBuffer(u64 column_id, BufferObj *, BufferManager *) : column_id_(column_id){};

    ColumnBuffer(u64 column_id, const BufferHandle &buffer_handle, BufferManager *buffer_mgr, SharedPtr<String> base_dir = nullptr)
        : column_id_(column_id), inline_col_(buffer_handle) {
        if (buffer_mgr) {
            outline_buffer_ = MakeUnique<OutlineBuffer>(buffer_mgr, base_dir);
        }
    }

    ColumnBuffer(u64 column_id, BufferObj *buffer, BufferManager *buffer_mgr = nullptr, SharedPtr<String> base_dir = nullptr)
        : column_id_(column_id), inline_col_(buffer->Load()) {
        if (buffer_mgr) {
            outline_buffer_ = MakeUnique<OutlineBuffer>(buffer_mgr, base_dir);
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
