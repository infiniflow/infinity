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

module physical_export;

import query_context;
import operator_state;
import logger;
import statement_common;
import third_party;
import column_def;
import block_entry;
import column_vector;
import value;

namespace infinity {

void PhysicalExport::Init() {}

bool PhysicalExport::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ExportOperatorState *export_op_state = static_cast<ExportOperatorState *>(operator_state);
    switch (file_type_) {
        case CopyFileType::kCSV: {
            ExportToCSV(query_context, export_op_state);
            break;
        }
        case CopyFileType::kJSONL: {
            ExportToJSONL(query_context, export_op_state);
            break;
        }
        default: {
            UnrecoverableError("Not supported file type");
        }
    }

    auto result_msg = MakeUnique<String>("EXPORT 0 Rows");
    export_op_state->result_msg_ = std::move(result_msg);

    export_op_state->SetComplete();
    return true;
}

void PhysicalExport::ExportToCSV(QueryContext *query_context, ExportOperatorState *export_op_state) {
    LOG_DEBUG(fmt::format("Export to CSV"));
    LOG_DEBUG(fmt::format("Export file path: {}", file_path_));
    LOG_DEBUG(fmt::format("Export table_name: {}", table_name_));
    LOG_DEBUG(fmt::format("Export db_name: {}", schema_name_));

    Map<SegmentID, SegmentSnapshot>& segment_block_index_ref = block_index_->segment_block_index_;
    for(auto& [segment_id, segment_snapshot]: segment_block_index_ref) {
        LOG_DEBUG(fmt::format("Export segment_id: {}", segment_id));
        SizeT block_count = segment_snapshot.block_map_.size();
        for(SizeT block_idx = 0; block_idx < block_count; ++ block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
        }
    }

    LOG_DEBUG(fmt::format("Header ? {}", header_));
    LOG_DEBUG(fmt::format("Delimiter {}", delimiter_));
}

void PhysicalExport::ExportToJSONL(QueryContext *query_context, ExportOperatorState *export_op_state) {

    LOG_DEBUG(fmt::format("Export to JSONL"));
    LOG_DEBUG(fmt::format("Export file path: {}", file_path_));
    LOG_DEBUG(fmt::format("Export table_name: {}", table_name_));
    LOG_DEBUG(fmt::format("Export db_name: {}", schema_name_));
    const Vector<SharedPtr<ColumnDef>>& column_defs = table_entry_->column_defs();
    SizeT column_count = column_defs.size();


    Map<SegmentID, SegmentSnapshot>& segment_block_index_ref = block_index_->segment_block_index_;
    for(auto& [segment_id, segment_snapshot]: segment_block_index_ref) {
        LOG_DEBUG(fmt::format("Export segment_id: {}", segment_id));
        SizeT block_count = segment_snapshot.block_map_.size();
        for(SizeT block_idx = 0; block_idx < block_count; ++ block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockEntry *block_entry = segment_snapshot.block_map_[block_idx];
            SizeT block_row_count = block_entry->row_count();

            Vector<ColumnVector> column_vectors;
            column_vectors.reserve(column_count);
            for(SizeT column_idx = 0; column_idx < column_count; ++ column_idx) {
                column_vectors.emplace_back(block_entry->GetColumnBlockEntry(column_idx)->GetColumnVector(query_context->storage()->buffer_manager()));
                if(column_vectors[column_idx].Size() != block_row_count) {
                    UnrecoverableError("Unmatched row_count between block and block_column");
                }
            }

            for(SizeT row_idx = 0; row_idx < block_row_count;  ++ row_idx) {
                nlohmann::json line_json;
                for(SizeT column_idx = 0; column_idx < column_count; ++ column_idx) {
                    ColumnDef* column_def = column_defs[column_idx].get();
                    Value v = column_vectors[column_idx].GetValue(row_idx);
                    v.AppendToJson(column_def->name(), line_json);
                }
                LOG_DEBUG(line_json.dump());
//                LOG_DEBUG(fmt::format("{} : {}", column_def->name(), v.ToString()));
            }
        }
    }

    LOG_DEBUG(fmt::format("Header ? {}", header_));
    LOG_DEBUG(fmt::format("Delimiter {}", delimiter_));
}

} // namespace infinity
