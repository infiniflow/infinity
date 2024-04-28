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
import internal_types;

namespace infinity {
export struct TermColumnMatchData {
    RowID doc_id_;
    tf_t tf_;
    docpayload_t doc_payload_;
};

export struct PhraseColumnMatchData {
    RowID doc_id_;
    tf_t tf_;
    docpayload_t doc_payload_;
    Vector<RowID> begin_positions_;
    Vector<tf_t> all_tf_;
    Vector<docpayload_t> all_doc_payload_;
};

class TermDocIterator;
class BlockMaxTermDocIterator;
class BlockMaxPhraseDocIterator;
class DocIterator;
struct IndexReader;

export class Scorer {
public:
    void Init(u64 num_of_docs, IndexReader *index_reader);

    void AddDocIterator(DocIterator *iter, u64 column_id);

    void AddBlockMaxDocIterator(BlockMaxTermDocIterator *iter, u64 column_id);

    void AddBlockMaxPhraseDocIterator(BlockMaxPhraseDocIterator *iter, u64 column_id);

    float Score(RowID doc_id);

private:
    u32 GetOrSetColumnIndex(u64 column_id);

    struct Hash {
        inline u64 operator()(const u64 &val) const { return val; }
    };

    u64 total_df_{0};
    u32 column_counter_{0};
    FlatHashMap<u64, u32, Hash> column_index_map_;
    Vector<u64> column_ids_;
    Vector<Vector<DocIterator *>> iterators_;
    Vector<Vector<BlockMaxTermDocIterator *>> block_max_term_iterators_;
    Vector<Vector<BlockMaxPhraseDocIterator *>> block_max_phrase_iterators_;
    Vector<float> avg_column_length_;
    ColumnLengthReader column_length_reader_;
    IndexReader *index_reader_ = nullptr;
};

} // namespace infinity
