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

module physical_import;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import expression_state;
import data_block;
import logger;
import third_party;
import local_file_system;
import defer_op;
import txn_store;

import file_system_type;
import file_system;
import buffer_handle;

import infinity_exception;
import zsv;
import status;
import column_vector;
import default_values;
import embedding_info;
import column_def;
import wal_entry;

import catalog;

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
        RecoverableError(Status::ImportFileFormatError("FVECS file must have only one column."));
    }
    auto &column_type = table_entry_->GetColumnDefByID(0)->column_type_;
    if (column_type->type() != kEmbedding) {
        RecoverableError(Status::ImportFileFormatError("FVECS file must have only one embedding column."));
    }
    auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    if (embedding_info->Type() != kElemFloat) {
        RecoverableError(Status::ImportFileFormatError("FVECS file must have only one embedding column with float element."));
    }

    LocalFileSystem fs;

    UniquePtr<FileHandler> file_handler = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn defer_fn([&]() { fs.Close(*file_handler); });

    int dimension = 0;
    i64 nbytes = fs.Read(*file_handler, &dimension, sizeof(dimension));
    fs.Seek(*file_handler, 0);
    if (nbytes == 0) {
        // file is empty
        auto result_msg = MakeUnique<String>("IMPORT 0 Rows");
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }
    
    if (nbytes != sizeof(dimension)) {
        RecoverableError(Status::ImportFileFormatError(fmt::format("Read dimension which length isn't {}.", nbytes)));
    }
    if ((int)embedding_info->Dimension() != dimension) {
        RecoverableError(Status::ImportFileFormatError(
            fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dimension, embedding_info->Dimension())));
    }
    SizeT file_size = fs.GetFileSize(*file_handler);
    SizeT row_size = dimension * sizeof(FloatT) + sizeof(dimension);
    if (file_size % row_size != 0) {
        UnrecoverableError("Weird file size.");
    }
    SizeT vector_n = file_size / row_size;

    Txn *txn = query_context->GetTxn();
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);

    SegmentID segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, query_context->GetTxn(), true);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);
    BufferHandle buffer_handle = block_entry->GetColumnBlockEntry(0)->buffer()->Load();
    SizeT row_idx = 0;
    auto buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
    while (true) {
        int dim;
        nbytes = fs.Read(*file_handler, &dim, sizeof(dimension));
        if (dim != dimension or nbytes != sizeof(dimension)) {
            RecoverableError(
                Status::ImportFileFormatError(fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dim, dimension)));
        }
        ptr_t dst_ptr = buf_ptr + block_entry->row_count() * sizeof(FloatT) * dimension;
        fs.Read(*file_handler, dst_ptr, sizeof(FloatT) * dimension);
        block_entry->IncreaseRowCount(1);
        ++row_idx;

        if (row_idx == vector_n) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            SaveSegmentData(txn_store, segment_entry);
            break;
        }

        if (block_entry->GetAvailableCapacity() <= 0) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                SaveSegmentData(txn_store, segment_entry);

                segment_id = NewCatalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, query_context->GetTxn(), true);
            }

            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            buffer_handle = block_entry->GetColumnBlockEntry(0)->buffer()->Load();
            buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
        }
    }
    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", vector_n));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::ImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state) {
    // opts, parser and parser_context points to each other.
    // opt -> parser_context
    // parser->opt
    // parser_context -> parser
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if (!fp) {
        UnrecoverableError(strerror(errno));
    }

    UniquePtr<ZxvParserCtx> parser_context = nullptr;
    {
        Txn *txn = query_context->GetTxn();
        auto *buffer_mgr = txn->GetBufferMgr();
        u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
        SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn, true);
        UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);
        Vector<ColumnVector> column_vectors;
        int column_count = table_entry_->ColumnCount();
        for (int i = 0; i < column_count; ++i) {
            auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
            column_vectors.emplace_back(block_column_entry->GetColumnVector(buffer_mgr));
        }
        parser_context = MakeUnique<ZxvParserCtx>(table_entry_, txn, segment_entry, std::move(block_entry), std::move(column_vectors), delimiter_);
    }

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

    { // add the last segment entry
        auto *txn_store = parser_context->txn_->GetTxnTableStore(table_entry_);
        auto segment_entry = parser_context->segment_entry_;
        auto &block_entry = parser_context->block_entry_;
        if (block_entry->row_count() > 0) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            SaveSegmentData(txn_store, segment_entry);
        }
    }
    fclose(fp);

    if (csv_parser_status != zsv_status_no_more_input) {
        if (parser_context->err_msg_.get() != nullptr) {
            UnrecoverableError(*parser_context->err_msg_);
        } else {
            String err_msg = ZsvParser::ParseStatusDesc(csv_parser_status);
            UnrecoverableError(err_msg);
        }
    }

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", parser_context->row_count_));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::ImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state) {
    LocalFileSystem fs;
    UniquePtr<FileHandler> file_handler = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT file_size = fs.GetFileSize(*file_handler);
    String jsonl_str(file_size + 1, 0);
    SizeT read_n = file_handler->Read(jsonl_str.data(), file_size);
    if (read_n != file_size) {
        UnrecoverableError(fmt::format("Read file size {} doesn't match with file size {}.", read_n, file_size));
    }

    Txn *txn = query_context->GetTxn();
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_entry_);
    u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn, true);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);

    SizeT start_pos = 0;
    Vector<ColumnVector> column_vectors;
    for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->GetBufferMgr()));
    }
    while (true) {
        SizeT end_pos = jsonl_str.find('\n', start_pos);
        if (end_pos == String::npos) {
            end_pos = file_size;
        }
        std::string_view json_sv(jsonl_str.data() + start_pos, end_pos - start_pos);
        if (end_pos == file_size) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            SaveSegmentData(txn_store, segment_entry);
            break;
        }
        start_pos = end_pos + 1;

        nlohmann::json line_json = nlohmann::json::parse(json_sv);

        JSONLRowHandler(line_json, column_vectors);
        block_entry->IncreaseRowCount(1);

        if (block_entry->GetAvailableCapacity() <= 0) {
            LOG_INFO(fmt::format("Block {} saved", block_entry->block_id()));
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                LOG_INFO(fmt::format("Segment {} saved", segment_entry->segment_id()));
                SaveSegmentData(txn_store, segment_entry);
                u64 segment_id = NewCatalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn, true);
            }

            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            column_vectors.clear();
            for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
                auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->GetBufferMgr()));
            }
        }
    }

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", table_entry_->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::ImportJSON(QueryContext *, ImportOperatorState *) {
    RecoverableError(Status::NotSupport("Import JSON is not implemented yet."));
}

