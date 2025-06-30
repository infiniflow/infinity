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

#include "arrow/type_fwd.h"
#include <arrow/io/caching.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <parquet/properties.h>
#include <string>

module physical_export;

import query_context;
import operator_state;
import logger;
import statement_common;
import third_party;
import column_def;
import column_vector;
import value;
import virtual_store;
import stl;
import logical_type;
import embedding_info;
import sparse_info;
import array_info;
import status;
import buffer_manager;
import default_values;
import internal_types;
import virtual_store;
import local_file_handle;
import knn_filter;
import block_index;

import new_txn;
import block_meta;
import column_meta;
import new_catalog;
import roaring_bitmap;

namespace infinity {

PhysicalExport::PhysicalExport(u64 id,
                               const SharedPtr<TableInfo> &table_info,
                               String schema_name,
                               String table_name,
                               String file_path,
                               bool header,
                               char delimiter,
                               CopyFileType type,
                               SizeT offset,
                               SizeT limit,
                               SizeT row_limit,
                               Vector<u64> column_idx_array,
                               SharedPtr<BlockIndex> block_index,
                               SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kExport, nullptr, nullptr, id, load_metas), table_info_(table_info), file_type_(type),
      file_path_(std::move(file_path)), table_name_(std::move(table_name)), schema_name_(std::move(schema_name)), header_(header),
      delimiter_(delimiter), offset_(offset), limit_(limit), row_limit_(row_limit), column_idx_array_(std::move(column_idx_array)),
      block_index_(std::move(block_index)) {}

PhysicalExport::~PhysicalExport() = default;

void PhysicalExport::Init(QueryContext *query_context) {}

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
        case CopyFileType::kFVECS: {
            exported_row_count = ExportToFVECS(query_context, export_op_state);
            break;
        }
        case CopyFileType::kPARQUET: {
            exported_row_count = ExportToPARQUET(query_context, export_op_state);
            break;
        }
        default: {
            String error_message = "Not supported file type";
            UnrecoverableError(error_message);
        }
    }

    auto result_msg = MakeUnique<String>(fmt::format("EXPORT {} Rows", exported_row_count));
    export_op_state->result_msg_ = std::move(result_msg);

    export_op_state->SetComplete();
    return true;
}

SizeT PhysicalExport::ExportToCSV(QueryContext *query_context, ExportOperatorState *export_op_state) {
    return ExportToFileInner(query_context, export_op_state, [this](const Vector<ColumnVector> &column_vectors, SizeT row_idx) {
        String line;
        for (SizeT select_column_idx = 0; select_column_idx < column_vectors.size(); ++select_column_idx) {
            Value v = column_vectors[select_column_idx].GetValue(row_idx);
            switch (v.type().type()) {
                case LogicalType::kArray:
                case LogicalType::kEmbedding:
                case LogicalType::kMultiVector:
                case LogicalType::kTensor:
                case LogicalType::kTensorArray:
                case LogicalType::kSparse: {
                    line += fmt::format("\"{}\"", v.ToString());
                    break;
                }
                default: {
                    line += v.ToString();
                }
            }
            if (select_column_idx == column_vectors.size() - 1) {
                line += "\n";
            } else {
                line += delimiter_;
            }
        }
        return line;
    });
}

SizeT PhysicalExport::ExportToJSONL(QueryContext *query_context, ExportOperatorState *export_op_state) {
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_info_->column_defs_;

    Vector<ColumnID> select_columns;
    // export all columns or export specific column index
    if (column_idx_array_.empty()) {
        SizeT column_count = column_defs.size();
        select_columns.reserve(column_count);
        for (ColumnID idx = 0; idx < column_count; ++idx) {
            select_columns.emplace_back(idx);
        }
    } else {
        select_columns = column_idx_array_;
    }

    return ExportToFileInner(query_context, export_op_state, [&](const Vector<ColumnVector> &column_vectors, SizeT row_idx) {
        nlohmann::json line_json;
        for (ColumnID block_column_idx = 0; block_column_idx < column_vectors.size(); ++block_column_idx) {
            ColumnID select_column_idx = select_columns[block_column_idx];
            switch (select_column_idx) {
                case COLUMN_IDENTIFIER_ROW_ID: {
                    Value v = column_vectors[block_column_idx].GetValue(row_idx);
                    v.AppendToJson("_row_id", line_json);
                    break;
                }
                case COLUMN_IDENTIFIER_CREATE: {
                    Value v = column_vectors[block_column_idx].GetValue(row_idx);
                    v.AppendToJson("_create_timestamp", line_json);
                    break;
                }
                case COLUMN_IDENTIFIER_DELETE: {
                    Value v = column_vectors[block_column_idx].GetValue(row_idx);
                    v.AppendToJson("_delete_timestamp", line_json);
                    break;
                }
                default: {
                    ColumnDef *column_def = column_defs[select_column_idx].get();
                    Value v = column_vectors[block_column_idx].GetValue(row_idx);
                    v.AppendToJson(column_def->name(), line_json);
                }
            }
        }
        String to_write = line_json.dump() + "\n";
        return to_write;
    });
}

