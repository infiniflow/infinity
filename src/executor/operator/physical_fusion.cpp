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

#include <algorithm>
#include <cstdlib>
#include <string>
import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_collection_entry;
import query_context;
// import data_table;
import operator_state;
import db_entry;
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

module physical_fusion;

namespace infinity {

struct RRFRankDoc {
    RowID row_id;
    float score;
    Vector<SizeT> ranks;
};

PhysicalFusion::PhysicalFusion(u64 id,
                               UniquePtr<PhysicalOperator> left,
                               UniquePtr<PhysicalOperator> right,
                               SharedPtr<FusionExpression> fusion_expr,
                               SharedPtr<Vector<String>> output_names,
                               SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                               SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kFusion, Move(left), Move(right), id, load_metas), fusion_expr_(fusion_expr),
      output_names_(Move(output_names)), output_types_(Move(output_types)) {}

PhysicalFusion::~PhysicalFusion() {}

void PhysicalFusion::Init() {}

bool PhysicalFusion::Execute(QueryContext *query_context, OperatorState *operator_state) {
    FusionOperatorState *fusion_operator_state = static_cast<FusionOperatorState *>(operator_state);
    if (!fusion_operator_state->input_complete_) {
        return false;
    }
    Assert<ExecutorException>(0 == fusion_expr_->method_.compare("rrf"), Format("Fusion method {} is not implemented.", fusion_expr_->method_));
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
    // 1 caclulate every doc's ranks
    for (auto &[fragment_id, input_blocks] : fusion_operator_state->input_data_blocks_) {
        fragment_ids.push_back(fragment_id);
        SizeT base_rank = 1;
        for (UniquePtr<DataBlock> &input_data_block : input_blocks) {
            Assert<ExecutorException>(
                input_data_block->column_count() == output_types_->size(),
                Format("input_data_block column count {} is incorrect, expect {}.", input_data_block->column_count(), output_types_->size()));
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

    // 2 caclulate every doc's score
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
    UniquePtr<DataBlock> output_data_block = Move(DataBlock::MakeUniquePtr());
    output_data_block->Init(*output_types_);
    SizeT row_count = 0;
    for (RRFRankDoc &doc : rrf_vec) {
        // 4.1 get every doc's columns from input data blocks
        if (row_count == output_data_block->capacity()) {
            output_data_block->Finalize();
            operator_state->data_block_array_.push_back(Move(output_data_block));
            output_data_block = Move(DataBlock::MakeUniquePtr());
            row_count = 0;
        }
        SizeT fragment_idx = 0;
        while (fragment_idx < doc.ranks.size() && doc.ranks[fragment_idx] == 0)
            fragment_idx++;
        if (fragment_idx >= doc.ranks.size()) {
            throw ExecutorException(Format("Cannot find fragment_idx"));
        }
        u64 fragment_id = fragment_ids[fragment_idx];
        auto &input_blocks = fusion_operator_state->input_data_blocks_[fragment_id];
        Assert<ExecutorException>(input_blocks.size() > 0, Format("input_data_blocks_[{}] is empty.", fragment_id));
        SizeT block_idx = 0;
        SizeT row_idx = doc.ranks[fragment_idx] - 1;
        while (row_idx >= input_blocks[block_idx]->row_count()) {
            row_idx -= input_blocks[block_idx]->row_count();
            block_idx++;
        }
        if (block_idx >= input_blocks.size()) {
            throw ExecutorException(Format("Cannot find block_idx"));
        }

        SizeT column_id = 0;
        for (; column_id < output_types_->size() - 2; ++column_id) {
            output_data_block->column_vectors[column_id]->AppendWith(*input_blocks[block_idx]->column_vectors[column_id], row_idx, 1);
        }
        // 4.2 add hidden columns: score, row_id
        Value v = Value::MakeFloat(doc.score);
        output_data_block->column_vectors[column_id++]->AppendValue(v);
        output_data_block->column_vectors[column_id]->AppendWith(doc.row_id, 1);
        row_count++;
    }
    output_data_block->Finalize();
    operator_state->data_block_array_.push_back(Move(output_data_block));
    fusion_operator_state->input_data_blocks_.clear();
    operator_state->SetComplete();
    return true;
}

String PhysicalFusion::ToString(i64 &space) const {
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> PhysicalFusion ";
    } else {
        arrow_str = "PhysicalFusion ";
    }
    String res = Format("{} {}", arrow_str, fusion_expr_->ToString());
    return res;
}

} // namespace infinity
