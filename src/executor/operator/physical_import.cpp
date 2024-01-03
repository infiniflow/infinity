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

// extern "C" {
// #include "zsv/api.h"
// #include "zsv/common.h"
// }

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <vector>

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import expression_state;
import data_block;
import logger;
import third_party;
import local_file_system;
import defer_op;
import txn_store;
import wal_entry;
import file_system_type;
import file_system;
import buffer_handle;
import catalog;
import infinity_exception;
import zsv;

module physical_import;

namespace infinity {

void PhysicalImport::Init() {}

/**
 * @brief copy statement execute function
 * @param query_context
 * @param input_state
 * @param output_state
 */
bool PhysicalImport::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ImportOperatorState *import_op_state = static_cast<ImportOperatorState *>(operator_state);
    switch (file_type_) {
        case CopyFileType::kCSV: {
            ImportCSV(query_context, import_op_state);
            break;
        }
        case CopyFileType::kJSON: {
            ImportJSON(query_context, import_op_state);
            break;
        }
        case CopyFileType::kJSONL: {
            ImportJSONL(query_context, import_op_state);
            break;
        }
        case CopyFileType::kFVECS: {
            ImportFVECS(query_context, import_op_state);
            break;
        }
    }
    import_op_state->SetComplete();
    Txn *txn = query_context->GetTxn();
    txn->BeginTS();
    return true;
}

void PhysicalImport::ImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state) {
    if (table_entry_->ColumnCount() != 1) {
        Error<ExecutorException>("FVECS file must have only one column.");
    }
    auto &column_type = table_entry_->GetColumnDefByID(0)->column_type_;
    if (column_type->type() != kEmbedding) {
        Error<ExecutorException>("FVECS file must have only one embedding column.");
    }
    auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    if (embedding_info->Type() != kElemFloat) {
        Error<ExecutorException>("FVECS file must have only one embedding column with float element.");
    }

    LocalFileSystem fs;

    UniquePtr<FileHandler> file_handler = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn defer_fn([&]() { fs.Close(*file_handler); });

    int dimension = 0;
    i64 nbytes = fs.Read(*file_handler, &dimension, sizeof(dimension));
    fs.Seek(*file_handler, 0);
    if (nbytes != sizeof(dimension)) {
        Error<ExecutorException>(Format("Read dimension which length isn't {}.", nbytes));
    }
    if ((int)embedding_info->Dimension() != dimension) {
        Error<ExecutorException>(Format("Dimension in file ({}) doesn't match with table definition ({}).", dimension, embedding_info->Dimension()));
    }
    SizeT file_size = fs.GetFileSize(*file_handler);
    SizeT row_size = dimension * sizeof(FloatT) + sizeof(dimension);
    if (file_size % row_size != 0) {
        Error<ExecutorException>("Weird file size.");
    }
    SizeT vector_n = file_size / row_size;

    Txn *txn = query_context->GetTxn();
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);

    u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::MakeNewSegmentEntry(table_entry_, segment_id, query_context->GetTxn()->GetBufferMgr());
    BlockEntry *last_block_entry = segment_entry->GetLastEntry();
    BufferHandle buffer_handle = last_block_entry->GetColumnBlockEntry(0)->buffer()->Load();
    SizeT row_idx = 0;
    auto buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
    while (true) {
        int dim;
        nbytes = fs.Read(*file_handler, &dim, sizeof(dimension));
        if (dim != dimension or nbytes != sizeof(dimension)) {
            Error<ExecutorException>(Format("Dimension in file ({}) doesn't match with table definition ({}).", dim, dimension));
        }
        ptr_t dst_ptr = buf_ptr + last_block_entry->row_count() * sizeof(FloatT) * dimension;
        fs.Read(*file_handler, dst_ptr, sizeof(FloatT) * dimension);
        segment_entry->IncreaseRowCount(1);
        last_block_entry->IncreaseRowCount(1);

        ++row_idx;
        if (row_idx == vector_n) {
            SaveSegmentData(txn_store, segment_entry);
            break;
        }

        if (segment_entry->Room() <= 0) {
            SaveSegmentData(txn_store, segment_entry);

            segment_id = NewCatalog::GetNextSegmentID(table_entry_);
            segment_entry = SegmentEntry::MakeNewSegmentEntry(table_entry_, segment_id, query_context->GetTxn()->GetBufferMgr());

            last_block_entry = segment_entry->GetLastEntry();
            buffer_handle = last_block_entry->GetColumnBlockEntry(0)->buffer()->Load();
        }

        if (last_block_entry->GetAvailableCapacity() <= 0) {
            segment_entry->AppendBlockEntry(MakeUnique<BlockEntry>(segment_entry.get(),
                                                                   segment_entry->block_entries().size(),
                                                                   0,
                                                                   table_entry_->ColumnCount(),
                                                                   txn->GetBufferMgr()));
            last_block_entry = segment_entry->GetLastEntry();
            buffer_handle = last_block_entry->GetColumnBlockEntry(0)->buffer()->Load();
            buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
        }
    }
    auto result_msg = MakeUnique<String>(Format("IMPORT {} Rows", vector_n));
    import_op_state->result_msg_ = Move(result_msg);
}