SizeT PhysicalExport::ExportToFileInner(QueryContext *query_context,
                                        ExportOperatorState *export_op_state,
                                        std::function<String(const Vector<ColumnVector> &, SizeT)> line_to_string) {
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_info_->column_defs_;

    Vector<ColumnID> select_columns;
    // export all columns or export specific column index
    if (column_idx_array_.empty()) {
        SizeT column_count = column_defs.size();
        select_columns.reserve(column_count);
        for (ColumnID idx = 0; idx < column_count; ++idx) {
            select_columns.emplace_back(idx);
        }
    } else {
        select_columns = column_idx_array_;
    }

    SizeT select_column_count = select_columns.size();

    String parent_path = VirtualStore::GetParentPath(file_path_);
    if (!parent_path.empty()) {
        Status create_status = VirtualStore::MakeDirectory(parent_path);
        if (!create_status.ok()) {
            RecoverableError(create_status);
        }
    }

    auto [file_handle, status] = VirtualStore::Open(file_path_, FileAccessMode::kWrite);
    if (!status.ok()) {
        RecoverableError(status);
    }

    if (header_ && file_type_ == CopyFileType::kCSV) {
        // Output CSV header
        String header;
        for (SizeT select_column_idx = 0; select_column_idx < select_column_count; ++select_column_idx) {
            ColumnID column_idx = select_columns[select_column_idx];
            switch (column_idx) {
                case COLUMN_IDENTIFIER_ROW_ID: {
                    header += "_row_id";
                    break;
                }
                case COLUMN_IDENTIFIER_CREATE: {
                    header += "_create_timestamp";
                    break;
                }
                case COLUMN_IDENTIFIER_DELETE: {
                    header += "_delete_timestamp";
                    break;
                }
                default: {
                    ColumnDef *column_def = column_defs[column_idx].get();
                    header += column_def->name();
                }
            }
            if (select_column_idx != select_column_count - 1) {
                header += delimiter_;
            } else {
                header += '\n';
            }
        }
        file_handle->Append(header.c_str(), header.size());
    }

    SizeT offset = offset_;
    SizeT row_count{0};
    SizeT file_no_{0};

    auto append_line = [&](const String &line) {
        if (row_count > 0 && this->row_limit_ != 0 && (row_count % this->row_limit_) == 0) {
            ++file_no_;
            String new_file_path = fmt::format("{}.part{}", file_path_, file_no_);
            auto [new_file_handle, new_status] = VirtualStore::Open(new_file_path, FileAccessMode::kWrite);
            if (!new_status.ok()) {
                RecoverableError(new_status);
            }
            file_handle = std::move(new_file_handle);
        }
        file_handle->Append(line.c_str(), line.size());
        ++row_count;
        if (limit_ != 0 && row_count == limit_) {
            return true;
        }
        return false;
    };

    NewTxn *new_txn = query_context->GetNewTxn();
    TxnTimeStamp begin_ts = new_txn->BeginTS();
    TxnTimeStamp commit_ts = new_txn->CommitTS();

    const Map<SegmentID, NewSegmentSnapshot> &segment_block_index_ref = block_index_->new_segment_block_index_;
    LOG_DEBUG(fmt::format("Going to export segment count: {}", segment_block_index_ref.size()));

    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map().size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));

            BlockMeta *block_meta = segment_snapshot.block_map()[block_idx].get();
            Vector<ColumnVector> column_vectors;
            column_vectors.resize(select_column_count);
            auto [block_row_count, status] = block_meta->GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                switch (select_column_idx) {
                    case COLUMN_IDENTIFIER_ROW_ID: {
                        column_vectors[block_column_idx] = ColumnVector(MakeShared<DataType>(LogicalType::kRowID));
                        column_vectors[block_column_idx].Initialize();
                        BlockID block_id = block_meta->block_id();
                        u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
                        column_vectors[block_column_idx].AppendWith(RowID(segment_id, segment_offset), block_row_count);
                        break;
                    }
                    case COLUMN_IDENTIFIER_CREATE: {
                        column_vectors[block_column_idx] = ColumnVector(MakeShared<DataType>(LogicalType::kBigInt));
                        column_vectors[block_column_idx].Initialize(ColumnVectorType::kFlat, block_row_count);
                        Status status = NewCatalog::GetCreateTSVector(*block_meta, 0, block_row_count, column_vectors[block_column_idx]);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        break;
                    }
                    case COLUMN_IDENTIFIER_DELETE: {
                        column_vectors[block_column_idx] = ColumnVector(MakeShared<DataType>(LogicalType::kBigInt));
                        column_vectors[block_column_idx].Initialize(ColumnVectorType::kFlat, block_row_count);
                        Status status = NewCatalog::GetDeleteTSVector(*block_meta, 0, block_row_count, column_vectors[block_column_idx]);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        break;
                    }
                    default: {
                        ColumnMeta column_meta(select_column_idx, *block_meta);
                        Status status =
                            NewCatalog::GetColumnVector(column_meta, block_row_count, ColumnVectorTipe::kReadOnly, column_vectors[block_column_idx]);
                        if (!status.ok()) {
                            UnrecoverableError("Failed to get column vector");
                        }
                    }
                }
            }

            Bitmask bitmask(block_row_count);
            status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            for (SizeT row_idx = 0; row_idx < block_row_count; ++row_idx) {
                if (!bitmask.IsTrue(row_idx)) {
                    continue;
                }
                if (offset > 0) {
                    --offset;
                    continue;
                }

                String line = line_to_string(column_vectors, row_idx);
                bool end = append_line(line);
                if (end) {
                    goto new_label_return;
                }
            }
        }
    }
