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

export module query_builder;

import stl;
import doc_iterator;
import column_index_reader;
import match_data;
import table_entry;
import internal_types;
import default_values;
import base_table_ref;

namespace infinity {

struct QueryNode;
export struct FullTextQueryContext {
    UniquePtr<QueryNode> query_tree_;
};

export class QueryBuilder {
public:
    QueryBuilder(TransactionID txn_id, TxnTimeStamp begin_ts, SharedPtr<BaseTableRef> &base_table_ref);

    ~QueryBuilder();

    Map<String, String> &GetColumn2Analyzer() { return column2analyzer_; }

    UniquePtr<DocIterator> CreateSearch(FullTextQueryContext &context);

    void LoadScorerColumnLength() { scorer_->LoadColumnLength(table_entry_, txn_id_, begin_ts_); }

    float Score(RowID doc_id) {
        if (SegmentID segment_id = doc_id.segment_id_; segment_id != target_segment_id_) {
            target_segment_id_ = segment_id;
            scorer_->UpdateTargetSegment(segment_id);
        }
        return scorer_->Score(doc_id);
    }

private:
    TransactionID txn_id_{};
    TxnTimeStamp begin_ts_{};
    TableEntry *table_entry_{nullptr};
    IndexReader index_reader_;
    UniquePtr<Scorer> scorer_;
    SegmentID target_segment_id_ = INVALID_SEGMENT_ID;
    Map<String, String> column2analyzer_;
};
} // namespace infinity