void PhysicalImport::ImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state) {
    // opts, parser and parser_context points to each other.
    // opt -> parser_context
    // parser->opt
    // parser_context -> parser
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if (!fp) {
        Error<ExecutorException>(strerror(errno));
    }
    Txn *txn = query_context->GetTxn();
    u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::MakeNewSegmentEntry(table_entry_, segment_id, txn->GetBufferMgr());

    auto parser_context = MakeUnique<ZxvParserCtx>(table_entry_, txn, segment_entry, delimiter_);

    auto opts = MakeUnique<ZsvOpts>();
    if (header_) {
        opts->row_handler = CSVHeaderHandler;
    } else {
        opts->row_handler = CSVRowHandler;
    }
    opts->delimiter = delimiter_;
    opts->stream = fp;
    opts->ctx = parser_context.get();
    opts->buffsize = (1 << 20); // default buffer size 256k, we use 1M

    parser_context->parser_ = ZsvParser(opts.get());

    ZsvStatus csv_parser_status;
    while ((csv_parser_status = parser_context->parser_.ParseMore()) == zsv_status_ok) {
        ;
    }
    parser_context->parser_.Finish();

    // add the last segment entry
    if (parser_context->segment_entry_->row_count() > 0) {
        auto txn_store = parser_context->txn_->GetTxnTableStore(table_entry_);
        SaveSegmentData(txn_store, parser_context->segment_entry_);
    }
    fclose(fp);

    if (csv_parser_status != zsv_status_no_more_input) {
        if (parser_context->err_msg_.get() != nullptr) {
            Error<ExecutorException>(*parser_context->err_msg_);
        } else {
            String err_msg = ZsvParser::ParseStatusDesc(csv_parser_status);
            Error<ExecutorException>(err_msg);
        }
    }
    NewCatalog::IncreaseTableRowCount(table_entry_, parser_context->row_count_);

    auto result_msg = MakeUnique<String>(Format("IMPORT {} Rows", parser_context->row_count_));
    import_op_state->result_msg_ = Move(result_msg);
}

void PhysicalImport::ImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state) {
    LocalFileSystem fs;
    UniquePtr<FileHandler> file_handler = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT file_size = fs.GetFileSize(*file_handler);
    String jsonl_str(file_size + 1, 0);
    SizeT read_n = file_handler->Read(jsonl_str.data(), file_size);
    if (read_n != file_size) {
        Error<ExecutorException>(Format("Read file size {} doesn't match with file size {}.", read_n, file_size));
    }

    Txn *txn = query_context->GetTxn();
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);
    u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::MakeNewSegmentEntry(table_entry_, segment_id, txn->GetBufferMgr());

    SizeT start_pos = 0;
    while (true) {
        SizeT end_pos = jsonl_str.find('\n', start_pos);
        if (end_pos == String::npos) {
            end_pos = file_size;
        }
        if (end_pos == start_pos) {
            break;
        }
        StringView json_sv(jsonl_str.data() + start_pos, end_pos - start_pos);
        start_pos = end_pos + 1;
        Json line_json = Json::parse(json_sv);

        if (segment_entry->Room() <= 0) {
            LOG_INFO(Format("Segment {} saved", segment_entry->segment_id()));
            SaveSegmentData(txn_store, segment_entry);
            u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
            segment_entry = SegmentEntry::MakeNewSegmentEntry(table_entry_, segment_id, txn->GetBufferMgr());
        }
        BlockEntry *block_entry = segment_entry->GetLastEntry();
        if (block_entry->GetAvailableCapacity() <= 0) {
            LOG_INFO(Format("Block {} saved", block_entry->block_id()));
            segment_entry->AppendBlockEntry(MakeUnique<BlockEntry>(segment_entry.get(),
                                                                   segment_entry->block_entries().size(),
                                                                   0,
                                                                   table_entry_->ColumnCount(),
                                                                   txn->GetBufferMgr()));
            block_entry = segment_entry->GetLastEntry();
        }

        JSONLRowHandler(line_json, block_entry);
        block_entry->IncreaseRowCount(1);
        segment_entry->IncreaseRowCount(1);
        NewCatalog::IncreaseTableRowCount(table_entry_, 1);
    }
    if (segment_entry->row_count() > 0) {
        SaveSegmentData(txn_store, segment_entry);
    }

    auto result_msg = MakeUnique<String>(Format("IMPORT {} Rows", table_entry_->RowCount()));
    import_op_state->result_msg_ = Move(result_msg);
}