new_label_return:
    LOG_DEBUG(fmt::format("Export to file, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SizeT PhysicalExport::ExportToFVECS(QueryContext *query_context, ExportOperatorState *export_op_state) {

    if (column_idx_array_.size() != 1) {
        String error_message = "Only one column with embedding data type can be exported as FVECS file";
        UnrecoverableError(error_message);
    }

    u64 exported_column_idx = column_idx_array_[0];
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_info_->column_defs_;
    DataType *data_type = column_defs[exported_column_idx]->type().get();
    if (data_type->type() != LogicalType::kEmbedding) {
        String error_message = fmt::format("Only embedding column can be exported as FVECS file, but it is {}", data_type->ToString());
        UnrecoverableError(error_message);
    }

    EmbeddingInfo *embedding_type_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
    switch (embedding_type_info->Type()) {
        case EmbeddingDataType::kElemFloat: {
            // Supported
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("Type: {}, only float element type embedding is supported now",
                                                           EmbeddingType::EmbeddingDataType2String(embedding_type_info->Type())));
            RecoverableError(status);
        }
    }

    i32 dimension = embedding_type_info->Dimension();

    String parent_path = VirtualStore::GetParentPath(file_path_);
    if (!parent_path.empty()) {
        Status create_status = VirtualStore::MakeDirectory(parent_path);
        if (!create_status.ok()) {
            RecoverableError(create_status);
        }
    }

    auto [file_handle, status] = VirtualStore::Open(file_path_, FileAccessMode::kWrite);
    if (!status.ok()) {
        RecoverableError(status);
    }

    SizeT offset = offset_;
    SizeT row_count{0};
    SizeT file_no_{0};

    auto append_line = [&](const Value &v) {
        Span<char> embedding = v.GetEmbedding();

        if (row_count > 0 && this->row_limit_ != 0 && (row_count % this->row_limit_) == 0) {
            ++file_no_;
            String new_file_path = fmt::format("{}.part{}", file_path_, file_no_);
            auto [new_file_handle, new_status] = VirtualStore::Open(new_file_path, FileAccessMode::kWrite);
            if (!new_status.ok()) {
                RecoverableError(new_status);
            }
            file_handle = std::move(new_file_handle);
        }

        file_handle->Append(&dimension, sizeof(dimension));
        file_handle->Append(embedding.data(), embedding.size_bytes());

        ++row_count;
    };

    NewTxn *new_txn = query_context->GetNewTxn();
    TxnTimeStamp begin_ts = new_txn->BeginTS();
    TxnTimeStamp commit_ts = new_txn->CommitTS();
    Map<SegmentID, NewSegmentSnapshot> &new_segment_block_index_ref = block_index_->new_segment_block_index_;
    LOG_DEBUG(fmt::format("Going to export segment count: {}", new_segment_block_index_ref.size()));
    for (auto &[segment_id, segment_snapshot] : new_segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map().size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockMeta *block_meta = segment_snapshot.block_map()[block_idx].get();

            auto [block_row_count, status] = block_meta->GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            ColumnMeta column_meta(exported_column_idx, *block_meta);
            ColumnVector exported_column_vector;
            status = NewCatalog::GetColumnVector(column_meta, block_row_count, ColumnVectorTipe::kReadOnly, exported_column_vector);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            Bitmask bitmask(block_row_count);
            status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            for (SizeT row_idx = 0; row_idx < block_row_count; ++row_idx) {
                if (!bitmask.IsTrue(row_idx)) {
                    continue;
                }
                if (offset > 0) {
                    --offset;
                    continue;
                }
                Value v = exported_column_vector.GetValue(row_idx);
                append_line(v);
                if (limit_ != 0 && row_count == limit_) {
                    goto new_label_return;
                }
            }
        }
    }
