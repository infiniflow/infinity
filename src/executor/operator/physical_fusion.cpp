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
#include <cmath>
#include <cstdlib>
#include <string>

module physical_fusion;

import stl;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
import operator_state;
import status;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import base_expression;
import fusion_expression;
import load_meta;
import default_values;
import third_party;
import infinity_exception;
import value;
import internal_types;
import logger;
import logical_type;
import knn_expr;
import match_tensor_expr;
import txn;
import buffer_manager;
import table_entry;
import column_def;
import embedding_info;
import block_index;
import block_column_entry;
import mlas_matrix_multiply;
import physical_match_tensor_scan;
import physical_knn_scan;
import physical_merge_knn;

namespace infinity {

struct DocScore {
    RowID row_id_;
    u64 from_input_data_block_id_;
    u32 from_block_idx_;
    u32 from_row_idx_;
    float fusion_score_;
    Vector<float> child_scores_;
    Vector<bool> mask_;
};

PhysicalFusion::PhysicalFusion(const u64 id,
                               SharedPtr<BaseTableRef> base_table_ref,
                               UniquePtr<PhysicalOperator> left,
                               UniquePtr<PhysicalOperator> right,
                               Vector<UniquePtr<PhysicalOperator>> other_children,
                               SharedPtr<FusionExpression> fusion_expr,
                               SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kFusion, std::move(left), std::move(right), id, load_metas), other_children_(std::move(other_children)),
      base_table_ref_(std::move(base_table_ref)), fusion_expr_(std::move(fusion_expr)) {}

PhysicalFusion::~PhysicalFusion() {}

void PhysicalFusion::Init() {
    {
        String &method = fusion_expr_->method_;
        String to_lower_method;
        std::transform(method.begin(), method.end(), std::back_inserter(to_lower_method), [](unsigned char c) { return std::tolower(c); });
        if (to_lower_method == "weighted_sum") {
            fusion_method_ = FusionMethod::kWeightedSum;
        } else if (to_lower_method == "match_tensor") {
            fusion_method_ = FusionMethod::kMatchTensor;
        } else {
            fusion_method_ = FusionMethod::kRRF;
        }
    }
    {
        const auto prev_output_names_ptr = left_->GetOutputNames();
        const Vector<String> &prev_output_names = *prev_output_names_ptr;
        output_names_ = MakeShared<Vector<String>>(prev_output_names);
        (*output_names_)[output_names_->size() - 2] = COLUMN_NAME_SCORE;
    }
    {
        const auto prev_output_types_ptr = left_->GetOutputTypes();
        const Vector<SharedPtr<DataType>> &prev_output_types = *prev_output_types_ptr;
        output_types_ = MakeShared<Vector<SharedPtr<DataType>>>(prev_output_types);
        (*output_types_)[output_types_->size() - 2] = MakeShared<DataType>(LogicalType::kFloat);
    }
    if (output_names_->size() != output_types_->size()) {
        String error_message =
            fmt::format("output_names_ size {} is not equal to output_types_ size {}.", output_names_->size(), output_types_->size());
        UnrecoverableError(error_message);
    }
}

// Refers to https://www.elastic.co/guide/en/elasticsearch/reference/current/rrf.html
void PhysicalFusion::ExecuteRRFWeighted(const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                                        Vector<UniquePtr<DataBlock>> &output_data_block_array) const {
    SizeT num_children = 2 + other_children_.size();
    SizeT rank_constant = 60;
    SizeT topn = DEFAULT_FUSION_OPTION_TOP_N;
    Vector<float> weights;
    if (fusion_expr_->options_.get() != nullptr) {
        if (auto it = fusion_expr_->options_->options_.find("window_size"); it != fusion_expr_->options_->options_.end()) {
            long l = std::strtol(it->second.c_str(), NULL, 10);
            if (l >= 1) {
                topn = (SizeT)l;
            }
        }
        if (auto it = fusion_expr_->options_->options_.find("topn"); it != fusion_expr_->options_->options_.end()) {
            long l = std::strtol(it->second.c_str(), NULL, 10);
            if (l >= 1) {
                topn = (SizeT)l;
            }
        }
        if (fusion_method_ == FusionMethod::kRRF) {
            if (auto it = fusion_expr_->options_->options_.find("rank_constant"); it != fusion_expr_->options_->options_.end()) {
                long l = std::strtol(it->second.c_str(), NULL, 10);
                if (l >= 1) {
                    rank_constant = (SizeT)l;
                }
            }
        } else {
            weights.reserve(num_children);
            if (auto it = fusion_expr_->options_->options_.find("weights"); it != fusion_expr_->options_->options_.end()) {
                const String &weight_str = it->second;
                std::stringstream ss(weight_str);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    double value = std::stod(item);
                    weights.push_back(value);
                }
            }
        }
    }
    if (fusion_method_ == FusionMethod::kWeightedSum) {
        SizeT num_weights = weights.size();
        if (num_weights < num_children) {
            for (SizeT i = num_weights; i < num_children; ++i) {
                weights.push_back(1.0F);
            }
        }
    }