void PhysicalImport::ImportJSON(QueryContext *, ImportOperatorState *) { Error<NotImplementException>("Import JSON is not implemented yet."); }

void PhysicalImport::CSVHeaderHandler(void *context) {
    ZxvParserCtx *parser_context = static_cast<ZxvParserCtx *>(context);
    ZsvParser &parser = parser_context->parser_;
    SizeT csv_column_count = parser.CellCount();

    SizeT table_column_count = parser_context->table_entry_->ColumnCount();
    if (csv_column_count != table_column_count) {
        parser_context->err_msg_ = MakeShared<String>(Format("Unmatched column count ({} != {})", csv_column_count, table_column_count));

        parser.Abort(); // return zsv_status_cancelled
        return;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(parser.GetCellStr(idx));
        const char *table_col_name = parser_context->table_entry_->GetColumnDefByID(idx)->name().c_str();
        if (!strcmp(csv_col_name, table_col_name)) {
            parser_context->err_msg_ = MakeShared<String>(Format("Unmatched column name({} != {})", csv_col_name, table_col_name));

            parser.Abort(); // return zsv_status_cancelled
            return;
        }
    }

    // This is header, doesn't count in row number
    parser.SetRowHandler(CSVRowHandler);
}

namespace {
Vector<StringView> SplitArrayElement(StringView data, char delimiter) {
    SizeT data_size = data.size();
    if (data_size < 2 || data[0] != '[' || data[data_size - 1] != ']') {
        Error<TypeException>("Embedding data must be surrounded by [ and ]");
    }
    Vector<StringView> ret;
    SizeT i = 1, j = 1;
    while (true) {
        if (data[i] == delimiter || data[i] == ' ' || i == data_size - 1) {
            if (i > j) {
                ret.emplace_back(data.begin() + j, data.begin() + i);
            }
            j = i + 1;
        }
        if (i == data_size - 1) {
            break;
        }
        i++;
    }
    return ret;
}

template <typename T>
void AppendSimpleData(BlockColumnEntry *column_data_entry, const StringView &str_view, SizeT dst_offset) {
    T ele = DataType::StringToValue<T>(str_view);
    column_data_entry->BlockColumnEntry::AppendRaw(dst_offset, reinterpret_cast<ptr_t>(&ele), sizeof(T), nullptr);
}

template <typename T>
void AppendEmbeddingData(BlockColumnEntry *column_data_entry, const Vector<StringView> &ele_str_views, SizeT dst_offset) {
    SizeT arr_len = ele_str_views.size();
    auto tmp_buffer = MakeUnique<T[]>(arr_len);
    for (SizeT ele_idx = 0; auto &ele_str_view : ele_str_views) {
        T ele = DataType::StringToValue<T>(ele_str_view);
        tmp_buffer[ele_idx++] = ele;
    }
    column_data_entry->BlockColumnEntry::AppendRaw(dst_offset, reinterpret_cast<ptr_t>(tmp_buffer.get()), sizeof(T) * arr_len, nullptr);
}

void AppendVarcharData(BlockColumnEntry *column_data_entry, const StringView &str_view, SizeT dst_offset) {
    auto varchar_ptr = MakeUnique<VarcharT>();
    varchar_ptr->InitAsValue(str_view.data(), str_view.size());
    // TODO shenyushi: unnecessary copy here.
    column_data_entry->BlockColumnEntry::AppendRaw(dst_offset, reinterpret_cast<ptr_t>(varchar_ptr.get()), sizeof(VarcharT), nullptr);
}

} // namespace