new_label_return:
    LOG_DEBUG(fmt::format("Export to FVECS, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SharedPtr<arrow::DataType> GetArrowType(const DataType &column_data_type);

SharedPtr<arrow::Array> BuildArrowArray(const ColumnDef *column_def, const ColumnVector &column_vector, const Vector<u32> &block_rows_for_output);

SizeT PhysicalExport::ExportToPARQUET(QueryContext *query_context, ExportOperatorState *export_op_state) {
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_info_->column_defs_;
    Vector<ColumnID> select_columns;
    // export all columns or export specific column index
    if (column_idx_array_.empty()) {
        SizeT column_count = column_defs.size();
        select_columns.reserve(column_count);
        for (ColumnID idx = 0; idx < column_count; ++idx) {
            select_columns.emplace_back(idx);
        }
    } else {
        select_columns = column_idx_array_;
    }

    SizeT select_column_count = select_columns.size();

    Vector<SharedPtr<arrow::Field>> fields;
    for (auto &column_id : select_columns) {
        ColumnDef *column_def = column_defs[column_id].get();
        auto arrow_type = GetArrowType(*(column_def->type()));
        fields.emplace_back(::arrow::field(column_def->name(), std::move(arrow_type)));
    }

    arrow::MemoryPool *pool = arrow::DefaultMemoryPool();
    SharedPtr<arrow::Schema> schema = ::arrow::schema(std::move(fields));
    SharedPtr<::arrow::io::FileOutputStream> file_stream;
    UniquePtr<::parquet::arrow::FileWriter> file_writer;

    String parent_path = VirtualStore::GetParentPath(file_path_);
    if (!parent_path.empty()) {
        Status create_status = VirtualStore::MakeDirectory(parent_path);
        if (!create_status.ok()) {
            RecoverableError(create_status);
        }
    }

    auto init_file_stream_writer = [&pool, &schema, &file_stream, &file_writer](const String &output_file_path) {
        file_writer.reset();
        file_stream.reset();
        auto file_stream_result = ::arrow::io::FileOutputStream::Open(output_file_path, pool);
        if (!file_stream_result.ok()) {
            RecoverableError(Status::IOError(file_stream_result.status().ToString()));
        }
        file_stream = std::move(file_stream_result).ValueOrDie();
        auto file_writer_result = ::parquet::arrow::FileWriter::Open(*schema, pool, file_stream, ::parquet::default_writer_properties());
        if (!file_writer_result.ok()) {
            RecoverableError(Status::IOError(file_writer_result.status().ToString()));
        }
        file_writer = std::move(file_writer_result).ValueOrDie();
    };
    init_file_stream_writer(file_path_);

    SizeT offset = offset_;
    SizeT row_count{0};
    SizeT file_no_{0};
    bool switch_to_new_file = false;
    auto consume_block =
        [&](Vector<ColumnVector> &column_vectors, const Bitmask *bitmask, SegmentOffset segment_offset, const SizeT block_row_count) -> bool {
        Vector<u32> block_rows_for_output;
        for (SizeT start_block_row_idx = 0; start_block_row_idx < block_row_count; ++start_block_row_idx) {
            bool need_switch_to_new_file = false;
            for (block_rows_for_output.clear(); start_block_row_idx < block_row_count; ++start_block_row_idx) {
                if (!bitmask->IsTrue(start_block_row_idx)) {
                    continue;
                }
                if (offset > 0) {
                    --offset;
                    continue;
                }
                block_rows_for_output.push_back(start_block_row_idx);
                ++row_count;
                if (row_limit_ != 0 && row_count % row_limit_ == 0) {
                    need_switch_to_new_file = true;
                    break;
                }
                if (row_count == limit_) {
                    break;
                }
            }
            if (block_rows_for_output.empty()) {
                continue;
            }
            if (switch_to_new_file) {
                if (auto status = file_writer->Close(); !status.ok()) {
                    RecoverableError(Status::IOError(fmt::format("Failed to close parquet file: {}", status.ToString())));
                }
                const String new_file_path = fmt::format("{}.part{}", file_path_, ++file_no_);
                init_file_stream_writer(new_file_path);
            }
            Vector<SharedPtr<arrow::Array>> block_arrays;
            for (SizeT i = 0; i < select_column_count; ++i) {
                const auto select_column_idx = select_columns[i];
                ColumnDef *column_def = column_defs[select_column_idx].get();
                ColumnVector &column_vector = column_vectors[i];
                block_arrays.emplace_back(BuildArrowArray(column_def, column_vector, block_rows_for_output));
            }
            SharedPtr<arrow::RecordBatch> block_batch = arrow::RecordBatch::Make(schema, block_rows_for_output.size(), std::move(block_arrays));
            if (auto status = file_writer->WriteRecordBatch(*block_batch); !status.ok()) {
                RecoverableError(Status::IOError(fmt::format("Failed to write record batch to parquet file: {}", status.ToString())));
            }
            switch_to_new_file = need_switch_to_new_file;
            if (row_count == limit_) {
                return false;
            }
        }
        return true;
    };

    Map<SegmentID, NewSegmentSnapshot> &segment_block_index_ref = block_index_->new_segment_block_index_;
    NewTxn *new_txn = query_context->GetNewTxn();
    TxnTimeStamp begin_ts = new_txn->BeginTS();
    TxnTimeStamp commit_ts = new_txn->CommitTS();

    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map().size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockMeta *block_meta = segment_snapshot.block_map()[block_idx].get();
            auto [block_row_count, status] = block_meta->GetRowCnt1();
            if (!status.ok()) {
                RecoverableError(status);
            }

            Vector<ColumnVector> column_vectors;
            column_vectors.resize(select_column_count);

            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                switch (select_column_idx) {
                    case COLUMN_IDENTIFIER_ROW_ID:
                    case COLUMN_IDENTIFIER_CREATE:
                    case COLUMN_IDENTIFIER_DELETE: {
                        UnrecoverableError("Not implemented");
                        break;
                    }
                    default: {
                        ColumnMeta column_meta(select_column_idx, *block_meta);
                        Status status =
                            NewCatalog::GetColumnVector(column_meta, block_row_count, ColumnVectorTipe::kReadOnly, column_vectors[block_column_idx]);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                    }
                }
            }
            Bitmask bitmask(block_row_count);
            status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
            if (!status.ok()) {
                RecoverableError(status);
            }
            if (!consume_block(column_vectors, &bitmask, 0, block_row_count)) {
                goto new_label_return;
            }
        }
    }

