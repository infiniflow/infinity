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
import default_values;
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
    Vector<const u32 *> column_length_vector_;

public:
    ColumnLengthReader() = default;
    ~ColumnLengthReader() = default;

    template <typename ScorerColumnIndexMapType>
    void LoadColumnLength(u32 column_counter,
                          ScorerColumnIndexMapType &column_index_map,
                          TableEntry *table_entry,
                          TransactionID txn_id,
                          TxnTimeStamp begin_ts);

    void UpdateAvgColumnLength(Vector<float> &avg_column_length) const;

    inline void UpdateTargetSegment(SegmentID segment_id) {
        if (auto iter = column_length_data_buffer_handles_.find(segment_id); iter != column_length_data_buffer_handles_.end()) [[likely]] {
            const Vector<BufferHandle> &buffer_handles = iter->second;
            for (u32 i = 0; i < column_counter_; ++i) {
                const auto *column_length_data = static_cast<const FullTextColumnLengthData *>(buffer_handles[i].GetData());
                column_length_vector_[i] = column_length_data->column_length_.data();
            }
        } else {
            UnrecoverableError(fmt::format("Segment {} is not found in column length data", segment_id));
        }
    }

    inline u32 GetColumnLength(u32 scorer_column_idx, SegmentOffset segment_offset) const {
        // TODO yzc: adapt for realtime index
        // return column_length_vector_[scorer_column_idx][segment_offset];
        return 100UL;
    }
};

} // namespace infinity