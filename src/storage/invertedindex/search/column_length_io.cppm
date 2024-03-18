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

export module column_length_io;

import stl;
import index_defines;
import internal_types;
import table_entry;
import buffer_handle;

namespace infinity {

export class ColumnLengthWriter {};

export class ColumnLengthReader {
    u32 column_counter_{};
    // all available data in the table
    Map<SegmentID, Vector<BufferHandle>> column_length_data_buffer_handles_;

public:
    ColumnLengthReader() = default;
    ~ColumnLengthReader() = default;

    template <typename ScorerColumnIndexMapType>
    void LoadColumnLength(u32 column_counter,
                          ScorerColumnIndexMapType &column_index_map,
                          TableEntry *table_entry,
                          TransactionID txn_id,
                          TxnTimeStamp begin_ts);

    void UpdateAvgColumnLength(Vector<float> &avg_column_length);

    u32 GetColumnLength(u32 scorer_column_idx, RowID doc_id);
};

} // namespace infinity