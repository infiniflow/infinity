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

#include <cassert>
#include <vector>
module index_filter_evaluators;

import stl;
import roaring_bitmap;
import secondary_index_data;
import secondary_index_in_mem;
import buffer_obj;
import buffer_handle;
import filter_expression_push_down;
import filter_fulltext_expression;
import query_node;
import column_index_reader;
import infinity_exception;
import status;
import internal_types;
import third_party;
import doc_iterator;
import score_threshold_iterator;
import column_def;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;
import mem_index;

namespace infinity {

// all cases for candidates:
//
// case IndexFilterEvaluator::Type::kAnd:
// case IndexFilterEvaluator::Type::kOr:
// case IndexFilterEvaluator::Type::kFulltextIndex:
// case IndexFilterEvaluator::Type::kSecondaryIndex:

void AddToFulltextEvaluator(UniquePtr<IndexFilterEvaluatorFulltext> &target_fulltext_evaluator,
                            Vector<UniquePtr<IndexFilterEvaluator>> &other_children_evaluators,
                            UniquePtr<IndexFilterEvaluatorFulltext> input,
                            const IndexFilterEvaluator::Type op) {
    if (!target_fulltext_evaluator) {
        target_fulltext_evaluator = std::move(input);
    } else {
        if (target_fulltext_evaluator->HaveMinimumShouldMatchOption() || input->HaveMinimumShouldMatchOption() ||
            target_fulltext_evaluator->score_threshold_ > 0.0f || input->score_threshold_ > 0.0f ||
            target_fulltext_evaluator->early_term_algo_ != input->early_term_algo_ ||
            target_fulltext_evaluator->ft_similarity_ != input->ft_similarity_) {
            // put into others
            other_children_evaluators.push_back(std::move(input));
        } else {
            target_fulltext_evaluator->src_filter_fulltext_expressions_.insert(target_fulltext_evaluator->src_filter_fulltext_expressions_.end(),
                                                                               input->src_filter_fulltext_expressions_.begin(),
                                                                               input->src_filter_fulltext_expressions_.end());
            if (op == IndexFilterEvaluator::Type::kAnd) {
                // merge two node
                auto new_node = MakeUnique<AndQueryNode>();
                new_node->Add(std::move(target_fulltext_evaluator->query_tree_));
                new_node->Add(std::move(input->query_tree_));
                target_fulltext_evaluator->query_tree_ = std::move(new_node);
            } else if (op == IndexFilterEvaluator::Type::kOr) {
                // merge two node
                auto new_node = MakeUnique<OrQueryNode>();
                new_node->Add(std::move(target_fulltext_evaluator->query_tree_));
                new_node->Add(std::move(input->query_tree_));
                target_fulltext_evaluator->query_tree_ = std::move(new_node);
            } else {
                UnrecoverableError("Wrong type of query node");
            }
        }
    }
}

UniquePtr<IndexFilterEvaluatorSecondary> MergeSameColumn(Vector<UniquePtr<IndexFilterEvaluatorSecondary>> &evaluators,
                                                         const IndexFilterEvaluator::Type op) {
    assert(evaluators.size() > 0);
    UniquePtr<IndexFilterEvaluatorSecondary> result = std::move(evaluators[0]);
    for (size_t i = 1; i < evaluators.size(); ++i) {
        result->Merge(*evaluators[i], op);
        if (!result->IsValid()) {
            return {};
        }
    }
    return result;
}

// return:
// true: use evaluators
// false: return all true / all false according to op
bool SimplifySecondaryIndexEvaluators(Vector<UniquePtr<IndexFilterEvaluatorSecondary>> &evaluators, const IndexFilterEvaluator::Type op) {
    if (evaluators.empty()) {
        return true;
    }
    Map<ColumnID, Vector<UniquePtr<IndexFilterEvaluatorSecondary>>> classify_evaluators;
    for (auto &evaluator : evaluators) {
        const auto column_id = evaluator->column_id();
        classify_evaluators[column_id].push_back(std::move(evaluator));
    }
    evaluators.clear();
    for (auto &[k, v] : classify_evaluators) {
        auto sub_result = MergeSameColumn(v, op);
        if (!sub_result) {
            return false;
        }
        evaluators.push_back(std::move(sub_result));
    }
    return true;
}

UniquePtr<IndexFilterEvaluator> IndexFilterEvaluatorBuildFromAnd(Vector<UniquePtr<IndexFilterEvaluator>> candidates) {
    using Type = IndexFilterEvaluator::Type;
    constexpr auto combine_type = Type::kAnd;
    assert(candidates.size() == 2);
    // 1. several IndexFilterEvaluatorSecondary from different columns
    Vector<UniquePtr<IndexFilterEvaluatorSecondary>> secondary_index_evaluators;
    // 2. optional filter_fulltext expr
    UniquePtr<IndexFilterEvaluatorFulltext> fulltext_evaluator;
    // 3. complex logic
    Vector<UniquePtr<IndexFilterEvaluator>> other_children_evaluators;

    for (auto &child : candidates) {
        switch (child->type()) {
            case Type::kAllTrue:
            case Type::kAllFalse:
            case Type::kInvalid: {
                UnrecoverableError("Check type first!");
                break;
            }
            case Type::kSecondaryIndex: {
                secondary_index_evaluators.emplace_back(static_cast<IndexFilterEvaluatorSecondary *>(child.release()));
                break;
            }
            case Type::kFulltextIndex: {
                AddToFulltextEvaluator(fulltext_evaluator,
                                       other_children_evaluators,
                                       UniquePtr<IndexFilterEvaluatorFulltext>{static_cast<IndexFilterEvaluatorFulltext *>(child.release())},
                                       combine_type);
                break;
            }
            case Type::kAnd: {
                // flatten all children
                UniquePtr<IndexFilterEvaluatorLogicalChildren> child_logical(static_cast<IndexFilterEvaluatorLogicalChildren *>(child.release()));
                for (auto &chi : child_logical->secondary_index_evaluators_) {
                    secondary_index_evaluators.push_back(std::move(chi));
                }
                if (child_logical->fulltext_evaluator_) {
                    AddToFulltextEvaluator(fulltext_evaluator,
                                           other_children_evaluators,
                                           std::move(child_logical->fulltext_evaluator_),
                                           combine_type);
                }
                for (auto &chi : child_logical->other_children_evaluators_) {
                    other_children_evaluators.push_back(std::move(chi));
                }
            }
            case Type::kOr: {
                other_children_evaluators.push_back(std::move(child));
            }
        }
    }
    if (!SimplifySecondaryIndexEvaluators(secondary_index_evaluators, combine_type)) {
        return MakeUnique<IndexFilterEvaluatorAllFalse>();
    }
    if (secondary_index_evaluators.empty() && other_children_evaluators.empty() && !fulltext_evaluator) {
        UnrecoverableError("Wrong status!");
    }
    if (secondary_index_evaluators.empty() && other_children_evaluators.empty()) {
        return fulltext_evaluator;
    }
    if (!fulltext_evaluator) {
        if (secondary_index_evaluators.empty() && other_children_evaluators.size() == 1) {
            return std::move(other_children_evaluators.front());
        }
        if (secondary_index_evaluators.size() == 1 && other_children_evaluators.empty()) {
            return std::move(secondary_index_evaluators.front());
        }
    }
    auto result = MakeUnique<IndexFilterEvaluatorAND>();
    result->secondary_index_evaluators_ = std::move(secondary_index_evaluators);
    result->fulltext_evaluator_ = std::move(fulltext_evaluator);
    result->other_children_evaluators_ = std::move(other_children_evaluators);
    return result;
}

UniquePtr<IndexFilterEvaluator> IndexFilterEvaluatorBuildFromOr(Vector<UniquePtr<IndexFilterEvaluator>> candidates) {
    using Type = IndexFilterEvaluator::Type;
    constexpr auto combine_type = Type::kOr;
    assert(candidates.size() == 2);
    // 1. several IndexFilterEvaluatorSecondary from different columns
    Vector<UniquePtr<IndexFilterEvaluatorSecondary>> secondary_index_evaluators;
    // 2. optional filter_fulltext expr
    UniquePtr<IndexFilterEvaluatorFulltext> fulltext_evaluator;
    // 3. complex logic
    Vector<UniquePtr<IndexFilterEvaluator>> other_children_evaluators;

    for (auto &child : candidates) {
        switch (child->type()) {
            case Type::kAllTrue:
            case Type::kAllFalse:
            case Type::kInvalid: {
                UnrecoverableError("Check type first!");
                break;
            }
            case Type::kSecondaryIndex: {
                secondary_index_evaluators.emplace_back(static_cast<IndexFilterEvaluatorSecondary *>(child.release()));
                break;
            }
            case Type::kFulltextIndex: {
                AddToFulltextEvaluator(fulltext_evaluator,
                                       other_children_evaluators,
                                       UniquePtr<IndexFilterEvaluatorFulltext>{static_cast<IndexFilterEvaluatorFulltext *>(child.release())},
                                       combine_type);
                break;
            }
            case Type::kOr: {
                // flatten all children
                UniquePtr<IndexFilterEvaluatorLogicalChildren> child_logical(static_cast<IndexFilterEvaluatorLogicalChildren *>(child.release()));
                for (auto &chi : child_logical->secondary_index_evaluators_) {
                    secondary_index_evaluators.push_back(std::move(chi));
                }
                if (child_logical->fulltext_evaluator_) {
                    AddToFulltextEvaluator(fulltext_evaluator,
                                           other_children_evaluators,
                                           std::move(child_logical->fulltext_evaluator_),
                                           combine_type);
                }
                for (auto &chi : child_logical->other_children_evaluators_) {
                    other_children_evaluators.push_back(std::move(chi));
                }
            }
            case Type::kAnd: {
                other_children_evaluators.push_back(std::move(child));
            }
        }
    }
    if (!SimplifySecondaryIndexEvaluators(secondary_index_evaluators, combine_type)) {
        return MakeUnique<IndexFilterEvaluatorAllTrue>();
    }
    if (secondary_index_evaluators.empty() && other_children_evaluators.empty() && !fulltext_evaluator) {
        UnrecoverableError("Wrong status!");
    }
    if (secondary_index_evaluators.empty() && other_children_evaluators.empty()) {
        return fulltext_evaluator;
    }
    if (!fulltext_evaluator) {
        if (secondary_index_evaluators.empty() && other_children_evaluators.size() == 1) {
            return std::move(other_children_evaluators.front());
        }
        if (secondary_index_evaluators.size() == 1 && other_children_evaluators.empty()) {
            return std::move(secondary_index_evaluators.front());
        }
    }
    auto result = MakeUnique<IndexFilterEvaluatorOR>();
    result->secondary_index_evaluators_ = std::move(secondary_index_evaluators);
    result->fulltext_evaluator_ = std::move(fulltext_evaluator);
    result->other_children_evaluators_ = std::move(other_children_evaluators);
    return result;
}

template <typename ColumnValueT>
ConvertToOrderedType<ColumnValueT> GetOrderedV(const Value &val) {
    return ConvertToOrderedKeyValue(val.GetValue<ColumnValueT>());
}

template <>
ConvertToOrderedType<VarcharT> GetOrderedV<VarcharT>(const Value &val) {
    const String &str = val.GetVarchar();
    return ConvertToOrderedKeyValue(static_cast<std::string_view>(str));
}

// 1. secondary index
// 2. filter_fulltext
// 3. AND, OR

// maybe combined from multiple exprs
// [start, end] pairs
template <typename ColumnValueT>
struct IndexFilterEvaluatorSecondaryT final : IndexFilterEvaluatorSecondary {
    using SecondaryIndexOrderedT = ConvertToOrderedType<ColumnValueT>;