new_label_return:
    if (auto status = file_writer->Close(); !status.ok()) {
        RecoverableError(Status::IOError(fmt::format("Failed to close parquet file: {}", status.ToString())));
    }
    LOG_DEBUG(fmt::format("Export to PARQUET, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SharedPtr<arrow::DataType> GetArrowType(const DataType &column_data_type) {
    switch (const auto column_logical_type = column_data_type.type(); column_logical_type) {
        case LogicalType::kBoolean:
            return arrow::boolean();
        case LogicalType::kTinyInt:
            return arrow::int8();
        case LogicalType::kSmallInt:
            return arrow::int16();
        case LogicalType::kInteger:
            return arrow::int32();
        case LogicalType::kBigInt:
            return arrow::int64();
        case LogicalType::kFloat16:
            return arrow::float16();
        case LogicalType::kBFloat16:
            return arrow::float32();
        case LogicalType::kFloat:
            return arrow::float32();
        case LogicalType::kDouble:
            return arrow::float64();
        case LogicalType::kDate:
            return arrow::date32();
        case LogicalType::kTime:
            return arrow::time32(arrow::TimeUnit::SECOND);
        case LogicalType::kDateTime:
        case LogicalType::kTimestamp:
            return arrow::timestamp(arrow::TimeUnit::SECOND);
        case LogicalType::kVarchar:
            return arrow::utf8();
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<const SparseInfo *>(column_data_type.type_info().get());
            SharedPtr<arrow::DataType> index_type;
            Optional<SharedPtr<arrow::DataType>> value_type = None;
            switch (sparse_info->IndexType()) {
                case EmbeddingDataType::kElemInt8: {
                    index_type = ::arrow::list(::arrow::int8());
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    index_type = ::arrow::list(::arrow::int16());
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    index_type = ::arrow::list(::arrow::int32());
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    index_type = ::arrow::list(::arrow::int64());
                    break;
                }
                default: {
                    UnrecoverableError("Index type invalid");
                }
            }
            switch (sparse_info->DataType()) {
                case EmbeddingDataType::kElemBit: {
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    value_type = ::arrow::list(::arrow::int8());
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    value_type = ::arrow::list(::arrow::int16());
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    value_type = ::arrow::list(::arrow::int32());
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    value_type = ::arrow::list(::arrow::int64());
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    value_type = ::arrow::list(::arrow::float32());
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    value_type = ::arrow::list(::arrow::float64());
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    value_type = ::arrow::list(::arrow::uint8());
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    value_type = ::arrow::list(::arrow::float16());
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    value_type = ::arrow::list(::arrow::float32());
                    break;
                }
                default: {
                    UnrecoverableError("Data type invalid");
                }
            }

            arrow::FieldVector fields{::arrow::field("index", std::move(index_type))};
            if (value_type.has_value()) {
                fields.emplace_back(::arrow::field("value", value_type.value()));
            }
            return arrow::struct_(std::move(fields));
        }
        case LogicalType::kEmbedding:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_data_type.type_info().get());
            const SizeT dimension = embedding_info->Dimension();
            SharedPtr<arrow::DataType> arrow_embedding_elem_type;
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    arrow_embedding_elem_type = ::arrow::boolean();
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    arrow_embedding_elem_type = ::arrow::int8();
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    arrow_embedding_elem_type = ::arrow::int16();
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    arrow_embedding_elem_type = ::arrow::int32();
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    arrow_embedding_elem_type = ::arrow::int64();
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    arrow_embedding_elem_type = ::arrow::float32();
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    arrow_embedding_elem_type = ::arrow::float64();
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    arrow_embedding_elem_type = ::arrow::uint8();
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    arrow_embedding_elem_type = ::arrow::float16();
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    arrow_embedding_elem_type = ::arrow::float32();
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid case EmbeddingDataType::kElemInvalid");
                    break;
                }
            }
            auto arrow_embedding_type = ::arrow::fixed_size_list(std::move(arrow_embedding_elem_type), dimension);
            if (column_logical_type == LogicalType::kEmbedding) {
                return arrow_embedding_type;
            }
            auto arrow_tensor_type = ::arrow::list(std::move(arrow_embedding_type));
            if (column_logical_type == LogicalType::kTensor || column_logical_type == LogicalType::kMultiVector) {
                return arrow_tensor_type;
            }
            auto arrow_tensor_array_type = ::arrow::list(std::move(arrow_tensor_type));
            if (column_logical_type == LogicalType::kTensorArray) {
                return arrow_tensor_array_type;
            }
            UnrecoverableError("Unreachable code!");
            return {};
        }
        case LogicalType::kArray: {
            const auto *array_info = static_cast<const ArrayInfo *>(column_data_type.type_info().get());
            auto element_arrow_type = GetArrowType(array_info->ElemType());
            return ::arrow::list(std::move(element_arrow_type));
        }
        case LogicalType::kRowID:
        case LogicalType::kInterval:
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kEmptyArray:
        case LogicalType::kInvalid: {
            String error_message = "Invalid data type";
            UnrecoverableError(error_message);
        }
    }
    return nullptr;
}