    Vector<DocScore> rescore_vec;
    Map<RowID, SizeT> rescore_map; // row_id to index of rescore_vec_
    // 1 Prepare rescore_vec
    SizeT fragment_idx = 0;
    for (const auto &[fragment_id, input_blocks] : input_data_blocks) {
        SizeT base_rank = 1;
        u32 from_block_idx = 0;
        for (const UniquePtr<DataBlock> &input_data_block : input_blocks) {
            if (input_data_block->column_count() != GetOutputTypes()->size()) {
                String error_message = fmt::format("input_data_block column count {} is incorrect, expect {}.",
                                                   input_data_block->column_count(),
                                                   GetOutputTypes()->size());
                UnrecoverableError(error_message);
            }
            auto &row_id_column = *input_data_block->column_vectors[input_data_block->column_count() - 1];
            auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
            SizeT row_n = input_data_block->row_count();
            auto &row_score_column = *input_data_block->column_vectors[input_data_block->column_count() - 2];
            auto row_scores = reinterpret_cast<float *>(row_score_column.data());
            for (SizeT i = 0; i < row_n; i++) {
                RowID docId = row_ids[i];
                if (rescore_map.find(docId) == rescore_map.end()) {
                    DocScore doc;
                    doc.row_id_ = docId;
                    doc.from_input_data_block_id_ = fragment_id;
                    doc.from_block_idx_ = from_block_idx;
                    doc.from_row_idx_ = i;
                    doc.child_scores_.resize(num_children, 0.0f);
                    doc.mask_.resize(num_children, false);
                    rescore_vec.push_back(doc);
                    rescore_map[docId] = rescore_vec.size() - 1;
                }
                SizeT doc_idx = rescore_map[docId];
                DocScore &doc = rescore_vec[doc_idx];
                assert(fragment_idx < num_children);
                doc.mask_[fragment_idx] = true;
                if (fusion_method_ == FusionMethod::kRRF) {
                    doc.child_scores_[fragment_idx] = base_rank + i;
                } else {
                    assert(fusion_method_ == FusionMethod::kWeightedSum);
                    doc.child_scores_[fragment_idx] = row_scores[i];
                }
            }
            base_rank += row_n;
            from_block_idx++;
        }
        fragment_idx++;
    }

