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
import txn;
import buffer_manager;
import table_entry;
import column_def;
import embedding_info;
import block_index;
import block_column_entry;
import mlas_matrix_multiply;

namespace infinity {

struct RRFRankDoc {
    RowID row_id;
    float score;
    Vector<SizeT> ranks;
};

PhysicalFusion::PhysicalFusion(const u64 id,
                               SharedPtr<BaseTableRef> base_table_ref,
                               UniquePtr<PhysicalOperator> left,
                               UniquePtr<PhysicalOperator> right,
                               SharedPtr<FusionExpression> fusion_expr,
                               SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kFusion, std::move(left), std::move(right), id, load_metas), base_table_ref_(std::move(base_table_ref)),
      fusion_expr_(std::move(fusion_expr)) {}

PhysicalFusion::~PhysicalFusion() {}

void PhysicalFusion::Init() {
    {
        String &method = fusion_expr_->method_;
        std::transform(method.begin(), method.end(), std::back_inserter(to_lower_method_), [](unsigned char c) { return std::tolower(c); });
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
        UnrecoverableError(fmt::format("output_names_ size {} is not equal to output_types_ size {}.", output_names_->size(), output_types_->size()));
    }
}

void PhysicalFusion::ExecuteRRF(const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                                Vector<UniquePtr<DataBlock>> &output_data_block_array) const {
    SizeT rank_constant = 60;
    if (fusion_expr_->options_.get() != nullptr) {
        if (auto it = fusion_expr_->options_->options_.find("rank_constant"); it != fusion_expr_->options_->options_.end()) {
            long l = std::strtol(it->second.c_str(), NULL, 10);
            if (l > 1) {
                rank_constant = (SizeT)l;
            }
        }
    }

    Vector<RRFRankDoc> rrf_vec;
    Map<RowID, SizeT> rrf_map; // row_id to index of rrf_vec_
    Vector<u64> fragment_ids;
    SizeT fragment_idx = 0;
    // 1 calculate every doc's ranks
    for (const auto &[fragment_id, input_blocks] : input_data_blocks) {
        fragment_ids.push_back(fragment_id);
        SizeT base_rank = 1;
        for (const UniquePtr<DataBlock> &input_data_block : input_blocks) {
            if (input_data_block->column_count() != GetOutputTypes()->size()) {
                UnrecoverableError(fmt::format("input_data_block column count {} is incorrect, expect {}.",
                                               input_data_block->column_count(),
                                               GetOutputTypes()->size()));
            }
            auto &row_id_column = *input_data_block->column_vectors[input_data_block->column_count() - 1];
            auto row_ids = reinterpret_cast<RowID *>(row_id_column.data());
            SizeT row_n = input_data_block->row_count();
            for (SizeT i = 0; i < row_n; i++) {
                RowID docId = row_ids[i];
                if (rrf_map.find(docId) == rrf_map.end()) {
                    RRFRankDoc doc;
                    doc.row_id = docId;
                    rrf_vec.push_back(doc);
                    rrf_map[docId] = rrf_vec.size() - 1;
                }
                RRFRankDoc &doc = rrf_vec[rrf_map[docId]];
                doc.ranks.resize(fragment_idx + 1, 0);
                doc.ranks[fragment_idx] = base_rank + i;
            }
            base_rank += row_n;
        }
        fragment_idx++;
    }

    // 2 calculate every doc's score
    for (auto &doc : rrf_vec) {
        doc.score = 0.0F;
        for (auto &rank : doc.ranks) {
            if (rank == 0)
                continue;
            doc.score += 1.0F / (rank_constant + rank);
        }
    }
    // 3 sort docs in reverse per their score
    std::sort(std::begin(rrf_vec), std::end(rrf_vec), [](const RRFRankDoc &lhs, const RRFRankDoc &rhs) noexcept { return lhs.score > rhs.score; });

    // 4 generate output data blocks
    UniquePtr<DataBlock> output_data_block = DataBlock::MakeUniquePtr();
    output_data_block->Init(*GetOutputTypes());
    SizeT row_count = 0;
    for (RRFRankDoc &doc : rrf_vec) {
        // 4.1 get every doc's columns from input data blocks
        if (row_count == output_data_block->capacity()) {
            output_data_block->Finalize();
            output_data_block_array.push_back(std::move(output_data_block));
            output_data_block = DataBlock::MakeUniquePtr();
            output_data_block->Init(*GetOutputTypes());
            row_count = 0;
        }
        SizeT fragment_idx = 0;
        while (fragment_idx < doc.ranks.size() && doc.ranks[fragment_idx] == 0)
            fragment_idx++;
        if (fragment_idx >= doc.ranks.size()) {
            UnrecoverableError(fmt::format("Cannot find fragment_idx"));
        }
        u64 fragment_id = fragment_ids[fragment_idx];
        const auto &input_blocks = input_data_blocks.at(fragment_id);
        if (input_blocks.size() == 0) {
            UnrecoverableError(fmt::format("input_data_blocks_[{}] is empty.", fragment_id));
        }
        SizeT block_idx = 0;
        SizeT row_idx = doc.ranks[fragment_idx] - 1;
        while (row_idx >= input_blocks[block_idx]->row_count()) {
            row_idx -= input_blocks[block_idx]->row_count();
            block_idx++;
        }
        if (block_idx >= input_blocks.size()) {
            UnrecoverableError(fmt::format("Cannot find block_idx"));
        }

        SizeT column_n = GetOutputTypes()->size() - 2;
        for (SizeT i = 0; i < column_n; ++i) {
            output_data_block->column_vectors[i]->AppendWith(*input_blocks[block_idx]->column_vectors[i], row_idx, 1);
        }
        // 4.2 add hidden columns: score, row_id
        Value v = Value::MakeFloat(doc.score);
        output_data_block->column_vectors[column_n]->AppendValue(v);
        output_data_block->column_vectors[column_n + 1]->AppendWith(doc.row_id, 1);
        row_count++;
    }
    output_data_block->Finalize();
    output_data_block_array.push_back(std::move(output_data_block));
}

struct MatchTensorRerankDoc {
    RowID row_id_;
    u64 from_input_data_block_id_;
    u32 from_block_idx_;
    u32 from_row_idx_;
    float score_ = 0.0f;
    MatchTensorRerankDoc(const RowID row_id, const u64 from_input_data_block_id, const u32 from_block_idx, const u32 from_row_idx)
        : row_id_(row_id), from_input_data_block_id_(from_input_data_block_id), from_block_idx_(from_block_idx), from_row_idx_(from_row_idx) {}
};

void PhysicalFusion::ExecuteMatchTensor(QueryContext *query_context,
                                        const Map<u64, Vector<UniquePtr<DataBlock>>> &input_data_blocks,
                                        Vector<UniquePtr<DataBlock>> &output_data_block_array) const {
    const TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    const BlockIndex *block_index = base_table_ref_->block_index_.get();
    u32 topn = 10;
    ColumnID column_id = std::numeric_limits<ColumnID>::max();
    const DataType *column_data_type = nullptr;
    const EmbeddingInfo *column_embedding_info = nullptr;
    u32 column_unit_embedding_dimension = 0;
    u32 column_unit_embedding_bytes = 0;
    UniquePtr<char[]> query_tensor;
    u32 query_tensor_data_num = 0;
    u32 query_tensor_embedding_num = 0;
    EmbeddingDataType query_tensor_data_type = EmbeddingDataType::kElemInvalid;
    // prepare query info
    {
        String column_name;
        String search_tensor;
        String tensor_data_type;
        String match_method;
        // find parameters
        if (fusion_expr_->options_.get() != nullptr) {
            const auto &options = fusion_expr_->options_->options_;
            if (const auto it = options.find("column_name"); it != options.end()) {
                column_name = it->second;
            }
            if (const auto it = options.find("search_tensor"); it != options.end()) {
                search_tensor = it->second;
            }
            if (const auto it = options.find("tensor_data_type"); it != options.end()) {
                tensor_data_type = it->second;
                // to_lower
                std::transform(tensor_data_type.begin(), tensor_data_type.end(), tensor_data_type.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
            }
            if (const auto it = options.find("match_method"); it != options.end()) {
                match_method = it->second;
                // to_lower
                std::transform(match_method.begin(), match_method.end(), match_method.begin(), [](unsigned char c) { return std::tolower(c); });
            }
            if (const auto it = options.find("topn"); it != options.end()) {
                if (const int topn_int = std::stoi(it->second); topn_int > 0) {
                    topn = topn_int;
                }
            }
        }
        // validate column_name
        {
            column_id = table_entry->GetColumnIdByName(column_name);
            const ColumnDef *column_def = table_entry->GetColumnDefByID(column_id);
            column_data_type = column_def->type().get();
            switch (column_data_type->type()) {
                case LogicalType::kTensor:
                case LogicalType::kTensorArray: {
                    column_embedding_info = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get());
                    break;
                }
                default: {
                    const auto error_info =
                        fmt::format("Fusion MatchTensor column_name {} is not a Tensor or TensorArray column. column type is : {}.",
                                    column_name,
                                    column_data_type->ToString());
                    LOG_ERROR(error_info);
                    RecoverableError(Status::NotSupport(error_info));
                    break;
                }
            }
            column_unit_embedding_dimension = column_embedding_info->Dimension();
            column_unit_embedding_bytes = column_embedding_info->Size();
            switch (column_embedding_info->Type()) {
                case EmbeddingDataType::kElemFloat: {
                    break;
                }
                // TODO: support bit type
                default: {
                    const auto error_info = fmt::format("Fusion MatchTensor target column {} basic element type is unsupported: {}.",
                                                        column_name,
                                                        EmbeddingT::EmbeddingDataType2String(column_embedding_info->Type()));
                    LOG_ERROR(error_info);
                    RecoverableError(Status::NotSupport(error_info));
                    break;
                }
            }
        }
        // validate tensor_data_type
        // TODO: now only support float32 query tensor
        if (tensor_data_type == "float32" or tensor_data_type == "float" or tensor_data_type == "f32") {
            query_tensor_data_type = EmbeddingDataType::kElemFloat;
        } else {
            const auto error_info = "Fusion MatchTensor tensor_data_type option is invalid. Now only support float32.";
            LOG_ERROR(error_info);
            RecoverableError(Status::NotSupport(error_info));
        }
        // validate search_tensor
        {
            const auto split_view = SplitTensorElement(search_tensor, ',', column_unit_embedding_dimension);
            query_tensor_data_num = split_view.size();
            query_tensor_embedding_num = query_tensor_data_num / column_unit_embedding_dimension;
            query_tensor = MakeUnique<char[]>(query_tensor_embedding_num * column_unit_embedding_bytes);
            switch (query_tensor_data_type) {
                case EmbeddingDataType::kElemFloat: {
                    auto *query_tensor_ptr = reinterpret_cast<float *>(query_tensor.get());
                    for (u32 i = 0; i < query_tensor_data_num; i++) {
                        query_tensor_ptr[i] = DataType::StringToValue<float>(split_view[i]);
                    }
                    break;
                }
                default: {
                    const auto error_info = "Fusion MatchTensor query tensor data type is invalid. Now only support float32.";
                    LOG_ERROR(error_info);
                    RecoverableError(Status::NotSupport(error_info));
                    break;
                }
            }
        }
        // validate match_method
        if (match_method != "maxsim") {
            const auto error_info = "Fusion MatchTensor match_method option is invalid. Now only support MaxSim.";
            LOG_ERROR(error_info);
            RecoverableError(Status::NotSupport(error_info));
        }
    }
    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    Vector<MatchTensorRerankDoc> rerank_docs;
    // 1. prepare query target rows
    for (std::unordered_set<u64> row_id_set; const auto &[input_data_block_id, input_blocks] : input_data_blocks) {
        for (u32 block_id = 0; block_id < input_blocks.size(); ++block_id) {
            const UniquePtr<DataBlock> &input_data_block = input_blocks[block_id];
            if (input_data_block->column_count() != GetOutputTypes()->size()) {
                UnrecoverableError(fmt::format("input_data_block column count {} is incorrect, expect {}.",
                                               input_data_block->column_count(),
                                               GetOutputTypes()->size()));
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
    // TODO: now only consider MaxSim
    for (auto &doc : rerank_docs) {
        const RowID row_id = doc.row_id_;
        const SegmentID segment_id = row_id.segment_id_;
        const SegmentOffset segment_offset = row_id.segment_offset_;
        const BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
        const BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
        BlockColumnEntry *block_column_entry =
            block_index->segment_block_index_.at(segment_id).block_map_.at(block_id)->GetColumnBlockEntry(column_id);
        auto column_vec = block_column_entry->GetColumnVector(buffer_mgr);
        auto tensor_ptr = reinterpret_cast<const TensorT *>(column_vec.data());
        const auto [embedding_num, chunk_id, chunk_offset] = tensor_ptr[block_offset];
        const char *tensor_data_ptr = column_vec.buffer_->fix_heap_mgr_->GetRawPtrFromChunk(chunk_id, chunk_offset);
        // use mlas
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_tensor_embedding_num * embedding_num);
        matrixA_multiply_transpose_matrixB_output_to_C(reinterpret_cast<const float *>(query_tensor.get()),
                                                       reinterpret_cast<const float *>(tensor_data_ptr),
                                                       query_tensor_embedding_num,
                                                       embedding_num,
                                                       column_unit_embedding_dimension,
                                                       output_ptr.get());
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_tensor_embedding_num; ++query_i) {
            const float *query_ip_ptr = output_ptr.get() + query_i * embedding_num;
            float max_score = std::numeric_limits<float>::lowest();
            for (u32 k = 0; k < embedding_num; ++k) {
                max_score = std::max(max_score, query_ip_ptr[k]);
            }
            maxsim_score += max_score;
        }
        doc.score_ = maxsim_score;
    }
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
    if (to_lower_method_.compare("rrf") == 0) {
        ExecuteRRF(fusion_operator_state->input_data_blocks_, fusion_operator_state->data_block_array_);
        fusion_operator_state->input_data_blocks_.clear();
        fusion_operator_state->SetComplete();
        return true;
    }
    if (to_lower_method_.compare("match_tensor") == 0) {
        ExecuteMatchTensor(query_context, fusion_operator_state->input_data_blocks_, fusion_operator_state->data_block_array_);
        fusion_operator_state->input_data_blocks_.clear();
        fusion_operator_state->SetComplete();
        return true;
    }
    Status status = Status::NotSupport(fmt::format("Fusion method {} is not implemented.", fusion_expr_->method_));
    LOG_ERROR(status.message());
    RecoverableError(std::move(status));
    return false;
}

bool PhysicalFusion::ExecuteNotFirstOp(QueryContext *query_context, OperatorState *operator_state) const {
    // this op has prev fusion op
    if (!operator_state->prev_op_state_->Complete()) {
        UnrecoverableError("Fusion with previous fusion op, but prev_op_state_ is not complete.");
        return false;
    }
    if (to_lower_method_.compare("match_tensor") == 0) {
        Map<u64, Vector<UniquePtr<DataBlock>>> input_data_blocks;
        input_data_blocks.emplace(0, std::move(operator_state->prev_op_state_->data_block_array_));
        operator_state->prev_op_state_->data_block_array_.clear();
        ExecuteMatchTensor(query_context, input_data_blocks, operator_state->data_block_array_);
        operator_state->SetComplete();
        return true;
    }
    Status status = Status::NotSupport(fmt::format("Fusion method {} is not implemented.", fusion_expr_->method_));
    LOG_ERROR(status.message());
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
