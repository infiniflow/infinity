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

#include <cassert>

module infinity_core:physical_match.impl;

import :physical_match;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :query_context;
import :operator_state;
import :data_block;
import :column_vector;
import :expression_evaluator;
import :expression_state;
import :base_expression;
import :match_expression;
import :default_values;
import :infinity_exception;
import :value;
import :base_table_ref;
import :block_index;
import :load_meta;
import :status;
import :index_defines;
import :query_node;
import :query_builder;
import :doc_iterator;
import :logger;
import :analyzer;
import :term;
import :fulltext_score_result_heap;
import :physical_index_scan;
import :column_index_reader;
import :filter_value_type_classification;
import :common_analyzer;
import :analyzer_pool;
import :roaring_bitmap;
import :knn_filter;
import :highlighter;
import :parse_fulltext_options;
import :result_cache_manager;
import :cached_match;
import :filter_iterator;
import :score_threshold_iterator;
import :new_txn;

import std;
import third_party;

import logical_type;
import search_options;
import row_id;
import data_type;

namespace infinity {

struct QueryIterators {
    std::unique_ptr<DocIterator> query_iter{};
    // for comparison
    std::unique_ptr<DocIterator> bmw_iter{};
    std::unique_ptr<DocIterator> batch_iter{};
    std::unique_ptr<DocIterator> naive_iter{};
};

QueryIterators CreateQueryIterators(QueryBuilder &query_builder,
                                    FullTextQueryContext &context,
                                    const EarlyTermAlgo early_term_algo,
                                    const float begin_threshold,
                                    const float score_threshold) {
    auto get_iter = [&query_builder, &context, begin_threshold, score_threshold, function_name = __func__](const EarlyTermAlgo algo) {
        switch (algo) {
            case EarlyTermAlgo::kAuto:
            case EarlyTermAlgo::kNaive:
            case EarlyTermAlgo::kBatch:
            case EarlyTermAlgo::kBMW: {
                // ok
                break;
            }
            case EarlyTermAlgo::kCompare: {
                UnrecoverableError(std::format("{}: Wrong parameter!", function_name));
                break;
            }
        }
        context.early_term_algo_ = algo;
        auto iter = query_builder.CreateSearch(context);
        // iter is nullptr if fulltext index is present but there's no data
        if (iter) {
            iter->UpdateScoreThreshold(std::max(begin_threshold, score_threshold));
            if (score_threshold > 0.0f) {
                auto new_iter = std::make_unique<ScoreThresholdIterator>(std::move(iter), score_threshold);
                iter = std::move(new_iter);
            }
        } else {
            // LOG_WARN("physical_match: iter is nullptr");
        }
        return iter;
    };
    QueryIterators query_iterators;
    switch (early_term_algo) {
        case EarlyTermAlgo::kAuto:
        case EarlyTermAlgo::kNaive:
        case EarlyTermAlgo::kBatch:
        case EarlyTermAlgo::kBMW: {
            query_iterators.query_iter = get_iter(early_term_algo);
            break;
        }
        case EarlyTermAlgo::kCompare: {
            query_iterators.bmw_iter = get_iter(EarlyTermAlgo::kBMW);
            query_iterators.batch_iter = get_iter(EarlyTermAlgo::kBatch);
            query_iterators.naive_iter = get_iter(EarlyTermAlgo::kNaive);
            break;
        }
    }
    return query_iterators;
}

void ASSERT_FLOAT_EQ(const float bar, const u32 i, const float a, const float b) {
    const float diff_percent = std::abs(a - b) / std::max(std::abs(a), std::abs(b));
    if (diff_percent > bar) {
        std::ostringstream oss;
        oss << "Debug Info: result mismatch at " << i << " : a: " << a << ", b: " << b << ", diff_percent: " << diff_percent << std::endl;
        LOG_ERROR(std::move(oss).str());
    }
}

u32 ExecuteFTSearch(DocIterator *iter, FullTextScoreResultHeap &result_heap) {
    u32 loop_cnt = 0;
    // iter is nullptr if fulltext index is present but there's no data
    if (!iter) {
        LOG_DEBUG("iter is nullptr");
        return loop_cnt;
    }
    while (true) {
        ++loop_cnt;
        if (!(iter->Next())) [[unlikely]] {
            break;
        }
        if (result_heap.AddResult(iter->Score(), iter->DocID())) {
            // update threshold
            iter->UpdateScoreThreshold(result_heap.GetScoreThreshold());
        }
    }
    return loop_cnt;
}

auto ExecuteFTSearch(const QueryIterators &query_iterators, const u32 topn) {
    struct FTSearchResultType {
        u32 result_count{};
        std::unique_ptr<float[]> score_result{};
        std::unique_ptr<RowID[]> row_id_result{};
    };
    auto GetFTSearchResult = [topn](const std::unique_ptr<DocIterator> &iter) {
        FTSearchResultType result;
        result.score_result = std::make_unique_for_overwrite<float[]>(topn);
        result.row_id_result = std::make_unique_for_overwrite<RowID[]>(topn);
        FullTextScoreResultHeap result_heap(topn, result.score_result.get(), result.row_id_result.get());
        [[maybe_unused]] const auto loop_cnt = ExecuteFTSearch(iter.get(), result_heap);
        result_heap.Sort();
        result.result_count = result_heap.GetResultSize();
        return result;
    };
    if (query_iterators.query_iter) [[likely]] {
        return GetFTSearchResult(query_iterators.query_iter);
    }
    // compare
    auto bmw_result = GetFTSearchResult(query_iterators.bmw_iter);
    FTSearchResultType naive_result;
    {
        naive_result.score_result = std::make_unique_for_overwrite<float[]>(topn);
        naive_result.row_id_result = std::make_unique_for_overwrite<RowID[]>(topn);
        FullTextScoreResultHeap result_heap(topn, naive_result.score_result.get(), naive_result.row_id_result.get());
        if (query_iterators.batch_iter || query_iterators.naive_iter) {
            if (!query_iterators.batch_iter || !query_iterators.naive_iter) {
                UnrecoverableError("batch_iter and naive_iter should be both nullptr or both not nullptr");
            }
            while (true) {
                const auto batch_next = query_iterators.batch_iter->Next();
                const auto naive_next = query_iterators.naive_iter->Next();
                if (!batch_next && !naive_next) {
                    break;
                }
                if (!batch_next || !naive_next) {
                    LOG_ERROR("Conflict: batch_next and naive_next should be both true or both false");
                    break;
                }
                const auto batch_score = query_iterators.batch_iter->Score();
                const auto naive_score = query_iterators.naive_iter->Score();
                const auto batch_doc_id = query_iterators.batch_iter->DocID();
                const auto naive_doc_id = query_iterators.naive_iter->DocID();
                const auto batch_match_count = query_iterators.batch_iter->MatchCount();
                const auto naive_match_count = query_iterators.naive_iter->MatchCount();
                if (batch_doc_id != naive_doc_id) {
                    LOG_ERROR(fmt::format("doc_id mismatch between batch and naive: {} vs {}", batch_doc_id.ToString(), naive_doc_id.ToString()));
                }
                if (batch_match_count != naive_match_count) {
                    LOG_ERROR(fmt::format("match count mismatch between batch and naive: {} vs {}", batch_match_count, naive_match_count));
                }
                if (std::abs(batch_score - naive_score) / std::max(std::abs(batch_score), std::abs(naive_score)) > 1e-4) {
                    LOG_ERROR(fmt::format("score mismatch between batch and naive: {} vs {}", batch_score, naive_score));
                }
                result_heap.AddResult(naive_score, naive_doc_id);
            }
        }
        result_heap.Sort();
        naive_result.result_count = result_heap.GetResultSize();
    }
    if (bmw_result.result_count != naive_result.result_count) {
        LOG_ERROR(fmt::format("result count mismatch between bmw and naive: {} vs {}", bmw_result.result_count, naive_result.result_count));
    }
    for (u32 i = 0; i < std::min(bmw_result.result_count, naive_result.result_count); ++i) {
        ASSERT_FLOAT_EQ(1e-4, i, bmw_result.score_result[i], naive_result.score_result[i]);
    }
    return bmw_result;
}

bool PhysicalMatch::ExecuteInner(QueryContext *query_context, OperatorState *operator_state) {
    if (!common_query_filter_) {
        UnrecoverableError(fmt::format("{}: common_query_filter_ is nullptr", __func__));
    }
    using TimeDurationType = std::chrono::duration<float, std::milli>;
    const auto execute_start_time = std::chrono::high_resolution_clock::now();
    // 1. build QueryNode tree
    QueryBuilder query_builder(base_table_ref_->table_info_);
    query_builder.Init(index_reader_);
    const auto finish_init_query_builder_time = std::chrono::high_resolution_clock::now();
    LOG_DEBUG(fmt::format("PhysicalMatch 1: Init QueryBuilder time: {} ms",
                          static_cast<TimeDurationType>(finish_init_query_builder_time - execute_start_time).count()));

    // 2 build query iterator
    FullTextQueryContext full_text_query_context(ft_similarity_,
                                                 bm25_params_,
                                                 minimum_should_match_option_,
                                                 rank_features_option_,
                                                 top_n_,
                                                 match_expr_->index_names_);
    full_text_query_context.query_tree_ = std::make_unique<FilterQueryNode>(common_query_filter_.get(), std::move(query_tree_));
    const auto query_iterators = CreateQueryIterators(query_builder, full_text_query_context, early_term_algo_, begin_threshold_, score_threshold_);
    const auto finish_query_builder_time = std::chrono::high_resolution_clock::now();
    LOG_DEBUG(fmt::format("PhysicalMatch Part 2: Build Query iterator time: {} ms",
                          static_cast<TimeDurationType>(finish_query_builder_time - finish_init_query_builder_time).count()));

    // 3 full text search
    const auto [result_count, score_result, row_id_result] = ExecuteFTSearch(query_iterators, top_n_);
    auto finish_query_time = std::chrono::high_resolution_clock::now();
    LOG_DEBUG(fmt::format("PhysicalMatch Part 3: Full text search time: {} ms",
                          static_cast<TimeDurationType>(finish_query_time - finish_query_builder_time).count()));
    LOG_DEBUG(fmt::format("Full text search result count: {}", result_count));

    // 4 populate result DataBlock
    // 4.1 prepare first output_data_block
    auto &output_data_blocks = operator_state->data_block_array_;
    auto OutputTypesPtr = GetOutputTypes();
    std::vector<std::shared_ptr<DataType>> &OutputTypes = *OutputTypesPtr;
    auto append_data_block = [&]() {
        auto data_block = DataBlock::MakeUniquePtr();
        data_block->Init(OutputTypes);
        output_data_blocks.emplace_back(std::move(data_block));
    };
    append_data_block();
    // 4.2 output
    {
        OutputToDataBlockHelper output_to_data_block_helper;
        u32 output_block_idx = output_data_blocks.size() - 1;
        std::vector<size_t> &column_ids = base_table_ref_->column_ids_;
        size_t column_n = column_ids.size();
        u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
        u32 output_block_row_id = 0;
        DataBlock *output_block_ptr = output_data_blocks.back().get();

        for (u32 output_id = 0; output_id < result_count; ++output_id) {
            if (output_block_row_id == block_capacity) {
                output_block_ptr->Finalize();
                append_data_block();
                output_block_ptr = output_data_blocks.back().get();
                ++output_block_idx;
                output_block_row_id = 0;
            }
            const RowID &row_id = row_id_result[output_id];
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            size_t column_id = 0;
            for (; column_id < column_n; ++column_id) {
                output_to_data_block_helper
                    .AddOutputJobInfo(segment_id, block_id, column_ids[column_id], block_offset, output_block_idx, column_id, output_block_row_id);
                output_block_ptr->column_vectors[column_id]->Finalize(output_block_ptr->column_vectors[column_id]->Size() + 1);
            }
            Value v = Value::MakeFloat(score_result[output_id]);
            output_block_ptr->column_vectors[column_id++]->AppendValue(v);
            output_block_ptr->column_vectors[column_id]->AppendWith(row_id, 1);
            ++output_block_row_id;
        }
        output_block_ptr->Finalize();
        output_to_data_block_helper.OutputToDataBlock(query_context->storage()->buffer_manager(),
                                                      base_table_ref_->block_index_.get(),
                                                      output_data_blocks);
    }
    operator_state->SetComplete();
    ResultCacheManager *cache_mgr = query_context->storage()->result_cache_manager();
    if (cache_result_ && cache_mgr != nullptr) {
        AddCache(query_context, cache_mgr, output_data_blocks);
    }
    LOG_DEBUG(fmt::format("PhysicalMatch Part 4: Output data time: {} ms",
                          static_cast<TimeDurationType>(std::chrono::high_resolution_clock::now() - finish_query_time).count()));
    return true;
}

PhysicalMatch::PhysicalMatch(const u64 id,
                             std::shared_ptr<BaseTableRef> base_table_ref,
                             std::shared_ptr<MatchExpression> match_expr,
                             std::shared_ptr<IndexReader> index_reader,
                             std::unique_ptr<QueryNode> &&query_tree,
                             const float begin_threshold,
                             const EarlyTermAlgo early_term_algo,
                             const u32 top_n,
                             const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                             MinimumShouldMatchOption &&minimum_should_match_option,
                             RankFeaturesOption &&rank_features_option,
                             const f32 score_threshold,
                             const FulltextSimilarity ft_similarity,
                             const BM25Params &bm25_params,
                             const u64 match_table_index,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas,
                             const bool cache_result)
    : PhysicalOperator(PhysicalOperatorType::kMatch, nullptr, nullptr, id, std::move(load_metas), cache_result), table_index_(match_table_index),
      base_table_ref_(std::move(base_table_ref)), match_expr_(std::move(match_expr)), index_reader_(std::move(index_reader)),
      query_tree_(std::move(query_tree)), begin_threshold_(begin_threshold), early_term_algo_(early_term_algo), top_n_(top_n),
      common_query_filter_(common_query_filter), minimum_should_match_option_(std::move(minimum_should_match_option)),
      rank_features_option_(std::move(rank_features_option)), score_threshold_(score_threshold), ft_similarity_(ft_similarity),
      bm25_params_(bm25_params) {}

PhysicalMatch::~PhysicalMatch() = default;

void PhysicalMatch::Init(QueryContext *query_context) {}

bool PhysicalMatch::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto start_time = std::chrono::high_resolution_clock::now();
    assert(common_query_filter_);
    {
        bool try_result = common_query_filter_->TryFinishBuild();
        auto finish_filter_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> filter_duration = finish_filter_time - start_time;
        LOG_DEBUG(fmt::format("PhysicalMatch Prepare: Filter time: {} ms", filter_duration.count()));
        if (!try_result) {
            // not ready, abort and wait for next time
            return true;
        }
    }
    bool return_value = ExecuteInner(query_context, operator_state);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end_time - start_time;
    LOG_DEBUG(fmt::format("PhysicalMatch Execute time: {} ms", duration.count()));
    return return_value;
}

