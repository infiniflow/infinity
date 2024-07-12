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

#include <sstream>

module logical_match_tensor_scan;

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import match_tensor_expression;
import default_values;
import logical_type;
import internal_types;
import third_party;
import explain_logical_plan;
import search_options;
import infinity_exception;
import status;
import logger;

namespace infinity {

void LogicalMatchTensorScan::InitExtraOptions() {
    auto match_tensor_expr = static_cast<MatchTensorExpression *>(query_expression_.get());
    SearchOptions options(match_tensor_expr->options_text_);
    // topn option
    auto top_n_it = options.options_.find("topn");
    if (top_n_it == options.options_.end()) {
        top_n_it = options.options_.find("top_n");
        if (top_n_it == options.options_.end()) {
            top_n_it = options.options_.find("topk");
            if (top_n_it == options.options_.end()) {
                top_n_it = options.options_.find("top_k");
            }
        }
    }
    if (top_n_it != options.options_.end()) {
        const int top_n_option = std::stoi(top_n_it->second);
        if (top_n_option <= 0) {
            Status status = Status::SyntaxError("topn must be a positive integer");
            RecoverableError(std::move(status));
        }
        topn_ = top_n_option;
    } else {
        topn_ = DEFAULT_MATCH_TENSOR_OPTION_TOP_N;
    }
    index_options_ = MakeUnique<MatchTensorScanIndexOptions>(topn_);
    if (const auto emvb_centroid_nprobe_it = options.options_.find("emvb_centroid_nprobe"); emvb_centroid_nprobe_it != options.options_.end()) {
        const auto emvb_centroid_nprobe_candidate = std::stoi(emvb_centroid_nprobe_it->second);
        if (emvb_centroid_nprobe_candidate <= 0) {
            Status status = Status::SyntaxError("emvb_centroid_nprobe must be a positive integer");
            RecoverableError(std::move(status));
        }
        index_options_->emvb_centroid_nprobe_ = emvb_centroid_nprobe_candidate;
    }
    if (const auto emvb_threshold_first_it = options.options_.find("emvb_threshold_first"); emvb_threshold_first_it != options.options_.end()) {
        index_options_->emvb_threshold_first_ = std::stof(emvb_threshold_first_it->second);
    }
    if (const auto emvb_n_doc_to_score_it = options.options_.find("emvb_n_doc_to_score"); emvb_n_doc_to_score_it != options.options_.end()) {
        const auto emvb_n_doc_to_score_candidate = std::stoi(emvb_n_doc_to_score_it->second);
        if (emvb_n_doc_to_score_candidate <= 0) {
            Status status = Status::SyntaxError("emvb_n_doc_to_score must be a positive integer");
            RecoverableError(std::move(status));
        }
        index_options_->emvb_n_doc_to_score_ = emvb_n_doc_to_score_candidate;
    }
    if (index_options_->emvb_n_doc_to_score_ < topn_) {
        Status status = Status::SyntaxError("emvb_n_doc_to_score must be at least topn");
        RecoverableError(std::move(status));
    }
    if (const auto emvb_n_doc_out_second_stage_it = options.options_.find("emvb_n_doc_out_second_stage");
        emvb_n_doc_out_second_stage_it != options.options_.end()) {
        const auto emvb_n_doc_out_second_stage_candidate = std::stoi(emvb_n_doc_out_second_stage_it->second);
        if (emvb_n_doc_out_second_stage_candidate <= 0) {
            Status status = Status::SyntaxError("emvb_n_doc_out_second_stage must be a positive integer");
            RecoverableError(std::move(status));
        }
        index_options_->emvb_n_doc_out_second_stage_ = emvb_n_doc_out_second_stage_candidate;
    }
    if (index_options_->emvb_n_doc_out_second_stage_ > index_options_->emvb_n_doc_to_score_) {
        Status status = Status::SyntaxError("emvb_n_doc_out_second_stage must be no more than emvb_n_doc_to_score");
        RecoverableError(std::move(status));
    }
    if (index_options_->emvb_n_doc_out_second_stage_ < topn_) {
        Status status = Status::SyntaxError("emvb_n_doc_out_second_stage must be no less than topn");
        RecoverableError(std::move(status));
    }
    if (const auto emvb_threshold_final_it = options.options_.find("emvb_threshold_final"); emvb_threshold_final_it != options.options_.end()) {
        index_options_->emvb_threshold_final_ = std::stof(emvb_threshold_final_it->second);
    }
}

} // namespace infinity