    Vector<Pair<SecondaryIndexOrderedT, SecondaryIndexOrderedT>> secondary_index_start_end_pairs_;

    Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count) const override;

    bool IsValid() const override { return !secondary_index_start_end_pairs_.empty(); }

    void Merge(IndexFilterEvaluatorSecondary &other, const Type op) override {
        assert(column_logical_type_ == other.column_logical_type_);
        assert(column_id_ == other.column_id_);
        if (column_id() != other.column_id()) [[unlikely]] {
            UnrecoverableError("Invalid Merge! Different column id.");
        }
        src_filter_secondary_index_expressions_.insert(src_filter_secondary_index_expressions_.end(),
                                                       other.src_filter_secondary_index_expressions_.begin(),
                                                       other.src_filter_secondary_index_expressions_.end());
        const auto &ot = static_cast<const IndexFilterEvaluatorSecondaryT &>(other);
        const auto &other_start_end_pairs = ot.secondary_index_start_end_pairs_;
        if (secondary_index_start_end_pairs_.empty() || other_start_end_pairs.empty()) {
            UnrecoverableError("Invalid Merge input!");
        }
        Vector<Pair<SecondaryIndexOrderedT, SecondaryIndexOrderedT>> new_start_end_pairs;
        auto self_it = secondary_index_start_end_pairs_.cbegin();
        auto other_it = other_start_end_pairs.cbegin();
        const auto self_end = secondary_index_start_end_pairs_.cend();
        const auto other_end = other_start_end_pairs.cend();
        if (op == Type::kOr) {
            auto back_v = std::min(*self_it, *other_it);
            while (self_it != self_end || other_it != other_end) {
                bool merge_self = true;
                if (self_it == self_end || (other_it != other_end && *other_it < *self_it)) {
                    merge_self = false;
                }
                const auto to_merge = merge_self ? *self_it : *other_it;
                if (to_merge.first > back_v.second) {
                    new_start_end_pairs.push_back(back_v);
                    back_v = to_merge;
                } else {
                    back_v.second = std::max(back_v.second, to_merge.second);
                }
                if (merge_self) {
                    ++self_it;
                } else {
                    ++other_it;
                }
            }
            // final element
            const auto full_range_v = Pair<SecondaryIndexOrderedT, SecondaryIndexOrderedT>{std::numeric_limits<SecondaryIndexOrderedT>::lowest(),
                                                                                           std::numeric_limits<SecondaryIndexOrderedT>::max()};
            if (back_v != full_range_v) {
                new_start_end_pairs.push_back(back_v);
            }
        } else if (op == Type::kAnd) {
            while (self_it != self_end && other_it != other_end) {
                const auto [s1, s2] = *self_it;
                const auto [o1, o2] = *other_it;
                if (s2 < o1) {
                    ++self_it;
                    continue;
                }
                if (o2 < s1) {
                    ++other_it;
                    continue;
                }
                // now o1 <= s2, s1 <= o2
                new_start_end_pairs.emplace_back(std::max(s1, o1), std::min(s2, o2));
                if (s2 <= o2) {
                    ++self_it;
                } else {
                    ++other_it;
                }
            }
        } else {
            UnrecoverableError("Invalid Merge! Type can only be And / Or");
        }
        secondary_index_start_end_pairs_ = std::move(new_start_end_pairs);
    }

