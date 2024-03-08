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

export module match_data;

import stl;
import third_party;
import index_defines;
import column_length_io;

namespace infinity {
export struct TermColumnMatchData {
    docid_t doc_id_;
    tf_t tf_;
    docpayload_t doc_payload_;
};

export struct MatchData {
    Vector<TermColumnMatchData> term_columns_;

    TermColumnMatchData *ResolveTermColumn(u32 column_sequence) { return &term_columns_[column_sequence]; }
};

class TermDocIterator;
export class Scorer {
public:
    Scorer(u64 num_of_docs);

    ~Scorer() = default;

    void InitRanker(const Map<u64, double> &weight_map);

    void AddDocIterator(TermDocIterator *iter, u64 column_id);

    float Score(docid_t doc_id);

private:
    u32 GetOrSetColumnIndex(u64 column_id);

    double GetAvgColumnLength(u64 column_id);

    struct Hash {
        inline u64 operator()(const u64 &val) const { return val; }
    };

    u64 total_df_;
    u32 column_counter_{0};
    FlatHashMap<u64, u32, Hash> column_index_map_;
    Vector<u64> column_ids_;
    Vector<Vector<TermDocIterator *>> iterators_;
    Vector<double> column_weights_;
    Vector<double> avg_column_length_;
    UniquePtr<ColumnLengthReader> column_length_reader_;
    MatchData match_data_;
};

} // namespace infinity