    // 2 calculate every doc's fusion_score
    if (fusion_method_ == FusionMethod::kRRF) {
        for (auto &doc : rescore_vec) {
            doc.fusion_score_ = 0.0f;
            for (auto &rank : doc.child_scores_) {
                if (rank < 1.0F)
                    continue;
                doc.fusion_score_ += 1.0F / (rank_constant + rank);
            }
        }
    } else {
        Vector<bool> min_heaps(num_children, false);
        for (SizeT i = 0; i < num_children; i++) {
            PhysicalOperator *child_op = nullptr;
            if (i == 0)
                child_op = left();
            else if (i == 1)
                child_op = right();
            else
                child_op = other_children_[i - 2].get();
            switch (child_op->operator_type()) {
                case PhysicalOperatorType::kKnnScan: {
                    PhysicalKnnScan *phy_knn_scan = static_cast<PhysicalKnnScan *>(child_op);
                    min_heaps[i] = phy_knn_scan->IsKnnMinHeap();
                    break;
                }
                case PhysicalOperatorType::kMergeKnn: {
                    PhysicalMergeKnn *phy_merge_knn = static_cast<PhysicalMergeKnn *>(child_op);
                    min_heaps[i] = phy_merge_knn->IsKnnMinHeap();
                    break;
                }
                case PhysicalOperatorType::kMatchTensorScan:
                case PhysicalOperatorType::kMergeMatchTensor:
                case PhysicalOperatorType::kMatchSparseScan:
                case PhysicalOperatorType::kMergeMatchSparse:
                case PhysicalOperatorType::kMatch: {
                    min_heaps[i] = true;
                    break;
                }
                default: {
                    String error_message = fmt::format("Cannot determine heap type of operator {}", int(child_op->operator_type()));
                    UnrecoverableError(error_message);
                }
            }
        }
        for (auto &doc : rescore_vec) {
            doc.fusion_score_ = 0.0f;
            for (SizeT i = 0; i < num_children; ++i) {
                if (!doc.mask_[i])
                    continue;
                // Normalize the child score in R to [0, 1]
                double normalized_score = std::atan(doc.child_scores_[i]) / M_PI + 0.5;
                if (!min_heaps[i])
                    normalized_score = 1.0 - normalized_score;
                doc.fusion_score_ += weights[i] * normalized_score;
            }
        }
    }

    // 3 sort docs in reverse per their fusion_score
    std::stable_sort(std::begin(rescore_vec), std::end(rescore_vec), [](const DocScore &lhs, const DocScore &rhs) noexcept {
        return lhs.fusion_score_ > rhs.fusion_score_;
    });
    if (rescore_vec.size() > topn) {
        rescore_vec.resize(topn);
    }

    // 4 generate output data blocks
    UniquePtr<DataBlock> output_data_block = DataBlock::MakeUniquePtr();
    output_data_block->Init(*GetOutputTypes());
    SizeT row_count = 0;
    for (DocScore &doc : rescore_vec) {
        // 4.1 get every doc's columns from input data blocks
        if (row_count == output_data_block->capacity()) {
            output_data_block->Finalize();
            output_data_block_array.push_back(std::move(output_data_block));
            output_data_block = DataBlock::MakeUniquePtr();
            output_data_block->Init(*GetOutputTypes());
            row_count = 0;
        }
        const auto &input_blocks = input_data_blocks.at(doc.from_input_data_block_id_);
        SizeT column_n = GetOutputTypes()->size() - 2;
        for (SizeT i = 0; i < column_n; ++i) {
            output_data_block->column_vectors[i]->AppendWith(*input_blocks[doc.from_block_idx_]->column_vectors[i], doc.from_row_idx_, 1);
        }
        // 4.2 add hidden columns: score, row_id
        Value v = Value::MakeFloat(doc.fusion_score_);
        output_data_block->column_vectors[column_n]->AppendValue(v);
        output_data_block->column_vectors[column_n + 1]->AppendWith(doc.row_id_, 1);
        row_count++;
    }
    output_data_block->Finalize();
    output_data_block_array.push_back(std::move(output_data_block));
}