void PhysicalImport::CSVHeaderHandler(void *context) {
    ZxvParserCtx *parser_context = static_cast<ZxvParserCtx *>(context);
    ZsvParser &parser = parser_context->parser_;
    SizeT csv_column_count = parser.CellCount();

    SizeT table_column_count = parser_context->table_entry_->ColumnCount();
    if (csv_column_count != table_column_count) {
        parser_context->err_msg_ = MakeShared<String>(fmt::format("Unmatched column count ({} != {})", csv_column_count, table_column_count));

        parser.Abort(); // return zsv_status_cancelled
        return;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(parser.GetCellStr(idx));
        const char *table_col_name = parser_context->table_entry_->GetColumnDefByID(idx)->name().c_str();
        if (!strcmp(csv_col_name, table_col_name)) {
            parser_context->err_msg_ = MakeShared<String>(fmt::format("Unmatched column name({} != {})", csv_col_name, table_col_name));

            parser.Abort(); // return zsv_status_cancelled
            return;
        }
    }

    // This is header, doesn't count in row number
    parser.SetRowHandler(CSVRowHandler);
}

void PhysicalImport::CSVRowHandler(void *context) {
    ZxvParserCtx *parser_context = static_cast<ZxvParserCtx *>(context);

    auto *table_entry = parser_context->table_entry_;
    SizeT column_count = parser_context->parser_.CellCount();

    auto *txn = parser_context->txn_;
    auto txn_store = txn->GetTxnTableStore(table_entry);
    auto *buffer_mgr = txn->GetBufferMgr();

    auto segment_entry = parser_context->segment_entry_;
    UniquePtr<BlockEntry> block_entry = std::move(parser_context->block_entry_);

    // if column count is larger than columns defined from schema, extra columns are abandoned
    if (column_count != table_entry->ColumnCount()) {
        UniquePtr<String> err_msg =
            MakeUnique<String>(fmt::format("CSV file row count isn't match with table schema, row id: {}.", parser_context->row_count_));
        LOG_ERROR(*err_msg);
        RecoverableError(Status::ColumnCountMismatch(*err_msg));
    }

    // append data to segment entry
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        ZsvCell cell = parser_context->parser_.GetCell(column_idx);
        std::string_view str_view{};
        if (cell.len) {
            str_view = std::string_view((char *)cell.str, cell.len);
        }
        auto &column_vector = parser_context->column_vectors_[column_idx];
        column_vector.AppendByStringView(str_view, parser_context->delimiter_);
    }
    block_entry->IncreaseRowCount(1);
    ++parser_context->row_count_;

    if (block_entry->GetAvailableCapacity() <= 0) {
        segment_entry->AppendBlockEntry(std::move(block_entry));
        // we have already used all space of the segment
        if (segment_entry->Room() <= 0) {
            SaveSegmentData(txn_store, segment_entry);
            u64 segment_id = NewCatalog::GetNextSegmentID(parser_context->table_entry_);
            segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn, true);
            parser_context->segment_entry_ = segment_entry;
        }

        block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry->ColumnCount(), txn);
        parser_context->column_vectors_.clear();
        for (SizeT i = 0; i < column_count; ++i) {
            auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
            parser_context->column_vectors_.emplace_back(block_column_entry->GetColumnVector(buffer_mgr));
        }
    }

    // set parser context
    parser_context->block_entry_ = std::move(block_entry);
}