std::shared_ptr<std::vector<std::string>> PhysicalMatch::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result_names = std::make_shared<std::vector<std::string>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalMatch::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(std::make_shared<DataType>(LogicalType::kFloat));
    result_types->emplace_back(std::make_shared<DataType>(LogicalType::kRowID));
    return result_types;
}

std::string PhysicalMatch::ToString(i64 &space) const {
    std::string arrow_str;
    if (space != 0) {
        arrow_str = std::string(space - 2, ' ');
        arrow_str += "-> PhysicalMatch ";
    } else {
        arrow_str = "PhysicalMatch ";
    }
    std::string res = fmt::format("{} Table: {}, {}", arrow_str, *(base_table_ref_->table_info_->table_name_), match_expr_->ToString());
    return res;
}

void PhysicalMatch::AddCache(QueryContext *query_context,
                             ResultCacheManager *cache_mgr,
                             const std::vector<std::unique_ptr<DataBlock>> &output_data_blocks) {
    NewTxn *new_txn = query_context->GetNewTxn();
    TxnTimeStamp begin_ts = new_txn->BeginTS();

    TableInfo *table_info = base_table_ref_->table_info_.get();
    TxnTimeStamp query_ts = std::min(begin_ts, table_info->max_commit_ts_);
    std::vector<std::unique_ptr<DataBlock>> data_blocks(output_data_blocks.size());
    for (size_t i = 0; i < output_data_blocks.size(); ++i) {
        data_blocks[i] = output_data_blocks[i]->Clone();
    }
    auto cached_node = std::make_unique<CachedMatch>(query_ts, this);
    bool success = cache_mgr->AddCache(std::move(cached_node), std::move(data_blocks));
    if (!success) {
        LOG_WARN(fmt::format("Add cache failed for query: {}", begin_ts));
    } else {
        LOG_INFO(fmt::format("Add cache success for query: {}", begin_ts));
    }
}

} // namespace infinity
