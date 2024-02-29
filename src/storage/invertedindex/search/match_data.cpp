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

module match_data;

import stl;
import term_doc_iterator;
import index_defines;

namespace infinity {

MatchDataManager::MatchDataManager() { column_length_reader_ = MakeUnique<ColumnLengthReader>(); }

u32 MatchDataManager::GetOrSetColumnIndex(u64 column_id) {
    if (column_index_map_.find(column_id) == column_index_map_.end()) {
        column_index_map_[column_id] = column_counter_;
        match_data_.term_columns_.resize(column_counter_ + 1);
        return column_counter_++;
    } else
        return column_index_map_[column_id];
}

void MatchDataManager::AddDocIterator(TermDocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    iterators_.resize(column_index + 1);
    iterators_[column_index].push_back(iter);
}

void MatchDataManager::FetchMatchData(docid_t doc_id) {
    for (u32 i = 0; i < column_counter_; i++) {
        Vector<TermDocIterator *> &column_iters = iterators_[i];
        TermColumnMatchData &column_match_data = match_data_.term_columns_[i];
        for (u32 j = 0; j < column_iters.size(); j++) {
            if (column_iters[j]->GetTermMatchData(column_match_data, doc_id)) {
                // score
            }
        }
    }
}

} // namespace infinity