void PhysicalImport::JSONLRowHandler(const nlohmann::json &line_json, Vector<ColumnVector> &column_vectors) {
    for (SizeT i = 0; auto &column_vector : column_vectors) {
        const ColumnDef *column_def = table_entry_->GetColumnDefByID(i++);

        switch (column_vector.data_type()->type()) {
            case kBoolean: {
                bool v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kTinyInt: {
                i8 v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kSmallInt: {
                i16 v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kInteger: {
                i32 v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kBigInt: {
                i64 v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kFloat: {
                float v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kDouble: {
                double v = line_json[column_def->name_];
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                break;
            }
            case kVarchar: {
                std::string_view str_view = line_json[column_def->name_].get<std::string_view>();
                column_vector.AppendByStringView(str_view, ',');
                break;
            }
            case kEmbedding: {
                auto embedding_info = static_cast<EmbeddingInfo *>(column_vector.data_type()->type_info().get());
                // SizeT dim = embedding_info->Dimension();
                switch (embedding_info->Type()) {
                    case kElemInt8: {
                        Vector<i8> &&embedding = line_json[column_def->name_].get<Vector<i8>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    case kElemInt16: {
                        Vector<i16> &&embedding = line_json[column_def->name_].get<Vector<i16>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    case kElemInt32: {
                        Vector<i32> &&embedding = line_json[column_def->name_].get<Vector<i32>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    case kElemInt64: {
                        Vector<i64> &&embedding = line_json[column_def->name_].get<Vector<i64>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    case kElemFloat: {
                        Vector<float> &&embedding = line_json[column_def->name_].get<Vector<float>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    case kElemDouble: {
                        Vector<double> &&embedding = line_json[column_def->name_].get<Vector<double>>();
                        column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                        break;
                    }
                    default: {
                        UnrecoverableError("Not implement: Embedding type.");
                    }
                }
                break;
            }
            default: {
                UnrecoverableError("Not implement: Invalid data type.");
            }
        }
    }
}

void PhysicalImport::SaveSegmentData(TxnTableStore *txn_store, SharedPtr<SegmentEntry> &segment_entry) {
    segment_entry->FlushNewData();
    const auto [block_cnt, last_block_row_count] = segment_entry->GetWalInfo();

    const String &db_name = *txn_store->table_entry_->GetDBName();
    const String &table_name = *txn_store->table_entry_->GetTableName();
    txn_store->txn_->AddWalCmd(MakeShared<WalCmdImport>(db_name,
                                                        table_name,
                                                        WalSegmentInfo{*segment_entry->segment_dir(),
                                                                       segment_entry->segment_id(),
                                                                       static_cast<u16>(block_cnt),
                                                                       DEFAULT_BLOCK_CAPACITY, // TODO: store block capacity in segment_entry
                                                                       last_block_row_count}));

    txn_store->Import(segment_entry);
}

} // namespace infinity
