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

namespace infinity {

Scorer::Scorer(u64 num_of_docs) : total_df_(num_of_docs) { column_length_reader_ = MakeUnique<ColumnLengthReader>(); }

u32 Scorer::GetOrSetColumnIndex(u64 column_id) {
    if (column_index_map_.find(column_id) == column_index_map_.end()) {
        column_index_map_[column_id] = column_counter_;
        match_data_.term_columns_.resize(column_counter_ + 1);
        return column_counter_++;
    } else
        return column_index_map_[column_id];
}

void Scorer::InitRanker(const Map<u64, double> &weight_map) {
    for (auto it : weight_map) {
        u32 column_index = GetOrSetColumnIndex(it.first);
        column_weights_.resize(column_index + 1);
        column_weights_[column_index] = it.second;
        column_ids_.resize(column_index + 1);
        column_ids_[column_index] = it.first;
    }
    for (u32 i = 0; i < column_counter_; ++i) {
        avg_column_length_[i] = GetAvgColumnLength(column_ids_[i]);
    }
}

double Scorer::GetAvgColumnLength(u64 column_id) {
    double length = 0.0F;
    //  TODO
    return length;
}

void Scorer::AddDocIterator(TermDocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    iterators_.resize(column_index + 1);
    iterators_[column_index].push_back(iter);
}

float Scorer::Score(docid_t doc_id) {
    float score = 0.0F;
    for (u32 i = 0; i < column_counter_; i++) {
        BM25Ranker ranker(total_df_);
        u32 column_len = column_length_reader_->GetColumnLength(column_ids_[i], doc_id);
        Vector<TermDocIterator *> &column_iters = iterators_[i];
        TermColumnMatchData &column_match_data = match_data_.term_columns_[i];
        for (u32 j = 0; j < column_iters.size(); j++) {
            if (column_iters[j]->GetTermMatchData(column_match_data, doc_id)) {
                ranker.AddTermParam(column_match_data.tf_, column_iters[j]->GetDF(), avg_column_length_[i], column_len);
            }
        }
        auto s = ranker.GetScore();
        score += column_weights_[i] * s;
    }
    return score;
}

} // namespace infinity