    IndexFilterEvaluatorSecondaryT(const BaseExpression *src_expr,
                                   const ColumnID column_id,
                                   const LogicalType column_logical_type,
                                   SharedPtr<TableIndexMeeta> new_secondary_index)
        : IndexFilterEvaluatorSecondary(src_expr, column_id, column_logical_type, new_secondary_index) {}

    static UniquePtr<IndexFilterEvaluatorSecondaryT> Make(const BaseExpression *src_expr,
                                                          const ColumnID column_id,
                                                          SharedPtr<TableIndexMeeta> new_secondary_index,
                                                          const FilterCompareType compare_type,
                                                          const Value &val) {
        constexpr auto expect_logical_type = GetLogicalType<ColumnValueT>;
        auto result = MakeUnique<IndexFilterEvaluatorSecondaryT>(src_expr, column_id, expect_logical_type, new_secondary_index);
        const SecondaryIndexOrderedT val_ordered = GetOrderedV<ColumnValueT>(val);
        switch (compare_type) {
            case FilterCompareType::kEqual: {
                result->secondary_index_start_end_pairs_.emplace_back(val_ordered, val_ordered);
                break;
            }
            case FilterCompareType::kGreaterEqual: {
                result->secondary_index_start_end_pairs_.emplace_back(val_ordered, std::numeric_limits<SecondaryIndexOrderedT>::max());
                break;
            }
            case FilterCompareType::kLessEqual: {
                result->secondary_index_start_end_pairs_.emplace_back(std::numeric_limits<SecondaryIndexOrderedT>::lowest(), val_ordered);
                break;
            }
            default: {
                UnrecoverableError("Wrong comparison type");
            }
        }
        return result;
    }
};

