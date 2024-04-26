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
import blockmax_term_doc_iterator;
import doc_iterator;
import phrase_doc_iterator;
import blockmax_phrase_doc_iterator;

namespace infinity {

void Scorer::Init(u64 num_of_docs, IndexReader *index_reader) {
    total_df_ = num_of_docs;
    index_reader_ = index_reader;
}

u32 Scorer::GetOrSetColumnIndex(u64 column_id) {
    if (auto iter = column_index_map_.find(column_id); iter == column_index_map_.end()) {
        column_index_map_[column_id] = column_counter_;
        column_ids_.push_back(column_id);
        column_length_reader_.AppendColumnLength(index_reader_, column_ids_, avg_column_length_);
        return column_counter_++;
    } else {
        return iter->second;
    }
}

void Scorer::AddDocIterator(DocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    iterators_.resize(std::max<u32>(column_index + 1, iterators_.size()));
    iterators_[column_index].push_back(iter);
}

void Scorer::AddBlockMaxDocIterator(BlockMaxTermDocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    block_max_term_iterators_.resize(std::max<u32>(column_index + 1, block_max_term_iterators_.size()));
    block_max_term_iterators_[column_index].push_back(iter);
    iter->InitBM25Info(total_df_, avg_column_length_[column_index], column_length_reader_.GetColumnLengthReader(column_index));
}

void Scorer::AddBlockMaxPhraseDocIterator(BlockMaxPhraseDocIterator *iter, u64 column_id) {
    u32 column_index = GetOrSetColumnIndex(column_id);
    block_max_phrase_iterators_.resize(std::max<u32>(column_index + 1, block_max_phrase_iterators_.size()));
    block_max_phrase_iterators_[column_index].push_back(iter);
    iter->InitBM25Info(total_df_, avg_column_length_[column_index], column_length_reader_.GetColumnLengthReader(column_index));
}

float Scorer::Score(RowID doc_id) {
    float score = 0.0F;
    for (u32 i = 0; i < column_counter_; i++) {
        BM25Ranker ranker(total_df_);
        float avg_column_length = avg_column_length_[i];
        u32 column_len = column_length_reader_.GetColumnLength(i, doc_id);
        Vector<DocIterator *> &column_iters = iterators_[i];
        if (column_iters.empty()) {
            continue;
        }
        if (column_iters[0]->GetType() == DocIteratorType::kTermIterator) {
            TermColumnMatchData column_match_data;
            for (u32 j = 0; j < column_iters.size(); j++) {
                TermDocIterator* term_iter = dynamic_cast<TermDocIterator*>(column_iters[j]);
                if (term_iter->GetTermMatchData(column_match_data, doc_id)) {
                    ranker.AddTermParam(column_match_data.tf_, term_iter->GetDF(), avg_column_length, column_len, term_iter->GetWeight());
                }
            }
        } else if (column_iters[0]->GetType() == DocIteratorType::kPhraseIterator) {
            for (u32 j = 0; j < column_iters.size(); j++) {
                PhraseColumnMatchData column_match_data;
                PhraseDocIterator* phrase_iter = dynamic_cast<PhraseDocIterator*>(column_iters[j]);
                if (phrase_iter->GetPhraseMatchData(column_match_data, doc_id)) {
                    ranker.AddPhraseParam(column_match_data.all_tf_,
                                          phrase_iter->GetAllDF(),
                                          avg_column_length,
                                          column_len,
                                          phrase_iter->GetWeight(),
                                          column_iters.size());
                }
            }
        }
        score += ranker.GetScore();
    }
    return score;
}

} // namespace infinity
