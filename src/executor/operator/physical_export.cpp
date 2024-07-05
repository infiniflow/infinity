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
#include "arrow/type_fwd.h"
#include <arrow/io/caching.h>
#include <arrow/io/file.h>
#include <parquet/properties.h>
#include <parquet/arrow/writer.h>

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
import logical_type;
import embedding_info;
import status;
import buffer_manager;
import default_values;
import internal_types;

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
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_entry_->column_defs();

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

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);
    if (!status.ok()) {
        RecoverableError(status);
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    if (header_) {
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
        fs.Write(*file_handler, header.c_str(), header.size());
    }

    SizeT row_count{0};
    Map<SegmentID, SegmentSnapshot> &segment_block_index_ref = block_index_->segment_block_index_;
    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        LOG_DEBUG(fmt::format("Export segment_id: {}", segment_id));
        SizeT block_count = segment_snapshot.block_map_.size();
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockEntry *block_entry = segment_snapshot.block_map_[block_idx];
            SizeT block_row_count = block_entry->row_count();

            Vector<ColumnVector> column_vectors;
            column_vectors.reserve(select_column_count);

            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                switch (select_column_idx) {
                    case COLUMN_IDENTIFIER_ROW_ID: {
                        u16 block_id = block_entry->block_id();
                        u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
                        auto column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kRowID));
                        column_vector.Initialize();
                        column_vector.AppendWith(RowID(segment_id, segment_offset), block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_CREATE: {
                        ColumnVector column_vector = block_entry->GetCreateTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_DELETE: {
                        ColumnVector column_vector = block_entry->GetDeleteTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    default: {
                        column_vectors.emplace_back(block_entry->GetColumnBlockEntry(select_column_idx)->GetColumnVector(buffer_manager));
                        if (column_vectors[block_column_idx].Size() != block_row_count) {
                            String error_message = "Unmatched row_count between block and block_column";
                            LOG_CRITICAL(error_message);
                            UnrecoverableError(error_message);
                        }
                    }
                }
            }

            for (SizeT row_idx = 0; row_idx < block_row_count; ++row_idx) {
                String line;
                for (SizeT select_column_idx = 0; select_column_idx < select_column_count; ++select_column_idx) {
                    // TODO: Check the visibility
                    Value v = column_vectors[select_column_idx].GetValue(row_idx);
                    switch (v.type().type()) {
                        case LogicalType::kEmbedding: {
                            line += fmt::format("\"{}\"", v.ToString());
                            break;
                        }
                        default: {
                            line += v.ToString();
                        }
                    }
                    if (select_column_idx == select_column_count - 1) {
                        line += "\n";
                    } else {
                        line += delimiter_;
                    }
                }
                fs.Write(*file_handler, line.c_str(), line.size());
                ++row_count;
            }
        }
    }
    LOG_DEBUG(fmt::format("Export to CSV, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SizeT PhysicalExport::ExportToJSONL(QueryContext *query_context, ExportOperatorState *export_op_state) {

    const Vector<SharedPtr<ColumnDef>> &column_defs = table_entry_->column_defs();

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

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);
    if (!status.ok()) {
        RecoverableError(status);
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT row_count{0};
    Map<SegmentID, SegmentSnapshot> &segment_block_index_ref = block_index_->segment_block_index_;
    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    LOG_DEBUG(fmt::format("Going to export segment count: {}", segment_block_index_ref.size()));
    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map_.size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockEntry *block_entry = segment_snapshot.block_map_[block_idx];
            SizeT block_row_count = block_entry->row_count();

            Vector<ColumnVector> column_vectors;
            column_vectors.reserve(select_column_count);

            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                switch (select_column_idx) {
                    case COLUMN_IDENTIFIER_ROW_ID: {
                        u16 block_id = block_entry->block_id();
                        u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
                        auto column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kRowID));
                        column_vector.Initialize();
                        column_vector.AppendWith(RowID(segment_id, segment_offset), block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_CREATE: {
                        ColumnVector column_vector = block_entry->GetCreateTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_DELETE: {
                        ColumnVector column_vector = block_entry->GetDeleteTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    default: {
                        column_vectors.emplace_back(block_entry->GetColumnBlockEntry(select_column_idx)->GetColumnVector(buffer_manager));
                        if (column_vectors[block_column_idx].Size() != block_row_count) {
                            String error_message = "Unmatched row_count between block and block_column";
                            LOG_CRITICAL(error_message);
                            UnrecoverableError(error_message);
                        }
                    }
                }
            }

            for (SizeT row_idx = 0; row_idx < block_row_count; ++row_idx) {
                nlohmann::json line_json;

                // TODO: Need to check visibility

                for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
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
                LOG_DEBUG(line_json.dump());
                String to_write = line_json.dump() + "\n";
                fs.Write(*file_handler, to_write.c_str(), to_write.size());
                ++row_count;
            }
        }
    }
    LOG_DEBUG(fmt::format("Export to JSONL, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SizeT PhysicalExport::ExportToFVECS(QueryContext *query_context, ExportOperatorState *export_op_state) {

    if (column_idx_array_.size() != 1) {
        String error_message = "Only one column with embedding data type can be exported as FVECS file";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    u64 exported_column_idx = column_idx_array_[0];
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_entry_->column_defs();
    DataType *data_type = column_defs[exported_column_idx]->type().get();
    if (data_type->type() != LogicalType::kEmbedding) {
        String error_message = fmt::format("Only embedding column can be exported as FVECS file, but it is {}", data_type->ToString());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    EmbeddingInfo *embedding_type_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
    if (embedding_type_info->Type() != EmbeddingDataType::kElemFloat) {
        Status status = Status::NotSupport("Only float element type embedding is supported now.");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }

    i32 dimension = embedding_type_info->Dimension();

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);
    if (!status.ok()) {
        RecoverableError(status);
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT row_count{0};
    Map<SegmentID, SegmentSnapshot> &segment_block_index_ref = block_index_->segment_block_index_;
    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    // Write header
    LOG_DEBUG(fmt::format("Going to export segment count: {}", segment_block_index_ref.size()));
    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map_.size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockEntry *block_entry = segment_snapshot.block_map_[block_idx];
            SizeT block_row_count = block_entry->row_count();

            ColumnVector exported_column_vector = block_entry->GetColumnBlockEntry(exported_column_idx)->GetColumnVector(buffer_manager);
            if (exported_column_vector.Size() != block_row_count) {
                String error_message = "Unmatched row_count between block and block_column";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }

            for (SizeT row_idx = 0; row_idx < block_row_count; ++row_idx) {
                Value v = exported_column_vector.GetValue(row_idx);
                Span<char> embedding = v.GetEmbedding();
                fs.Write(*file_handler, &dimension, sizeof(dimension));
                fs.Write(*file_handler, embedding.data(), embedding.size_bytes());
                ++row_count;
            }
        }
    }
    LOG_DEBUG(fmt::format("Export to FVECS, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SizeT PhysicalExport::ExportToPARQUET(QueryContext *query_context, ExportOperatorState *export_op_state) {
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_entry_->column_defs();
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
        auto arrow_type = GetArrowType(column_def);
        fields.emplace_back(::arrow::field(column_def->name(), std::move(arrow_type)));
    }

    arrow::MemoryPool *pool = arrow::DefaultMemoryPool();
    SharedPtr<arrow::Schema> schema = ::arrow::schema(fields);
    SharedPtr<::arrow::io::FileOutputStream> file_stream;
    SharedPtr<::parquet::arrow::FileWriter> file_writer;

    file_stream = ::arrow::io::FileOutputStream::Open(file_path_, pool).ValueOrDie();
    file_writer = ::parquet::arrow::FileWriter::Open(*schema, pool, file_stream, ::parquet::default_writer_properties()).ValueOrDie();

    SizeT row_count{0};
    Map<SegmentID, SegmentSnapshot> &segment_block_index_ref = block_index_->segment_block_index_;
    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    for (auto &[segment_id, segment_snapshot] : segment_block_index_ref) {
        SizeT block_count = segment_snapshot.block_map_.size();
        LOG_DEBUG(fmt::format("Export segment_id: {}, with block count: {}", segment_id, block_count));
        for (SizeT block_idx = 0; block_idx < block_count; ++block_idx) {
            LOG_DEBUG(fmt::format("Export block_idx: {}", block_idx));
            BlockEntry *block_entry = segment_snapshot.block_map_[block_idx];
            SizeT block_row_count = block_entry->row_count();

            Vector<ColumnVector> column_vectors;
            column_vectors.reserve(select_column_count);

            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                switch (select_column_idx) {
                    case COLUMN_IDENTIFIER_ROW_ID: {
                        u16 block_id = block_entry->block_id();
                        u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
                        auto column_vector = ColumnVector(MakeShared<DataType>(LogicalType::kRowID));
                        column_vector.Initialize();
                        column_vector.AppendWith(RowID(segment_id, segment_offset), block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_CREATE: {
                        ColumnVector column_vector = block_entry->GetCreateTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    case COLUMN_IDENTIFIER_DELETE: {
                        ColumnVector column_vector = block_entry->GetDeleteTSVector(buffer_manager, 0, block_row_count);
                        column_vectors.emplace_back(column_vector);
                        break;
                    }
                    default: {
                        column_vectors.emplace_back(block_entry->GetColumnBlockEntry(select_column_idx)->GetColumnVector(buffer_manager));
                        if (column_vectors[block_column_idx].Size() != block_row_count) {
                            String error_message = "Unmatched row_count between block and block_column";
                            LOG_CRITICAL(error_message);
                            UnrecoverableError(error_message);
                        }
                    }
                }
            }

            Vector<SharedPtr<arrow::Array>> block_arrays; 
            for (ColumnID block_column_idx = 0; block_column_idx < select_column_count; ++block_column_idx) {
                ColumnID select_column_idx = select_columns[block_column_idx];
                ColumnDef *column_def = column_defs[select_column_idx].get();
                ColumnVector &column_vector = column_vectors[block_column_idx];
                block_arrays.emplace_back(BuildArrowArray(column_def, column_vector));
            }


            SharedPtr<arrow::RecordBatch> block_batch = arrow::RecordBatch::Make(schema, block_row_count, block_arrays);
            auto status = file_writer->WriteRecordBatch(*block_batch);
            if (!status.ok()) {
                String error_message = fmt::format("Failed to write record batch to parquet file: {}", status.message());
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            row_count += block_row_count;
        }
    }

    auto status = file_writer->Close();
    if (!status.ok()) {
        String error_message = fmt::format("Failed to close parquet file: {}", status.message());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    LOG_DEBUG(fmt::format("Export to PARQUET, db {}, table {}, file: {}, row: {}", schema_name_, table_name_, file_path_, row_count));
    return row_count;
}

SharedPtr<arrow::DataType> PhysicalExport::GetArrowType(ColumnDef *column_def) {
    auto &column_type = column_def->type();
    switch (column_type->type()) {
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
        case LogicalType::kFloat:
            return arrow::float32();
        case LogicalType::kDouble:
            return arrow::float64();
        case LogicalType::kDecimal: {
            return arrow::decimal(64, 64);
        }
        case LogicalType::kDate:
            return arrow::date32();
        case LogicalType::kTime:
            return arrow::time32(arrow::TimeUnit::SECOND);
        case LogicalType::kDateTime:
            return arrow::timestamp(arrow::TimeUnit::SECOND);
        case LogicalType::kInterval:
            return arrow::duration(arrow::TimeUnit::SECOND);
        case LogicalType::kVarchar:
            return arrow::utf8();
        case LogicalType::kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
            switch (embedding_info->Type()) {
                case kElemInt8: {
                    return ::arrow::list(::arrow::int8());
                }
                case kElemInt16: {
                    return ::arrow::list(::arrow::int16());
                }
                case kElemInt32: {
                    return ::arrow::list(::arrow::int32());
                }
                case kElemInt64: {
                    return ::arrow::list(::arrow::int64());
                }
                case kElemFloat: {
                    return ::arrow::list(::arrow::float32());
                }
                case kElemDouble: {
                    return ::arrow::list(::arrow::float64());
                }
                default: {
                    String error_message = "Invalid embedding data type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
        }
        default: {
            String error_message = "Invalid data type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
    return nullptr;
}

SharedPtr<arrow::Array> PhysicalExport::BuildArrowArray(ColumnDef *column_def, const ColumnVector &column_vector) {
    SharedPtr<arrow::ArrayBuilder> array_builder = nullptr;
    auto &column_type = column_def->type();

    switch (column_type->type()) {
        case LogicalType::kBoolean: {
            array_builder = MakeShared<arrow::UInt8Builder>();
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
        case LogicalType::kDate: {
            array_builder = MakeShared<::arrow::Date32Builder>();
            break;
        }
        // case LogicalType::kTime: {
        //     auto time32_type = ::arrow::time32(arrow::TimeUnit::SECOND);
        //     array_builder = MakeShared<::arrow::Time32Builder>(::arrow::Time32Builder(time32_type));
        //     break;
        // }
        // case LogicalType::kDateTime: {
        //     auto timestamp_type = ::arrow::timestamp(arrow::TimeUnit::SECOND);
        //     array_builder = MakeShared<::arrow::TimestampBuilder>(::arrow::TimestampBuilder(timestamp_type));
        //     break;
        // }
        // case LogicalType::kInterval: {
        //     auto duration_type = ::arrow::duration(arrow::TimeUnit::SECOND);
        //     array_builder = MakeShared<::arrow::DurationBuilder>(::arrow::DurationBuilder(duration_type));
        //     break;
        // }
        case LogicalType::kVarchar: {
            array_builder = MakeShared<::arrow::StringBuilder>();
            break;
        }
        case LogicalType::kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
            switch (embedding_info->Type()) {
                case kElemBit: {
                    SharedPtr<arrow::UInt8Builder> uint8_builder = MakeShared<::arrow::UInt8Builder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), uint8_builder);
                    break;
                }
                case kElemInt8: {
                    SharedPtr<arrow::Int8Builder> int8_builder = MakeShared<::arrow::Int8Builder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int8_builder);
                    break;
                }
                case kElemInt16: {
                    SharedPtr<arrow::Int16Builder> int16_builder = MakeShared<::arrow::Int16Builder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int16_builder);
                    break;
                }
                case kElemInt32: {
                    SharedPtr<arrow::Int32Builder> int32_builder = MakeShared<::arrow::Int32Builder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int32_builder);
                    break;
                }
                case kElemInt64: {
                    SharedPtr<arrow::Int64Builder> int64_builder = MakeShared<::arrow::Int64Builder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), int64_builder);
                    break;
                }
                case kElemFloat: {
                    SharedPtr<arrow::FloatBuilder> float_builder = MakeShared<::arrow::FloatBuilder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), float_builder);
                    break;
                }
                case kElemDouble: {
                    SharedPtr<arrow::DoubleBuilder> double_builder = MakeShared<::arrow::DoubleBuilder>();
                    array_builder = MakeShared<::arrow::ListBuilder>(arrow::DefaultMemoryPool(), double_builder);
                    break;
                }
                default: {
                    String error_message = "Invalid embedding data type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
            break;
        }
        case LogicalType::kDecimal:
        default: {
            String error_message = "Invalid data type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    for (SizeT i = 0; i < column_vector.Size(); ++i) {
        auto value = column_vector.GetValue(i);
        value.AppendToArrowArray(column_type, array_builder);
    }

    SharedPtr<arrow::Array> array;
    auto status = array_builder->Finish(&array);
    if (!status.ok()) {
        String error_message = fmt::format("Failed to build arrow array: {}", status.message());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    return array;
}

} // namespace infinity
