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

export module logical_match_tensor_scan;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import logical_match_scan_base;
import base_expression;
import match_tensor_expression;
import base_table_ref;
import table_entry;
import internal_types;
import data_type;
import fast_rough_filter;
import common_query_filter;
import default_values;

namespace infinity {

export struct MatchTensorScanIndexOptions {
    // emvb index options
    u32 topn_ = 0;
    u32 emvb_centroid_nprobe_ = EMVB_CENTROID_NPROBE;
    f32 emvb_threshold_first_ = EMVB_THRESHOLD_FIRST;
    u32 emvb_n_doc_to_score_ = topn_ * EMVB_N_DOC_TO_SCORE_FACTOR;
    u32 emvb_n_doc_out_second_stage_ = topn_ * EMVB_N_DOC_OUT_SECOND_STAGE_FACTOR;
    f32 emvb_threshold_final_ = EMVB_THRESHOLD_FINAL;
    explicit MatchTensorScanIndexOptions(u32 topn) : topn_(topn) {}
};

export class LogicalMatchTensorScan final : public LogicalMatchScanBase {
public:
    explicit LogicalMatchTensorScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<MatchTensorExpression> match_tensor_expr)
        : LogicalMatchScanBase(node_id, LogicalNodeType::kMatchTensorScan, base_table_ref, match_tensor_expr) {}

    String name() override { return "LogicalMatchTensorScan"; }

    void InitExtraOptions();

public:
    // extra options
    // will be parsed in InitExtraOptions()
    u32 topn_ = 0;
    UniquePtr<MatchTensorScanIndexOptions> index_options_;
};

} // namespace infinity
