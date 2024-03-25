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
import bm25_ranker;
import internal_types;
import column_index_reader;

namespace infinity {

u32 Scorer::GetOrSetColumnIndex(u64 column_id) {
    if (auto iter = column_index_map_.find(column_id); iter == column_index_map_.end()) {
        column_index_map_[column_id] = column_counter_;
        column_ids_.push_back(column_id);
        return column_counter_++;
    } else {
        return iter->second;
    }
}

void Scorer::AddDocIterator(TermDocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    iterators_.resize(column_index + 1);
    iterators_[column_index].push_back(iter);
}

void Scorer::LoadColumnLength(RowID first_doc_id, IndexReader &index_reader) {
    column_length_reader_.LoadColumnLength(first_doc_id, index_reader, column_ids_, avg_column_length_);
}

float Scorer::Score(RowID doc_id) {
    float score = 0.0F;
    for (u32 i = 0; i < column_counter_; i++) {
        BM25Ranker ranker(total_df_);
        float avg_column_length = avg_column_length_[i];
        u32 column_len = column_length_reader_.GetColumnLength(i, doc_id);
        Vector<TermDocIterator *> &column_iters = iterators_[i];
        TermColumnMatchData column_match_data;
        for (u32 j = 0; j < column_iters.size(); j++) {
            if (column_iters[j]->GetTermMatchData(column_match_data, doc_id)) {
                ranker.AddTermParam(column_match_data.tf_, column_iters[j]->GetDF(), avg_column_length, column_len, column_iters[j]->GetWeight());
            }
        }
        score += ranker.GetScore();
    }
    return score;
}

} // namespace infinity
