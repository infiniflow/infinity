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

export module invert_task;

import stl;
import column_vector;
import internal_types;

namespace infinity {

export struct BatchInvertTask {
public:
    BatchInvertTask(u64 task_seq, const ColumnVector &column_vector, u32 row_offset, u32 row_count, RowID row_id_begin)
        : task_seq_(task_seq), column_vector_(column_vector), row_offset_(row_offset), row_count_(row_count), row_id_begin_(row_id_begin) {}

    u64 task_seq_;
    const ColumnVector &column_vector_;
    u32 row_offset_;
    u32 row_count_;
    RowID row_id_begin_;
};
} // namespace infinity
