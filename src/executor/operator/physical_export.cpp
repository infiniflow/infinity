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

#include <string>

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
import local_file_system;
import file_system;
import file_system_type;
import defer_op;
import stl;

namespace infinity {

void PhysicalExport::Init() {}

bool PhysicalExport::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ExportOperatorState *export_op_state = static_cast<ExportOperatorState *>(operator_state);
    SizeT exported_row_count{0};
    switch (file_type_) {
        case CopyFileType::kCSV: {
            exported_row_count = ExportToCSV(query_context, export_op_state);
            break;
        }
        case CopyFileType::kJSONL: {
            exported_row_count = ExportToJSONL(query_context, export_op_state);
            break;
        }
        default: {
            String error_message = "Not supported file type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    auto result_msg = MakeUnique<String>(fmt::format("EXPORT {} Rows", exported_row_count));
    export_op_state->result_msg_ = std::move(result_msg);

    export_op_state->SetComplete();
    return true;
}

SizeT PhysicalExport::ExportToCSV(QueryContext *query_context, ExportOperatorState *export_op_state) {
    const Vector<SharedPtr<ColumnDef>>& column_defs = table_entry_->column_defs();
    SizeT column_count = column_defs.size();

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);
    if(!status.ok()) {
        RecoverableError(status);
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    if(header_) {
        // Output CSV header
        String header;
        for(SizeT column_idx = 0; column_idx < column_count; ++ column_idx) {
            ColumnDef* column_def = column_defs[column_idx].get();
            header += column_def->name();
            if(column_idx != column_count - 1) {
                header += delimiter_;
            } else {
                header += '\n';
            }
        }
        fs.Write(*file_handler, header.c_str(), header.size());
    }

    SizeT row_count{0};
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
                    String error_message = "Unmatched row_count between block and block_column";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }

            for(SizeT row_idx = 0; row_idx < block_row_count;  ++ row_idx) {
                String line;
                for(SizeT column_idx = 0; column_idx < column_count; ++ column_idx) {
                    Value v = column_vectors[column_idx].GetValue(row_idx);
                    line += v.ToString();
                    if(column_idx == column_count - 1) {
                        line += "\n";
                    } else {
                        line += delimiter_;
                    }
                }
                fs.Write(*file_handler, line.c_str(), line.size());
                ++ row_count;
            }
        }
    }
    LOG_DEBUG(fmt::format("Export to CSV, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SizeT PhysicalExport::ExportToJSONL(QueryContext *query_context, ExportOperatorState *export_op_state) {

    const Vector<SharedPtr<ColumnDef>>& column_defs = table_entry_->column_defs();
    SizeT column_count = column_defs.size();

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);
    if(!status.ok()) {
        RecoverableError(status);
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT row_count{0};
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
                    String error_message = "Unmatched row_count between block and block_column";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
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
                String to_write = line_json.dump() + "\n";
                fs.Write(*file_handler, to_write.c_str(), to_write.size());
                ++ row_count;
            }
        }
    }
    LOG_DEBUG(fmt::format("Export to JSONL, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

} // namespace infinity