UniquePtr<IndexFilterEvaluatorSecondary> IndexFilterEvaluatorSecondary::Make(const BaseExpression *src_expr,
                                                                             ColumnID column_id,
                                                                             SharedPtr<TableIndexMeeta> new_secondary_index,
                                                                             FilterCompareType compare_type,
                                                                             const Value &val) {
    ColumnDef *column_def;
    auto [column_def_ptr, status] = new_secondary_index->GetColumnDef();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    column_def = column_def_ptr.get();
    if (column_def->id() != static_cast<i64>(column_id)) {
        UnrecoverableError("Invalid column id");
    }
    switch (column_def->type()->type()) {
        case LogicalType::kTinyInt: {
            return IndexFilterEvaluatorSecondaryT<TinyIntT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kSmallInt: {
            return IndexFilterEvaluatorSecondaryT<SmallIntT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kInteger: {
            return IndexFilterEvaluatorSecondaryT<IntegerT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kBigInt: {
            return IndexFilterEvaluatorSecondaryT<BigIntT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kFloat: {
            return IndexFilterEvaluatorSecondaryT<FloatT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kDouble: {
            return IndexFilterEvaluatorSecondaryT<DoubleT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kDate: {
            return IndexFilterEvaluatorSecondaryT<DateT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kTime: {
            return IndexFilterEvaluatorSecondaryT<TimeT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kDateTime: {
            return IndexFilterEvaluatorSecondaryT<DateTimeT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kTimestamp: {
            return IndexFilterEvaluatorSecondaryT<TimestampT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
        }
        case LogicalType::kVarchar: {
            if (compare_type == FilterCompareType::kEqual) {
                return IndexFilterEvaluatorSecondaryT<VarcharT>::Make(src_expr, column_id, new_secondary_index, compare_type, val);
            }
            RecoverableError(Status::SyntaxError("VarcharT only support kEqual compare type in secondary index."));
            return {};
        }
        default: {
            UnrecoverableError(fmt::format("Unexpected type for secondary index: {}", column_def->type()->ToString()));
            return {};
        }
    }
}

void IndexFilterEvaluatorFulltext::OptimizeQueryTree() {
    if (after_optimize_.test(std::memory_order_acquire)) {
        UnrecoverableError(std::format("{}: Already optimized!", __func__));
    }
    auto new_query_tree = QueryNode::GetOptimizedQueryTree(std::move(query_tree_));
    query_tree_ = std::move(new_query_tree);
    if (!minimum_should_match_option_.empty()) {
        const auto leaf_count = query_tree_->LeafCount();
        minimum_should_match_ = GetMinimumShouldMatchParameter(minimum_should_match_option_, leaf_count);
    }
    after_optimize_.test_and_set(std::memory_order_release);
}

Bitmask IndexFilterEvaluatorFulltext::Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count) const {
    if (!after_optimize_.test(std::memory_order_acquire)) {
        UnrecoverableError(std::format("{}: Not optimized!", __func__));
    }
    Bitmask result(segment_row_count);
    result.SetAllFalse();
    const RowID begin_rowid(segment_id, 0);
    const RowID end_rowid(segment_id, segment_row_count);
    const CreateSearchParams
        params{table_info_, index_reader_.get(), early_term_algo_, ft_similarity_, bm25_params_, minimum_should_match_, 0u, index_names_};
    auto ft_iter = query_tree_->CreateSearch(params);
    if (ft_iter && score_threshold_ > 0.0f) {
        auto new_ft_iter = MakeUnique<ScoreThresholdIterator>(std::move(ft_iter), score_threshold_);
        ft_iter = std::move(new_ft_iter);
    }
    if (ft_iter && ft_iter->Next(begin_rowid)) {
        while (ft_iter->DocID() < end_rowid) {
            result.SetTrue(ft_iter->DocID().segment_offset_);
            ft_iter->Next();
        }
        result.RunOptimize();
    }
    return result;
}

Bitmask IndexFilterEvaluatorAND::Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count) const {
    Bitmask result(segment_row_count);
    // 1.
    for (const auto &child : secondary_index_evaluators_) {
        const auto child_result = child->Evaluate(segment_id, segment_row_count);
        result.MergeAnd(child_result);
        if (result.CountTrue() == 0) {
            // empty now
            return result;
        }
    }
    // 2.
    if (fulltext_evaluator_) {
        if (result.IsAllTrue()) {
            result = fulltext_evaluator_->Evaluate(segment_id, segment_row_count);
        } else {
            auto roaring_begin = result.Begin();
            const auto &roaring_end = result.End();
            Bitmask new_result(segment_row_count);
            new_result.SetAllFalse();
            if (!fulltext_evaluator_->after_optimize_.test(std::memory_order_acquire)) {
                UnrecoverableError(std::format("{}: Not optimized!", __func__));
            }
            const CreateSearchParams params{fulltext_evaluator_->table_info_,
                                            fulltext_evaluator_->index_reader_.get(),
                                            fulltext_evaluator_->early_term_algo_,
                                            fulltext_evaluator_->ft_similarity_,
                                            fulltext_evaluator_->bm25_params_,
                                            fulltext_evaluator_->minimum_should_match_,
                                            0u,
                                            fulltext_evaluator_->index_names_};
            auto ft_iter = fulltext_evaluator_->query_tree_->CreateSearch(params);
            if (ft_iter && fulltext_evaluator_->score_threshold_ > 0.0f) {
                auto new_ft_iter = MakeUnique<ScoreThresholdIterator>(std::move(ft_iter), fulltext_evaluator_->score_threshold_);
                ft_iter = std::move(new_ft_iter);
            }
            if (ft_iter) {
                const RowID end_rowid(segment_id, segment_row_count);
                while (roaring_begin != roaring_end && ft_iter->Next(RowID(segment_id, *roaring_begin))) {
                    const RowID ft_pos = ft_iter->DocID();
                    if (ft_pos >= end_rowid) {
                        break;
                    }
                    if (ft_pos.segment_offset_ == *roaring_begin) {
                        new_result.SetTrue(*roaring_begin);
                        ++roaring_begin;
                    } else {
                        assert((ft_pos.segment_offset_ > *roaring_begin));
                        roaring_begin.equalorlarger(ft_pos.segment_offset_);
                    }
                }
            }
            result = std::move(new_result);
            result.RunOptimize();
        }
    }
    // 3. complex part
    for (const auto &child : other_children_evaluators_) {
        if (result.CountTrue() == 0) {
            // empty now
            return result;
        }
        const auto child_result = child->Evaluate(segment_id, segment_row_count);
        result.MergeAnd(child_result);
    }
    return result;
}

Bitmask IndexFilterEvaluatorOR::Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count) const {
    Bitmask result(segment_row_count);
    result.SetAllFalse();
    // 1.
    for (const auto &child : secondary_index_evaluators_) {
        const auto child_result = child->Evaluate(segment_id, segment_row_count);
        result.MergeOr(child_result);
        if (result.CountFalse() == 0) {
            // full now
            result.RunOptimize();
            return result;
        }
    }
    // 2.
    if (fulltext_evaluator_) {
        const auto child_result = fulltext_evaluator_->Evaluate(segment_id, segment_row_count);
        result.MergeOr(child_result);
    }
    // 3. complex part
    for (const auto &child : other_children_evaluators_) {
        if (result.CountFalse() == 0) {
            // full now
            result.RunOptimize();
            return result;
        }
        const auto child_result = child->Evaluate(segment_id, segment_row_count);
        result.MergeOr(child_result);
    }
    result.RunOptimize();
    return result;
}

template <typename ColumnValueType, typename CardinalityTag>
struct TrunkReader {
    using SecondaryIndexOrderedT = ConvertToOrderedType<ColumnValueType>;
    virtual ~TrunkReader() = default;
    virtual void OutPut(Bitmask &selected_rows) = 0;
};

// Specialization for HighCardinalityTag - includes GetResultCnt
template <typename ColumnValueType>
struct TrunkReader<ColumnValueType, HighCardinalityTag> {
    using SecondaryIndexOrderedT = ConvertToOrderedType<ColumnValueType>;
    virtual ~TrunkReader() = default;
    virtual u32 GetResultCnt(Pair<SecondaryIndexOrderedT, SecondaryIndexOrderedT> interval_range) = 0;
    virtual void OutPut(Bitmask &selected_rows) = 0;
};

template <typename ColumnValueType, typename CardinalityTag>
struct TrunkReaderT final : TrunkReader<ColumnValueType, CardinalityTag> {
    using KeyType = typename TrunkReader<ColumnValueType, CardinalityTag>::SecondaryIndexOrderedT;
    const u32 segment_row_count_;
    BufferObj *index_buffer_ = nullptr;
    const SecondaryIndexDataBase<CardinalityTag> *index_ = nullptr;
    u32 begin_pos_ = 0;
    u32 end_pos_ = 0;
    // For LowCardinality: store the range for processing
    Pair<KeyType, KeyType> current_range_;

    TrunkReaderT(const u32 segment_row_count, BufferObj *index_buffer) : segment_row_count_(segment_row_count), index_buffer_(index_buffer) {}
    TrunkReaderT(const u32 segment_row_count, const SecondaryIndexDataBase<CardinalityTag> *index)
        : segment_row_count_(segment_row_count), index_(index) {}

    // Method to set range for LowCardinality processing
    void SetRange(const Pair<KeyType, KeyType> &range) { current_range_ = range; }

    void OutPut(Bitmask &selected_rows) override {
        if constexpr (std::is_same_v<CardinalityTag, HighCardinalityTag>) {
            // High cardinality: use traditional approach
            const u32 begin_pos = begin_pos_;
            const u32 end_pos = end_pos_;
            const auto index_handle = index_buffer_->Load();
            const auto index = static_cast<const SecondaryIndexDataBase<CardinalityTag> *>(index_handle.GetData());
            const auto [key_ptr, offset_ptr] = index->GetKeyOffsetPointer();
            // output result
            for (u32 i = begin_pos; i < end_pos; ++i) {
                selected_rows.SetTrue(offset_ptr[i]);
            }
        } else {
            // Low cardinality: use RoaringBitmap approach
            const auto [begin_val, end_val] = current_range_;
            const auto index_handle = index_buffer_->Load();
            const auto index = static_cast<const SecondaryIndexDataBase<CardinalityTag> *>(index_handle.GetData());

            // Get unique keys count and pointer through base class interface
            u32 unique_key_count = index->GetUniqueKeyCount();
            if (unique_key_count == 0)
                return;

            const KeyType *unique_keys = static_cast<const KeyType *>(index->GetUniqueKeysPtr());

            // Find keys in range [begin_val, end_val]
            auto begin_it = std::lower_bound(unique_keys, unique_keys + unique_key_count, begin_val);
            auto end_it = std::upper_bound(unique_keys, unique_keys + unique_key_count, end_val);

            // For each key in range, add its offsets to the result
            for (auto it = begin_it; it != end_it; ++it) {
                const auto *bitmap = static_cast<const Bitmap *>(index->GetOffsetsForKeyPtr(it));
                if (bitmap) {
                    bitmap->RoaringBitmapApplyFunc([&selected_rows](u32 offset) -> bool {
                        selected_rows.SetTrue(offset);
                        return true; // continue iteration
                    });
                }
            }
        }
    }
};

// Specialization for HighCardinalityTag that includes GetResultCnt
template <typename ColumnValueType>
struct TrunkReaderT<ColumnValueType, HighCardinalityTag> final : TrunkReader<ColumnValueType, HighCardinalityTag> {
    using KeyType = typename TrunkReader<ColumnValueType, HighCardinalityTag>::SecondaryIndexOrderedT;
    const u32 segment_row_count_;
    BufferObj *index_buffer_ = nullptr;
    const SecondaryIndexDataBase<HighCardinalityTag> *index_ = nullptr;
    u32 begin_pos_ = 0;
    u32 end_pos_ = 0;
    TrunkReaderT(const u32 segment_row_count, BufferObj *index_buffer) : segment_row_count_(segment_row_count), index_buffer_(index_buffer) {}
    TrunkReaderT(const u32 segment_row_count, const SecondaryIndexDataBase<HighCardinalityTag> *index)
        : segment_row_count_(segment_row_count), index_(index) {}

    u32 GetResultCnt(const Pair<KeyType, KeyType> interval_range) override {
        Optional<BufferHandle> index_handle;
        const SecondaryIndexDataBase<HighCardinalityTag> *index = nullptr;
        if (index_buffer_) {
            index_handle = index_buffer_->Load();
            index = static_cast<const SecondaryIndexDataBase<HighCardinalityTag> *>(index_handle->GetData());
        } else {
            index = index_;
        }
        const u32 index_data_num = index->GetChunkRowCount();
        const auto [begin_val, end_val] = interval_range;
        // 1. search PGM and get approximate search range
        // result:
        //    1. size_t pos_;         ///< The approximate position of the key.
        //    2. size_t lower_bound_; ///< The lower bound of the range.
        //    3. size_t upper_bound_; ///< The upper bound of the range.
        // NOTICE: PGM return a range [lower_bound_, upper_bound_) which must include **one** key when the key exists
        // NOTICE: but the range may not include the complete [start, end] range
        const auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        const auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        u32 begin_pos = begin_lower;
        u32 end_pos = std::min<u32>(end_upper, index_data_num - 1);
        if (end_pos < begin_pos) {
            return 0;
        }
        const auto [key_ptr, offset_ptr] = index->GetKeyOffsetPointer();
        auto index_key_ptr = [key_ptr](const u32 i) -> KeyType {
            KeyType key{};
            std::memcpy(&key, static_cast<const char *>(key_ptr) + i * sizeof(KeyType), sizeof(KeyType));
            return key;
        };
        // 2. find the exact range
        // 2.1 find the exact begin_pos which is the first position that index_key >= begin_val
        if (index_key_ptr(begin_pos) < begin_val) {
            // search forward
            while (index_key_ptr(begin_pos) < begin_val) {
                if (++begin_pos == index_data_num) {
                    // nothing found
                    return 0;
                }
            }
        } else {
            // search backward
            auto test_begin_pos = begin_pos;
            while (index_key_ptr(test_begin_pos) >= begin_val) {
                // keep valid begin_pos
                begin_pos = test_begin_pos;
                if (test_begin_pos-- == 0) {
                    // left bound is the leftmost
                    break;
                }
            }
        }
        // 2.2 find the exact end_pos which is the first position that index_key > end_val (or the position past the end)
        if (index_key_ptr(end_pos) <= end_val) {
            // search forward
            while (index_key_ptr(end_pos) <= end_val) {
                if (++end_pos == index_data_num) {
                    // right bound is the rightmost
                    break;
                }
            }
        } else {
            // search backward
            auto test_end_pos = end_pos;
            while (index_key_ptr(test_end_pos) > end_val) {
                end_pos = test_end_pos;
                if (test_end_pos-- == 0) {
                    // nothing found
                    return 0;
                }
            }
        }
        // 3. now we know result size
        if (end_pos <= begin_pos) {
            // nothing found
            return 0;
        }
        // have result
        begin_pos_ = begin_pos;
        end_pos_ = end_pos;
        const u32 result_size = end_pos - begin_pos;
        return result_size;
    }

    void OutPut(Bitmask &selected_rows) override {
        const u32 begin_pos = begin_pos_;
        const u32 end_pos = end_pos_;
        const auto index_handle = index_buffer_->Load();
        const auto index = static_cast<const SecondaryIndexDataBase<HighCardinalityTag> *>(index_handle.GetData());
        const auto [key_ptr, offset_ptr] = index->GetKeyOffsetPointer();
        // output result
        for (u32 i = begin_pos; i < end_pos; ++i) {
            selected_rows.SetTrue(offset_ptr[i]);
        }
    }
};

template <typename ColumnValueType, typename CardinalityTag>
struct TrunkReaderM final : TrunkReader<ColumnValueType, CardinalityTag> {
    using KeyType = typename TrunkReader<ColumnValueType, CardinalityTag>::SecondaryIndexOrderedT;
    const u32 segment_row_count_;
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_;
    Pair<u32, Bitmask> result_cache_;
    TrunkReaderM(const u32 segment_row_count, const SharedPtr<SecondaryIndexInMem> &memory_secondary_index)
        : segment_row_count_(segment_row_count), memory_secondary_index_(memory_secondary_index) {}
    void OutPut(Bitmask &selected_rows) override { selected_rows.MergeOr(result_cache_.second); }
};

// Specialization for HighCardinalityTag that includes GetResultCnt
template <typename ColumnValueType>
struct TrunkReaderM<ColumnValueType, HighCardinalityTag> final : TrunkReader<ColumnValueType, HighCardinalityTag> {
    using KeyType = typename TrunkReader<ColumnValueType, HighCardinalityTag>::SecondaryIndexOrderedT;
    const u32 segment_row_count_;
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_;
    Pair<u32, Bitmask> result_cache_;
    TrunkReaderM(const u32 segment_row_count, const SharedPtr<SecondaryIndexInMem> &memory_secondary_index)
        : segment_row_count_(segment_row_count), memory_secondary_index_(memory_secondary_index) {}
    u32 GetResultCnt(const Pair<KeyType, KeyType> interval_range) override {
        const auto [begin_val, end_val] = interval_range;
        Tuple<u32, KeyType, KeyType> arg_tuple = {segment_row_count_, begin_val, end_val};
        result_cache_ = memory_secondary_index_->RangeQuery(&arg_tuple);
        return result_cache_.first;
    }
    void OutPut(Bitmask &selected_rows) override { selected_rows.MergeOr(result_cache_.second); }
};

template <typename ColumnValueType>
Bitmask ExecuteSingleRangeHighCardinalityT(const Pair<ConvertToOrderedType<ColumnValueType>, ConvertToOrderedType<ColumnValueType>> interval_range,
                                           SegmentIndexMeta *index_meta,
                                           const SegmentOffset segment_row_count) {
    Vector<UniquePtr<TrunkReader<ColumnValueType, HighCardinalityTag>>> trunk_readers;
    auto [chunk_ids_ptr, status] = index_meta->GetChunkIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *index_meta);
        BufferObj *index_buffer = nullptr;
        Status status = chunk_index_meta.GetIndexBuffer(index_buffer);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        trunk_readers.emplace_back(MakeUnique<TrunkReaderT<ColumnValueType, HighCardinalityTag>>(segment_row_count, index_buffer));
    }
    SharedPtr<MemIndex> mem_index = index_meta->GetMemIndex();
    if (mem_index) {
        SharedPtr<SecondaryIndexInMem> secondary_index = mem_index->GetSecondaryIndex();
        if (secondary_index) {
            trunk_readers.emplace_back(MakeUnique<TrunkReaderM<ColumnValueType, HighCardinalityTag>>(segment_row_count, secondary_index));
        }
    }

    // output result
    Bitmask part_result(segment_row_count);
    part_result.SetAllFalse();
    for (auto &trunk_reader : trunk_readers) {
        // real calculation is done here
        if (const auto part_result_size = trunk_reader->GetResultCnt(interval_range); part_result_size) {
            trunk_reader->OutPut(part_result);
        }
    }
    part_result.RunOptimize();
    return part_result;
}

template <typename ColumnValueType>
Bitmask ExecuteSingleRangeLowCardinalityT(const Pair<ConvertToOrderedType<ColumnValueType>, ConvertToOrderedType<ColumnValueType>> interval_range,
                                          SegmentIndexMeta *index_meta,
                                          const SegmentOffset segment_row_count) {
    Vector<UniquePtr<TrunkReader<ColumnValueType, LowCardinalityTag>>> trunk_readers;
    auto [chunk_ids_ptr, status] = index_meta->GetChunkIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *index_meta);
        BufferObj *index_buffer = nullptr;
        Status status = chunk_index_meta.GetIndexBuffer(index_buffer);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        trunk_readers.emplace_back(MakeUnique<TrunkReaderT<ColumnValueType, LowCardinalityTag>>(segment_row_count, index_buffer));
    }
    SharedPtr<MemIndex> mem_index = index_meta->GetMemIndex();
    if (mem_index) {
        SharedPtr<SecondaryIndexInMem> secondary_index = mem_index->GetSecondaryIndex();
        if (secondary_index) {
            trunk_readers.emplace_back(MakeUnique<TrunkReaderM<ColumnValueType, LowCardinalityTag>>(segment_row_count, secondary_index));
        }
    }

    // output result - for LowCardinality, we don't use GetResultCnt optimization
    Bitmask part_result(segment_row_count);
    part_result.SetAllFalse();
    for (auto &trunk_reader : trunk_readers) {
        // Set the range for LowCardinality processing
        auto *typed_reader = static_cast<TrunkReaderT<ColumnValueType, LowCardinalityTag> *>(trunk_reader.get());
        typed_reader->SetRange(interval_range);
        // directly output without GetResultCnt optimization
        trunk_reader->OutPut(part_result);
    }
    part_result.RunOptimize();
    return part_result;
}

template <typename ColumnValueT>
Bitmask IndexFilterEvaluatorSecondaryT<ColumnValueT>::Evaluate(const SegmentID segment_id, const SegmentOffset segment_row_count) const {
    Optional<SegmentIndexMeta> index_meta;
    index_meta.emplace(segment_id, *new_secondary_index_);

    // Check cardinality to determine which execution path to use
    auto [cardinality, status] = new_secondary_index_->GetSecondaryIndexCardinality();
    if (!status.ok()) {
        // Default to HighCardinality if unable to determine
        cardinality = SecondaryIndexCardinality::kHighCardinality;
    }

    Bitmask result(segment_row_count);
    result.SetAllFalse();
    for (const auto rng : secondary_index_start_end_pairs_) {
        Bitmask part_result(segment_row_count);
        if (cardinality == SecondaryIndexCardinality::kHighCardinality) {
            part_result = ExecuteSingleRangeHighCardinalityT<ColumnValueT>(rng, &*index_meta, segment_row_count);
        } else {
            part_result = ExecuteSingleRangeLowCardinalityT<ColumnValueT>(rng, &*index_meta, segment_row_count);
        }
        result.MergeOr(part_result);
    }
    result.RunOptimize();
    return result;
}

} // namespace infinity
