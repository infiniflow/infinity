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
module column_length_io;

import stl;
import index_defines;
import internal_types;
import status;
import match_data;
import create_index_info;
import table_entry;
import segment_index_entry;
import third_party;
import buffer_handle;
import fulltext_file_worker;
import infinity_exception;
import logger;

namespace infinity {

template <typename ScorerColumnIndexMapType>
void ColumnLengthReader::LoadColumnLength(u32 column_counter,
                                          ScorerColumnIndexMapType &column_index_map,
                                          TableEntry *table_entry,
                                          TransactionID txn_id,
                                          TxnTimeStamp begin_ts) {
    column_counter_ = column_counter;
    column_length_data_buffer_handles_.clear();
    for (auto map_guard = table_entry->IndexMetaMap(); auto &[index_name, table_index_meta] : *map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
        if (!status.ok()) {
            // Table index entry isn't found
            RecoverableError(status);
        }
        const String column_name = table_index_entry->index_base()->column_name();
        auto column_id = table_entry->GetColumnIdByName(column_name);
        auto iter_column_index = column_index_map.find(column_id);
        if (iter_column_index == column_index_map.end()) {
            // unnecessary column
            continue;
        }
        u32 column_index = iter_column_index->second;
        // check index type
        if (auto index_type = table_index_entry->index_base()->index_type_; index_type != IndexType::kFullText) {
            // non-fulltext index
            continue;
        }
        const HashMap<u32, SharedPtr<SegmentIndexEntry>> &index_by_segment = table_index_entry->index_by_segment();
        for (auto &[segment_id, segment_index_entry] : index_by_segment) {
            auto &buffer_handle_v = column_length_data_buffer_handles_[segment_id];
            if (buffer_handle_v.empty()) {
                buffer_handle_v.resize(column_counter);
            }
            if (buffer_handle_v[column_index].GetData() != nullptr) {
                UnrecoverableError(fmt::format("Duplicate fulltext indexes for the same column {}", column_name));
            }
            buffer_handle_v[column_index] = segment_index_entry->GetIndex();
        }
    }
    // check if all columns are loaded
    for (const auto &[_, buffer_handle_v] : column_length_data_buffer_handles_) {
        bool valid = true;
        if (buffer_handle_v.size() != column_counter) {
            valid = false;
        } else {
            for (const auto &buffer_handle : buffer_handle_v) {
                if (buffer_handle.GetData() == nullptr) {
                    valid = false;
                    break;
                }
            }
        }
        if (!valid) {
            UnrecoverableError("segment list mismatch between different fulltext indexes");
        }
    }
    column_length_vector_.resize(column_counter_);
}

template void ColumnLengthReader::LoadColumnLength(u32 column_counter,
                                                   Scorer::ColumnIndexMapType &column_index_map,
                                                   TableEntry *table_entry,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts);

void ColumnLengthReader::UpdateAvgColumnLength(Vector<float> &avg_column_length) const {
    avg_column_length.clear();
    avg_column_length.resize(column_counter_);
    u32 total_row_count = 0;
    for (const auto &[segment_id, buffer_handle_v] : column_length_data_buffer_handles_) {
        u32 segment_row_cnt = -1;
        for (u32 i = 0; i < column_counter_; ++i) {
            auto &buffer_handle = buffer_handle_v[i];
            auto &column_length_data = *static_cast<const FullTextColumnLengthData *>(buffer_handle.GetData());
            if (segment_row_cnt == u32(-1)) {
                segment_row_cnt = column_length_data.row_count_;
                total_row_count += segment_row_cnt;
            } else if (segment_row_cnt != column_length_data.row_count_) {
                UnrecoverableError(fmt::format("Segment {} has different row count for different columns", segment_id));
            }
            avg_column_length[i] += column_length_data.total_length_;
        }
    }
    for (auto &len : avg_column_length) {
        len /= total_row_count;
    }
}

} // namespace infinity