SharedPtr<arrow::ArrayBuilder> GetArrowBuilder(const DataType &column_type) {
    SharedPtr<arrow::ArrayBuilder> array_builder{};
    switch (const auto column_logical_type = column_type.type(); column_logical_type) {
        case LogicalType::kBoolean: {
            array_builder = MakeShared<arrow::BooleanBuilder>();
            break;
        }
        case LogicalType::kTinyInt: {
            array_builder = MakeShared<::arrow::Int8Builder>();
            break;
        }
        case LogicalType::kSmallInt: {
            array_builder = MakeShared<::arrow::Int16Builder>();
            break;
        }
        case LogicalType::kInteger: {
            array_builder = MakeShared<::arrow::Int32Builder>();
            break;
        }
        case LogicalType::kBigInt: {
            array_builder = MakeShared<::arrow::Int64Builder>();
            break;
        }
        case LogicalType::kFloat: {
            array_builder = MakeShared<::arrow::FloatBuilder>();
            break;
        }
        case LogicalType::kDouble: {
            array_builder = MakeShared<::arrow::DoubleBuilder>();
            break;
        }
        case LogicalType::kFloat16: {
            array_builder = MakeShared<::arrow::HalfFloatBuilder>();
            break;
        }
        case LogicalType::kBFloat16: {
            array_builder = MakeShared<::arrow::FloatBuilder>();
            break;
        }
        case LogicalType::kDate: {
            array_builder = MakeShared<::arrow::Date32Builder>();
            break;
        }
        case LogicalType::kTime: {
            array_builder = MakeShared<::arrow::Time32Builder>(arrow::time32(arrow::TimeUnit::SECOND), arrow::DefaultMemoryPool());
            break;
        }
        case LogicalType::kDateTime:
        case LogicalType::kTimestamp: {
            array_builder = MakeShared<::arrow::TimestampBuilder>(::arrow::timestamp(arrow::TimeUnit::SECOND), arrow::DefaultMemoryPool());
            break;
        }
        case LogicalType::kVarchar: {
            array_builder = MakeShared<::arrow::StringBuilder>();
            break;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<const SparseInfo *>(column_type.type_info().get());
            SharedPtr<arrow::ArrayBuilder> index_builder = nullptr;
            SharedPtr<arrow::ArrayBuilder> value_builder = nullptr;
            switch (sparse_info->IndexType()) {
                case EmbeddingDataType::kElemInt8: {
                    auto int8_builder = MakeShared<::arrow::Int8Builder>();
                    index_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int8_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    auto int16_builder = MakeShared<::arrow::Int16Builder>();
                    index_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int16_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    auto int32_builder = MakeShared<::arrow::Int32Builder>();
                    index_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int32_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    auto int64_builder = MakeShared<::arrow::Int64Builder>();
                    index_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int64_builder);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid index type.");
                }
            }
            switch (sparse_info->DataType()) {
                case EmbeddingDataType::kElemBit: {
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    auto int8_builder = MakeShared<::arrow::Int8Builder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int8_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    auto int16_builder = MakeShared<::arrow::Int16Builder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int16_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    auto int32_builder = MakeShared<::arrow::Int32Builder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int32_builder);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    auto int64_builder = MakeShared<::arrow::Int64Builder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int64_builder);
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    auto float_builder = MakeShared<::arrow::FloatBuilder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), float_builder);
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    auto double_builder = MakeShared<::arrow::DoubleBuilder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), double_builder);
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    auto uint8_builder = MakeShared<::arrow::UInt8Builder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), uint8_builder);
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    auto float16_builder = MakeShared<::arrow::HalfFloatBuilder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), float16_builder);
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    auto float_builder = MakeShared<::arrow::FloatBuilder>();
                    value_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), float_builder);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid data type.");
                }
            }
            auto struct_type = arrow::struct_({arrow::field("index", index_builder->type()), arrow::field("value", value_builder->type())});
            Vector<SharedPtr<arrow::ArrayBuilder>> field_builders = {index_builder};
            if (value_builder.get() != nullptr) {
                field_builders.emplace_back(value_builder);
            }
            array_builder = MakeShared<::arrow::StructBuilder>(struct_type, arrow::DefaultMemoryPool(), std::move(field_builders));
            break;
        }
        case LogicalType::kEmbedding:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_type.type_info().get());
            SharedPtr<::arrow::ArrayBuilder> embedding_element_builder;
            switch (embedding_info->Type()) {
                case EmbeddingDataType::kElemBit: {
                    embedding_element_builder = MakeShared<::arrow::BooleanBuilder>();
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    embedding_element_builder = MakeShared<::arrow::Int8Builder>();
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    embedding_element_builder = MakeShared<::arrow::Int16Builder>();
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    embedding_element_builder = MakeShared<::arrow::Int32Builder>();
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    embedding_element_builder = MakeShared<::arrow::Int64Builder>();
                    break;
                }
                case EmbeddingDataType::kElemFloat: {
                    embedding_element_builder = MakeShared<::arrow::FloatBuilder>();
                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    embedding_element_builder = MakeShared<::arrow::DoubleBuilder>();
                    break;
                }
                case EmbeddingDataType::kElemUInt8: {
                    embedding_element_builder = MakeShared<::arrow::UInt8Builder>();
                    break;
                }
                case EmbeddingDataType::kElemFloat16: {
                    embedding_element_builder = MakeShared<::arrow::HalfFloatBuilder>();
                    break;
                }
                case EmbeddingDataType::kElemBFloat16: {
                    embedding_element_builder = MakeShared<::arrow::FloatBuilder>();
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    String error_message = "Invalid embedding data type: EmbeddingDataType::kElemInvalid";
                    UnrecoverableError(error_message);
                    break;
                }
            }
            const SizeT dimension = embedding_info->Dimension();
            auto embedding_arrow_array_builder =
                MakeShared<::arrow::FixedSizeListBuilder>(arrow::DefaultMemoryPool(), embedding_element_builder, dimension);
            if (column_logical_type == LogicalType::kEmbedding) {
                array_builder = std::move(embedding_arrow_array_builder);
                break;
            }
            auto tensor_arrow_array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), embedding_arrow_array_builder);
            if (column_logical_type == LogicalType::kTensor || column_logical_type == LogicalType::kMultiVector) {
                array_builder = std::move(tensor_arrow_array_builder);
                break;
            }
            auto tensor_array_arrow_array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), tensor_arrow_array_builder);
            if (column_logical_type == LogicalType::kTensorArray) {
                array_builder = std::move(tensor_array_arrow_array_builder);
                break;
            }
            UnrecoverableError("Unreachable code!");
            break;
        }
        case LogicalType::kArray: {
            const auto *array_info = static_cast<const ArrayInfo *>(column_type.type_info().get());
            auto element_arrow_builder = GetArrowBuilder(array_info->ElemType());
            array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), element_arrow_builder);
            break;
        }
        case LogicalType::kRowID:
        case LogicalType::kInterval:
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kEmptyArray:
        case LogicalType::kInvalid: {
            String error_message = "Invalid data type";
            UnrecoverableError(error_message);
        }
    }
    return array_builder;
}

SharedPtr<arrow::Array> BuildArrowArray(const ColumnDef *column_def, const ColumnVector &column_vector, const Vector<u32> &block_rows_for_output) {
    auto &column_type = column_def->type();
    SharedPtr<arrow::ArrayBuilder> array_builder = GetArrowBuilder(*column_type);

    for (const auto idx : block_rows_for_output) {
        auto value = column_vector.GetValue(idx);
        value.AppendToArrowArray(*column_type, array_builder.get());
    }

    SharedPtr<arrow::Array> array;
    if (auto status = array_builder->Finish(&array); !status.ok()) {
        UnrecoverableError(fmt::format("Failed to build arrow array: {}", status.message()));
    }
    return array;
}

} // namespace infinity
