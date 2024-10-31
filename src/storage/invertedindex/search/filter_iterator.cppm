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

export module filter_iterator;
import stl;
import doc_iterator;
import query_node;
import internal_types;
import index_defines;
import common_query_filter;
import base_expression;
import roaring_bitmap;
import table_entry;
import column_index_reader;

namespace infinity {

export class FilterIterator final : public DocIterator {
public:
    explicit FilterIterator(CommonQueryFilter *common_query_filter, UniquePtr<DocIterator> &&query_iterator)
        : common_query_filter_(common_query_filter), query_iterator_(std::move(query_iterator)) {}

    DocIteratorType GetType() const override { return DocIteratorType::kFilterIterator; }
    String Name() const override { return "FilterIterator"; };

    bool Next(RowID doc_id) override {
        while (true) {
            if (!(query_iterator_->Next(doc_id))) {
                doc_id_ = INVALID_ROWID;
                return false;
            }
            doc_id = query_iterator_->DocID();
            // check filter
            if (common_query_filter_ == nullptr || common_query_filter_->PassFilter(doc_id)) {
                doc_id_ = doc_id;
                return true;
            }
            ++doc_id;
        }
    }

    float Score() override { return query_iterator_->Score(); }

    void UpdateScoreThreshold(const float threshold) override { query_iterator_->UpdateScoreThreshold(threshold); }

    // for minimum_should_match parameter
    u32 MatchCount() const override { return query_iterator_->MatchCount(); }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

private:
    CommonQueryFilter *common_query_filter_{};
    UniquePtr<DocIterator> query_iterator_{};
};

// use QueryNodeType::FILTER
export struct FilterQueryNode final : public QueryNode {
    // search iterator
    UniquePtr<QueryNode> query_tree_{};
    // filter info
    CommonQueryFilter *common_query_filter_{};
    const SizeT filter_result_count_ = common_query_filter_->filter_result_count_;
    const Map<SegmentID, Bitmask> *filter_result_ptr_ = &common_query_filter_->filter_result_;
    const BaseExpression *filter_expression = common_query_filter_->original_filter_.get();

    explicit FilterQueryNode(CommonQueryFilter *common_query_filter, UniquePtr<QueryNode> &&query_tree)
        : QueryNode(QueryNodeType::FILTER), query_tree_(std::move(query_tree)), common_query_filter_(common_query_filter) {}

    void FilterOptimizeQueryTree() override {
        auto new_query_tree = GetOptimizedQueryTree(std::move(query_tree_));
        query_tree_ = std::move(new_query_tree);
    }

    u32 LeafCount() const override { return query_tree_->LeafCount(); }

    void PushDownWeight(const float factor) override { MultiplyWeight(factor); }

    UniquePtr<DocIterator> CreateSearch(const TableEntry *table_entry,
                                        const IndexReader &index_reader,
                                        EarlyTermAlgo early_term_algo,
                                        u32 minimum_should_match) const override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    void GetQueryColumnsTerms(Vector<String> &columns, Vector<String> &terms) const override {}
};

} // namespace infinity