void PhysicalFusion::ExecuteMatchTensor(QueryContext *query_context,
                                        const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                                        Vector<UniquePtr<DataBlock>> &output_data_block_array) const {
    const BlockIndex *block_index = base_table_ref_->block_index_.get();
    const TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    const ColumnID column_id = fusion_expr_->match_tensor_expr_->column_expr_->binding().column_idx;
    const ColumnDef *column_def = table_entry->GetColumnDefByID(column_id);
    const DataType *column_data_type = column_def->type().get();
    switch (column_data_type->type()) {
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            break;
        }
        default: {
            const auto error_info = fmt::format("Fusion MatchTensor column_name {} is not a Tensor or TensorArray column. column type is : {}.",
                                                fusion_expr_->match_tensor_expr_->column_expr_->column_name(),
                                                column_data_type->ToString());
            RecoverableError(Status::NotSupport(error_info));
            break;
        }
    }
    const EmbeddingInfo *column_embedding_info = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get());
    if (fusion_expr_->match_tensor_expr_->tensor_basic_embedding_dimension_ != column_embedding_info->Dimension()) {
        UnrecoverableError("Dimension of column and query tensor mismatch!");
    }
    // validate match_method
    if (fusion_expr_->match_tensor_expr_->search_method_ == MatchTensorSearchMethod::kInvalid) {
        const auto error_info = "Fusion MatchTensor match_method option is invalid.";
        RecoverableError(Status::NotSupport(error_info));
    }
    // prepare topn
    u32 topn = DEFAULT_MATCH_TENSOR_OPTION_TOP_N;
    {
        // find topn
        if (fusion_expr_->options_.get() != nullptr) {
            const auto &options = fusion_expr_->options_->options_;
            auto topn_it = options.end();
            if (topn_it = options.find("topn"); topn_it == options.end()) {
                if (topn_it = options.find("top_n"); topn_it == options.end()) {
                    if (topn_it = options.find("topk"); topn_it == options.end()) {
                        if (topn_it = options.find("top_k"); topn_it == options.end()) {
                            if (topn_it = options.find("topN"); topn_it == options.end()) {
                                topn_it = options.find("top_N");
                            }
                        }
                    }
                }
            }
            if (topn_it != options.end()) {
                if (const int topn_int = std::stoi(topn_it->second); topn_int > 0) {
                    topn = topn_int;
                }
            }
        }
    }
    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    Vector<MatchTensorRerankDoc> rerank_docs;
    // 1. prepare query target rows
    for (std::unordered_set<u64> row_id_set; const auto &[input_data_block_id, input_blocks] : input_data_blocks) {
        for (u32 block_id = 0; block_id < input_blocks.size(); ++block_id) {
            const UniquePtr<DataBlock> &input_data_block = input_blocks[block_id];
            if (input_data_block->column_count() != GetOutputTypes()->size()) {
                String error_message = fmt::format("input_data_block column count {} is incorrect, expect {}.",
                                                   input_data_block->column_count(),
                                                   GetOutputTypes()->size());
                UnrecoverableError(error_message);
            }
            auto &row_id_column = *input_data_block->column_vectors[input_data_block->column_count() - 1];
            auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
            u32 row_n = input_data_block->row_count();
            for (u32 i = 0; i < row_n; i++) {
                const RowID doc_id = row_ids[i];
                if (row_id_set.contains(doc_id.ToUint64())) {
                    // skip duplicate target
                    continue;
                }
                row_id_set.insert(doc_id.ToUint64());
                rerank_docs.emplace_back(doc_id, input_data_block_id, block_id, i);
            }
        }
    }
    // 2. sort by RowID, and access blocks in order
    std::sort(rerank_docs.begin(), rerank_docs.end(), [](const MatchTensorRerankDoc &lhs, const MatchTensorRerankDoc &rhs) noexcept {
        return lhs.row_id_ < rhs.row_id_;
    });
    // 3. calculate score
    CalculateFusionMatchTensorRerankerScores(rerank_docs, buffer_mgr, column_data_type, column_id, block_index, *fusion_expr_->match_tensor_expr_);
    // 4. sort by score
    std::sort(rerank_docs.begin(), rerank_docs.end(), [](const MatchTensorRerankDoc &lhs, const MatchTensorRerankDoc &rhs) noexcept {
        return lhs.score_ > rhs.score_;
    });
    if (rerank_docs.size() > topn) {
        rerank_docs.erase(rerank_docs.begin() + topn, rerank_docs.end());
    }
    // 5. generate output data blocks
    UniquePtr<DataBlock> output_data_block = DataBlock::MakeUniquePtr();
    output_data_block->Init(*GetOutputTypes());
    u32 row_count = 0;
    for (MatchTensorRerankDoc &doc : rerank_docs) {
        if (row_count == output_data_block->capacity()) {
            output_data_block->Finalize();
            output_data_block_array.push_back(std::move(output_data_block));
            output_data_block = DataBlock::MakeUniquePtr();
            output_data_block->Init(*GetOutputTypes());
            row_count = 0;
        }
        const auto &input_blocks = input_data_blocks.at(doc.from_input_data_block_id_);
        const u32 block_idx = doc.from_block_idx_;
        const u32 row_idx = doc.from_row_idx_;
        const ColumnID column_n = GetOutputTypes()->size() - 2;
        for (ColumnID i = 0; i < column_n; ++i) {
            output_data_block->column_vectors[i]->AppendWith(*input_blocks[block_idx]->column_vectors[i], row_idx, 1);
        }
        // 4.2 add hidden columns: score, row_id
        Value v = Value::MakeFloat(doc.score_);
        output_data_block->column_vectors[column_n]->AppendValue(v);
        output_data_block->column_vectors[column_n + 1]->AppendWith(doc.row_id_, 1);
        row_count++;
    }
    output_data_block->Finalize();
    output_data_block_array.push_back(std::move(output_data_block));
}