void PhysicalImport::CSVRowHandler(void *context) {
    ZxvParserCtx *parser_context = static_cast<ZxvParserCtx *>(context);
    auto *table_entry = parser_context->table_entry_;
    SizeT column_count = parser_context->parser_.CellCount();
    auto *txn = parser_context->txn_;
    auto txn_store = txn->GetTxnTableStore(table_entry);

    auto segment_entry = parser_context->segment_entry_;
    // we have already used all space of the segment
    if (segment_entry->Room() <= 0) {
        SaveSegmentData(txn_store, segment_entry);
        u64 segment_id = NewCatalog::GetNextSegmentID(parser_context->table_entry_);
        parser_context->segment_entry_ = SegmentEntry::MakeNewSegmentEntry(table_entry, segment_id, txn->GetBufferMgr());
        segment_entry = parser_context->segment_entry_;
    }

    BlockEntry *last_block_entry = segment_entry->GetLastEntry();
    if (last_block_entry->GetAvailableCapacity() <= 0) {
        segment_entry->AppendBlockEntry(
            MakeUnique<BlockEntry>(segment_entry.get(), segment_entry->block_entries().size(), 0, table_entry->ColumnCount(), txn->GetBufferMgr()));
        last_block_entry = segment_entry->GetLastEntry();
    }

    // if column count is larger than columns defined from schema, extra columns are abandoned
    if (column_count != table_entry->ColumnCount()) {
        UniquePtr<String> err_msg =
            MakeUnique<String>(Format("CSV file row count isn't match with table schema, row id: {}.", parser_context->row_count_));
        LOG_ERROR(*err_msg);
        Error<StorageException>(*err_msg);
    }
    // append data to segment entry
    SizeT write_row = last_block_entry->row_count();
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        ZsvCell cell = parser_context->parser_.GetCell(column_idx);
        StringView str_view{};
        if (cell.len) {
            str_view = StringView((char *)cell.str, cell.len);
        }
        BlockColumnEntry *block_column_entry = last_block_entry->GetColumnBlockEntry(column_idx);
        auto column_type = block_column_entry->column_type().get();
        // FIXME: Variable length types cannot use type inference addresses
        SizeT dst_offset = write_row * column_type->Size();

        if (column_type->type() == kVarchar) {
            AppendVarcharData(block_column_entry, str_view, dst_offset);
        } else if (column_type->type() == kEmbedding) {
            Vector<StringView> res;
            auto ele_str_views = SplitArrayElement(str_view, parser_context->delimiter_);
            auto embedding_info = dynamic_cast<EmbeddingInfo *>(column_type->type_info().get());
            if (embedding_info->Dimension() < ele_str_views.size()) {
                Error<ExecutorException>("Embedding data size exceeds dimension.");
            }

            switch (embedding_info->Type()) {
                case kElemBit: {
                    Error<ExecutorException>("Embedding bit type is not implemented.");
                }
                case kElemInt8: {
                    AppendEmbeddingData<TinyIntT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemInt16: {
                    AppendEmbeddingData<SmallIntT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemInt32: {
                    AppendEmbeddingData<IntegerT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemInt64: {
                    AppendEmbeddingData<BigIntT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemFloat: {
                    AppendEmbeddingData<FloatT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemDouble: {
                    AppendEmbeddingData<DoubleT>(block_column_entry, ele_str_views, dst_offset);
                    break;
                }
                case kElemInvalid: {
                    Error<ExecutorException>("Embedding element type is invalid.");
                }
            }
        } else {
            switch (column_type->type()) {
                case kBoolean: {
                    AppendSimpleData<BooleanT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kTinyInt: {
                    AppendSimpleData<TinyIntT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kSmallInt: {
                    AppendSimpleData<SmallIntT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kInteger: {
                    AppendSimpleData<IntegerT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kBigInt: {
                    AppendSimpleData<BigIntT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kFloat: {
                    AppendSimpleData<FloatT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kDouble: {
                    AppendSimpleData<DoubleT>(block_column_entry, str_view, dst_offset);
                    break;
                }
                case kMissing:
                case kInvalid: {
                    Error<ExecutorException>("Invalid data type");
                }
                default: {
                    Error<ExecutorException>("Not supported now in append data in column");
                }
            }
        }
    }
    last_block_entry->IncreaseRowCount(1);
    segment_entry->IncreaseRowCount(1);
    ++parser_context->row_count_;
}

template <typename T>
void AppendEmbeddingJsonl(BlockColumnEntry *block_column_entry, const Vector<T> &embedding, SizeT dst_offset, SizeT dim) {
    if (embedding.size() != dim) {
        Error<ExecutorException>("Embedding data size neq dimension.");
    }
    block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(embedding.data()), embedding.size() * sizeof(T), nullptr);
}

void PhysicalImport::JSONLRowHandler(const Json &line_json, BlockEntry *block_entry) {
    SizeT column_n = table_entry_->ColumnCount();
    SizeT row_cnt = block_entry->row_count();
    for (SizeT i = 0; i < column_n; ++i) {
        const ColumnDef *column_def = table_entry_->GetColumnDefByID(i);
        auto block_column_entry = block_entry->GetColumnBlockEntry(i);

        auto column_type = block_column_entry->column_type().get();
        SizeT dst_offset = row_cnt * column_type->Size();
        switch (column_type->type()) {
            case LogicalType::kVarchar: {
                AppendVarcharData(block_column_entry, line_json[column_def->name_].get<StringView>(), dst_offset);
                break;
            }
            case LogicalType::kEmbedding: {
                auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
                SizeT dim = embedding_info->Dimension();
                switch (embedding_info->Type()) {
                    case kElemInt8: {
                        AppendEmbeddingJsonl<i8>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    case kElemInt16: {
                        AppendEmbeddingJsonl<i16>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    case kElemInt32: {
                        AppendEmbeddingJsonl<i32>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    case kElemInt64: {
                        AppendEmbeddingJsonl<i64>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    case kElemFloat: {
                        AppendEmbeddingJsonl<float>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    case kElemDouble: {
                        AppendEmbeddingJsonl<double>(block_column_entry, line_json[column_def->name_], dst_offset, dim);
                        break;
                    }
                    default: {
                        Error<NotImplementException>("Embedding type not implemented.");
                    }
                }
                break;
            }
            case LogicalType::kBoolean: {
                bool v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(bool), nullptr);
                break;
            }
            case LogicalType::kTinyInt: {
                i8 v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(i8), nullptr);
                break;
            }
            case LogicalType::kSmallInt: {
                i16 v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(i16), nullptr);
                break;
            }
            case LogicalType::kInteger: {
                i32 v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(i32), nullptr);
                break;
            }
            case LogicalType::kBigInt: {
                i64 v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(i64), nullptr);
                break;
            }
            case LogicalType::kFloat: {
                float v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(float), nullptr);
                break;
            }
            case LogicalType::kDouble: {
                double v = line_json[column_def->name_];
                block_column_entry->AppendRaw(dst_offset, reinterpret_cast<const_ptr_t>(&v), sizeof(double), nullptr);
                break;
            }
            default: {
                Error<NotImplementException>("Type not implemented.");
            }
        }
    }
}

void PhysicalImport::SaveSegmentData(TxnTableStore *txn_store, SharedPtr<SegmentEntry> &segment_entry) {
    Vector<u16> block_row_counts;

    const auto &block_entries = segment_entry->block_entries();
    block_row_counts.reserve(block_entries.size());
    for (auto &block_entry : block_entries) {
        block_entry->FlushData(block_entry->row_count());
        auto size = Max(block_entries.size(), static_cast<SizeT>(block_entry->block_id() + 1));
        block_row_counts.resize(size);
        block_row_counts[block_entry->block_id()] = block_entry->row_count();
    }

    LOG_TRACE(Format("Block rows count {}", block_row_counts.size()));
    for (SizeT i = 0; i < block_row_counts.size(); ++i) {
        LOG_TRACE(Format("Block {} row count {}", i, block_row_counts[i]));
    }

    const String &db_name = *txn_store->table_entry_->GetDBName();
    const String &table_name = *txn_store->table_entry_->GetTableName();
    txn_store->txn_->AddWalCmd(MakeShared<WalCmdImport>(db_name,
                                                        table_name,
                                                        *segment_entry->segment_dir(),
                                                        segment_entry->segment_id(),
                                                        block_entries.size(),
                                                        block_row_counts));

    txn_store->Import(segment_entry);
}

} // namespace infinity
