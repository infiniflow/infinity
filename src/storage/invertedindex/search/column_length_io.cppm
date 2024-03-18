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
import fulltext_file_worker;
import third_party;
import infinity_exception;

namespace infinity {

export class ColumnLengthWriter {};

export class ColumnLengthReader {
    struct Hash {
        inline SegmentID operator()(const SegmentID &val) const { return val; }
    };
    u32 column_counter_{};
    // all available data in the table
    FlatHashMap<SegmentID, Vector<BufferHandle>, Hash> column_length_data_buffer_handles_;

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

    inline u32 GetColumnLength(u32 scorer_column_idx, RowID doc_id) {
        SegmentID segment_id = doc_id.segment_id_;
        SegmentOffset segment_offset = doc_id.segment_offset_;
        if (auto iter = column_length_data_buffer_handles_.find(segment_id); iter != column_length_data_buffer_handles_.end()) {
            auto &buffer_handle = (iter->second)[scorer_column_idx];
            auto &column_length_data = *static_cast<const FullTextColumnLengthData *>(buffer_handle.GetData());
            return column_length_data.column_length_[segment_offset];
        } else {
            UnrecoverableError(fmt::format("Segment {} is not found in column length data", segment_id));
            return -1;
        }
    }
};

} // namespace infinity