bool PhysicalFusion::Execute(QueryContext *query_context, OperatorState *operator_state) {
    if (operator_state->prev_op_state_ != nullptr) {
        // not the first op in fragment
        return ExecuteNotFirstOp(query_context, operator_state);
    }
    return ExecuteFirstOp(query_context, static_cast<FusionOperatorState *>(operator_state));
}

bool PhysicalFusion::ExecuteFirstOp(QueryContext *query_context, FusionOperatorState *fusion_operator_state) const {
    if (!fusion_operator_state->input_complete_) {
        return false;
    }
    if (fusion_method_ == FusionMethod::kRRF || fusion_method_ == FusionMethod::kWeightedSum) {
        ExecuteRRFWeighted(fusion_operator_state->input_data_blocks_, fusion_operator_state->data_block_array_);
        fusion_operator_state->input_data_blocks_.clear();
        fusion_operator_state->SetComplete();
        return true;
    }
    if (fusion_method_ == FusionMethod::kMatchTensor) {
        ExecuteMatchTensor(query_context, fusion_operator_state->input_data_blocks_, fusion_operator_state->data_block_array_);
        fusion_operator_state->input_data_blocks_.clear();
        fusion_operator_state->SetComplete();
        return true;
    }
    Status status = Status::NotSupport(fmt::format("Fusion method {} is not implemented.", fusion_expr_->method_));
    RecoverableError(std::move(status));
    return false;
}

bool PhysicalFusion::ExecuteNotFirstOp(QueryContext *query_context, OperatorState *operator_state) const {
    // this op has prev fusion op
    if (!operator_state->prev_op_state_->Complete()) {
        String error_message = "Fusion with previous fusion op, but prev_op_state_ is not complete.";
        UnrecoverableError(error_message);
        return false;
    }
    if (fusion_method_ == FusionMethod::kMatchTensor) {
        Map<u64, Vector<UniquePtr<DataBlock>>> input_data_blocks;
        input_data_blocks.emplace(0, std::move(operator_state->prev_op_state_->data_block_array_));
        operator_state->prev_op_state_->data_block_array_.clear();
        ExecuteMatchTensor(query_context, input_data_blocks, operator_state->data_block_array_);
        operator_state->SetComplete();
        return true;
    }
    Status status = Status::NotSupport(fmt::format("Fusion method {} is not implemented.", fusion_expr_->method_));
    RecoverableError(std::move(status));
    return false;
}

String PhysicalFusion::ToString(i64 &space) const {
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> PhysicalFusion ";
    } else {
        arrow_str = "PhysicalFusion ";
    }
    String res = fmt::format("{} {}", arrow_str, fusion_expr_->ToString());
    return res;
}

} // namespace infinity
