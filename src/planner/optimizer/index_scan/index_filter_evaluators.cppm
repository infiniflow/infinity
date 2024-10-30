//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module index_filter_evaluators;

import stl;
import roaring_bitmap;
import table_index_entry;
import secondary_index_data;
import filter_expression_push_down;
import filter_expression_push_down_helper;
import filter_fulltext_expression;
import base_expression;
import table_entry;
import query_node;
import column_index_reader;
import txn;
import logical_type;
import value;
import doc_iterator;
import parse_fulltext_options;

namespace infinity {

export struct IndexFilterEvaluatorAllTrue : IndexFilterEvaluator {
    IndexFilterEvaluatorAllTrue() : IndexFilterEvaluator(Type::kAllTrue) {}
    Bitmask Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count, Txn *txn) const override {
        Bitmask result(segment_row_count);
        return result;
    }
};

export struct IndexFilterEvaluatorAllFalse : IndexFilterEvaluator {
    IndexFilterEvaluatorAllFalse() : IndexFilterEvaluator(Type::kAllFalse) {}
    Bitmask Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count, Txn *txn) const override {
        Bitmask result(segment_row_count);
        result.SetAllFalse();
        return result;
    }
};

// 1. secondary index
// 2. filter_fulltext
// 3. AND, OR

// maybe combined from multiple exprs
// [start, end] pairs
export struct IndexFilterEvaluatorSecondary : IndexFilterEvaluator {
    Vector<const BaseExpression *> src_filter_secondary_index_expressions_;
    ColumnID column_id_ = std::numeric_limits<ColumnID>::max();
    LogicalType column_logical_type_ = LogicalType::kInvalid;
    const TableIndexEntry *secondary_index_ = nullptr;

    ColumnID column_id() const { return column_id_; }
    virtual bool IsValid() const = 0;
    virtual void Merge(IndexFilterEvaluatorSecondary &other, Type op) = 0;
    static UniquePtr<IndexFilterEvaluatorSecondary> Make(const BaseExpression *src_filter_secondary_index_expressions,
                                                         ColumnID column_id,
                                                         const TableIndexEntry *secondary_index,
                                                         FilterCompareType compare_type,
                                                         const Value &val);

protected:
    IndexFilterEvaluatorSecondary(const BaseExpression *src_expr,
                                  const ColumnID column_id,
                                  const LogicalType column_logical_type,
                                  const TableIndexEntry *secondary_index)
        : IndexFilterEvaluator(Type::kSecondaryIndex), src_filter_secondary_index_expressions_({src_expr}), column_id_(column_id),
          column_logical_type_(column_logical_type), secondary_index_(secondary_index) {}
};

// maybe combined from multiple filter_fulltext exprs
export struct IndexFilterEvaluatorFulltext final : IndexFilterEvaluator {
    Vector<const FilterFulltextExpression *> src_filter_fulltext_expressions_;
    const TableEntry *table_entry_ = nullptr;
    EarlyTermAlgo early_term_algo_ = EarlyTermAlgo::kNaive;
    IndexReader index_reader_;
    UniquePtr<QueryNode> query_tree_;
    MinimumShouldMatchOption minimum_should_match_option_;
    u32 minimum_should_match_ = 0;
    std::atomic_flag after_optimize_ = {};
    f32 score_threshold_ = {};

    IndexFilterEvaluatorFulltext(const FilterFulltextExpression *src_filter_fulltext_expression,
                                 const TableEntry *table_entry,
                                 const EarlyTermAlgo early_term_algo,
                                 IndexReader &&index_reader,
                                 UniquePtr<QueryNode> &&query_tree,
                                 MinimumShouldMatchOption &&minimum_should_match_option,
                                 const f32 score_threshold)
        : IndexFilterEvaluator(Type::kFulltextIndex), src_filter_fulltext_expressions_({src_filter_fulltext_expression}), table_entry_(table_entry),
          early_term_algo_(early_term_algo), index_reader_(std::move(index_reader)), query_tree_(std::move(query_tree)),
          minimum_should_match_option_(std::move(minimum_should_match_option)), score_threshold_(score_threshold) {}
    Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count, Txn *txn) const override;
    bool HaveMinimumShouldMatchOption() const { return !minimum_should_match_option_.empty(); }
    void OptimizeQueryTree();
};

export UniquePtr<IndexFilterEvaluator> IndexFilterEvaluatorBuildFromAnd(Vector<UniquePtr<IndexFilterEvaluator>> candidates);
export UniquePtr<IndexFilterEvaluator> IndexFilterEvaluatorBuildFromOr(Vector<UniquePtr<IndexFilterEvaluator>> candidates);

export struct IndexFilterEvaluatorLogicalChildren : IndexFilterEvaluator {
    // 1. several IndexFilterEvaluatorSecondary from different columns
    Vector<UniquePtr<IndexFilterEvaluatorSecondary>> secondary_index_evaluators_;
    // 2. optional filter_fulltext expr
    UniquePtr<IndexFilterEvaluatorFulltext> fulltext_evaluator_;
    // 3. complex logic
    Vector<UniquePtr<IndexFilterEvaluator>> other_children_evaluators_;

protected:
    using IndexFilterEvaluator::IndexFilterEvaluator;
};

// maybe combined from multiple AND
struct IndexFilterEvaluatorAND final : IndexFilterEvaluatorLogicalChildren {
    IndexFilterEvaluatorAND() : IndexFilterEvaluatorLogicalChildren(Type::kAnd) {}
    Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count, Txn *txn) const override;
};

// maybe combined from multiple OR
struct IndexFilterEvaluatorOR final : IndexFilterEvaluatorLogicalChildren {
    IndexFilterEvaluatorOR() : IndexFilterEvaluatorLogicalChildren(Type::kOr) {}
    Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count, Txn *txn) const override;
};

} // namespace infinity