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
import sparse_info;
import column_def;
import constant_expr;
import wal_entry;
import knn_expr;
import value;
import catalog;
import catalog_delta_entry;
import build_fast_rough_filter_task;

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
        case CopyFileType::kCSR: {
            ImportCSR(query_context, import_op_state);
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }
    import_op_state->SetComplete();
    Txn *txn = query_context->GetTxn();
    txn->BeginTS();
    return true;
}

void PhysicalImport::ImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state) {
    if (table_entry_->ColumnCount() != 1) {
        Status status = Status::ImportFileFormatError("FVECS file must have only one column.");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    auto &column_type = table_entry_->GetColumnDefByID(0)->column_type_;
    if (column_type->type() != kEmbedding) {
        Status status = Status::ImportFileFormatError("FVECS file must have only one embedding column.");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    if (embedding_info->Type() != kElemFloat) {
        Status status = Status::ImportFileFormatError("FVECS file must have only one embedding column with float element.");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }

    LocalFileSystem fs;

    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }
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
        Status status = Status::ImportFileFormatError(fmt::format("Read dimension which length isn't {}.", nbytes));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    if ((int)embedding_info->Dimension() != dimension) {
        Status status = Status::ImportFileFormatError(fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dimension, embedding_info->Dimension()));
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    SizeT file_size = fs.GetFileSize(*file_handler);
    SizeT row_size = dimension * sizeof(FloatT) + sizeof(dimension);
    if (file_size % row_size != 0) {
        UnrecoverableError("Weird file size.");
    }
    SizeT vector_n = file_size / row_size;

    Txn *txn = query_context->GetTxn();

    SegmentID segment_id = Catalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);
    BufferHandle buffer_handle = block_entry->GetColumnBlockEntry(0)->buffer()->Load();
    SizeT row_idx = 0;
    auto buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
    while (true) {
        int dim;
        nbytes = fs.Read(*file_handler, &dim, sizeof(dimension));
        if (dim != dimension or nbytes != sizeof(dimension)) {
            Status status = Status::ImportFileFormatError(fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dim, dimension));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        ptr_t dst_ptr = buf_ptr + block_entry->row_count() * sizeof(FloatT) * dimension;
        fs.Read(*file_handler, dst_ptr, sizeof(FloatT) * dimension);
        block_entry->IncreaseRowCount(1);
        ++row_idx;

        if (row_idx == vector_n) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            SaveSegmentData(table_entry_, txn, segment_entry);
            break;
        }

        if (block_entry->GetAvailableCapacity() <= 0) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                SaveSegmentData(table_entry_, txn, segment_entry);

                segment_id = Catalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, query_context->GetTxn());
            }

            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            buffer_handle = block_entry->GetColumnBlockEntry(0)->buffer()->Load();
            buf_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut());
        }
    }
    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", vector_n));
    import_op_state->result_msg_ = std::move(result_msg);
}

template <typename IdxT>
UniquePtr<char[]> ConvertCSRIndice(const i32 *tmp_indice_ptr, SizeT nnz) {
    auto res = MakeUnique<char []>(sizeof(IdxT) * nnz);
    auto *ptr = reinterpret_cast<IdxT *>(res.get());
    for (SizeT i = 0; i < nnz; ++i) {
        if (tmp_indice_ptr[i] < 0 || tmp_indice_ptr[i] > std::numeric_limits<IdxT>::max()) {
            UnrecoverableError(fmt::format("In compactible idx {} in csr file.", tmp_indice_ptr[i]));
        }
        ptr[i] = tmp_indice_ptr[i];
    }
    return res;
}

UniquePtr<char[]> ConvertCSRIndice(UniquePtr<char[]> tmp_indice_ptr, SparseInfo *sparse_info, SizeT nnz) {
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            return ConvertCSRIndice<TinyIntT>(reinterpret_cast<i32 *>(tmp_indice_ptr.get()), nnz);
        }
        case EmbeddingDataType::kElemInt16: {
            return ConvertCSRIndice<SmallIntT>(reinterpret_cast<i32 *>(tmp_indice_ptr.get()), nnz);
        }
        case EmbeddingDataType::kElemInt32: {
            return tmp_indice_ptr;
        }
        case EmbeddingDataType::kElemInt64: {
            return ConvertCSRIndice<BigIntT>(reinterpret_cast<i32 *>(tmp_indice_ptr.get()), nnz);
        }
        default: {
            UnrecoverableError(fmt::format("Unsupported index type {}.", sparse_info->IndexType()));
        }
    }
    return {};
}

void PhysicalImport::ImportCSR(QueryContext *query_context, ImportOperatorState *import_op_state) {
    if (table_entry_->ColumnCount() != 1) {
        Status status = Status::ImportFileFormatError("CSR file must have only one column.");
        RecoverableError(status);
    }
    auto &column_type = table_entry_->GetColumnDefByID(0)->column_type_;
    if (column_type->type() != kSparse) {
        Status status = Status::ImportFileFormatError("CSR file must have only one sparse column.");
        RecoverableError(status);
    }
    auto sparse_info = std::static_pointer_cast<SparseInfo>(column_type->type_info());
    if (sparse_info->DataType() != kElemFloat) {
        Status status = Status::ImportFileFormatError("FVECS file must has only one sparse column with float element");
        RecoverableError(status);
    }

    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    i64 nrow = 0;
    i64 ncol = 0;
    i64 nnz = 0;
    file_handler->Read(&nrow, sizeof(nrow));
    file_handler->Read(&ncol, sizeof(ncol));
    file_handler->Read(&nnz, sizeof(nnz));

    SizeT file_size = fs.GetFileSize(*file_handler);
    if (file_size != 3 * sizeof(i64) + (nrow + 1) * sizeof(i64) + nnz * sizeof(i32) + nnz * sizeof(FloatT)) {
        UnrecoverableError("Invalid CSR file format.");
    }
    i64 prev_off = 0;
    file_handler->Read(&prev_off, sizeof(i64));
    if (prev_off != 0) {
        UnrecoverableError("Invalid CSR file format.");
    }
    auto [idx_reader, idx_status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    if (!idx_status.ok()) {
        UnrecoverableError(idx_status.message());
    }
    fs.Seek(*idx_reader, 3 * sizeof(i64) + (nrow + 1) * sizeof(i64));
    auto [data_reader, data_status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    if (!data_status.ok()) {
        UnrecoverableError(data_status.message());
    }
    fs.Seek(*data_reader, 3 * sizeof(i64) + (nrow + 1) * sizeof(i64) + nnz * sizeof(i32));

    //------------------------------------------------------------------------------------------------------------------------

    Txn *txn = query_context->GetTxn();
    auto *buffer_mgr = txn->buffer_mgr();

    SegmentID segment_id = Catalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);
    auto column_vector = MakeShared<ColumnVector>(block_entry->GetColumnBlockEntry(0)->GetColumnVector(buffer_mgr));

    i64 row_id = 0;
    while (true) {
        i64 off = 0;
        file_handler->Read(&off, sizeof(i64));
        i64 nnz = off - prev_off;
        SizeT data_len = sparse_info->DataSize(nnz);
        SizeT indice_len = sparse_info->IndiceSize(nnz);
        auto tmp_indice_ptr = MakeUnique<char[]>(sizeof(i32) * nnz);
        auto data_ptr = MakeUnique<char[]>(data_len);
        idx_reader->Read(tmp_indice_ptr.get(), sizeof(i32) * nnz);
        data_reader->Read(data_ptr.get(), data_len);
        auto indice_ptr = ConvertCSRIndice(std::move(tmp_indice_ptr), sparse_info.get(), nnz);

        auto value = Value::MakeSparse(nnz, std::move(indice_ptr), indice_len, std::move(data_ptr), data_len, sparse_info);
        column_vector->AppendValue(value);

        block_entry->IncreaseRowCount(1);
        prev_off = off;
        ++row_id;
        if (row_id == nrow) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            SaveSegmentData(table_entry_, txn, segment_entry);
            break;
        }
        if (block_entry->GetAvailableCapacity() <= 0) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                SaveSegmentData(table_entry_, txn, segment_entry);

                segment_id = Catalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, query_context->GetTxn());
            }
            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            column_vector = MakeShared<ColumnVector>(block_entry->GetColumnBlockEntry(0)->GetColumnVector(buffer_mgr));
        }
    }
    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", nrow));
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
    Txn *txn = query_context->GetTxn();
    {
        auto *buffer_mgr = txn->buffer_mgr();
        u64 segment_id = Catalog::GetNextSegmentID(table_entry_);
        SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
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
        auto segment_entry = parser_context->segment_entry_;
        auto &block_entry = parser_context->block_entry_;
        if (block_entry->row_count() > 0) {
            segment_entry->AppendBlockEntry(std::move(block_entry));
        } else {
            parser_context->column_vectors_.clear();
            std::move(*block_entry).Cleanup();
        }
        if (segment_entry->row_count() == 0) {
            parser_context->column_vectors_.clear();
            std::move(*segment_entry).Cleanup();
        } else {
            SaveSegmentData(table_entry_, txn, segment_entry);
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
    auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }
    DeferFn file_defer([&]() { fs.Close(*file_handler); });

    SizeT file_size = fs.GetFileSize(*file_handler);
    String jsonl_str(file_size + 1, 0);
    SizeT read_n = file_handler->Read(jsonl_str.data(), file_size);
    if (read_n != file_size) {
        UnrecoverableError(fmt::format("Read file size {} doesn't match with file size {}.", read_n, file_size));
    }

    if (read_n == 0) {
        auto result_msg = MakeUnique<String>(fmt::format("Empty JSONL file, IMPORT 0 Rows"));
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }

    Txn *txn = query_context->GetTxn();
    u64 segment_id = Catalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);

    SizeT start_pos = 0;
    Vector<ColumnVector> column_vectors;
    for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
    }
    while (true) {
        if (start_pos >= file_size) {
            if (block_entry->row_count() == 0) {
                std::move(*block_entry).Cleanup();
            } else {
                segment_entry->AppendBlockEntry(std::move(block_entry));
            }
            if (segment_entry->row_count() == 0) {
                std::move(*segment_entry).Cleanup();
            } else {
                SaveSegmentData(table_entry_, txn, segment_entry);
            }
            break;
        }
        SizeT end_pos = jsonl_str.find('\n', start_pos);
        if (end_pos == String::npos) {
            end_pos = file_size;
        }
        std::string_view json_sv(jsonl_str.data() + start_pos, end_pos - start_pos);
        start_pos = end_pos + 1;

        nlohmann::json line_json = nlohmann::json::parse(json_sv);

        JSONLRowHandler(line_json, column_vectors);
        block_entry->IncreaseRowCount(1);

        if (block_entry->GetAvailableCapacity() <= 0) {
            LOG_DEBUG(fmt::format("Block {} saved", block_entry->block_id()));
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                LOG_DEBUG(fmt::format("Segment {} saved", segment_entry->segment_id()));
                SaveSegmentData(table_entry_, txn, segment_entry);
                u64 segment_id = Catalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
            }

            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            column_vectors.clear();
            for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
                auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
            }
        }
    }

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", table_entry_->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::ImportJSON(QueryContext *query_context, ImportOperatorState *import_op_state) {
    nlohmann::json json_arr;
    {
        LocalFileSystem fs;
        auto [file_handler, status] = fs.OpenFile(file_path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        DeferFn file_defer([&]() { fs.Close(*file_handler); });

        SizeT file_size = fs.GetFileSize(*file_handler);
        String json_str(file_size, 0);
        SizeT read_n = file_handler->Read(json_str.data(), file_size);
        if (read_n != file_size) {
            UnrecoverableError(fmt::format("Read file size {} doesn't match with file size {}.", read_n, file_size));
        }

        if (read_n == 0) {
            auto result_msg = MakeUnique<String>(fmt::format("Empty JSON file, IMPORT 0 Rows"));
            import_op_state->result_msg_ = std::move(result_msg);
            return;
        }

        json_arr = nlohmann::json::parse(json_str);
    }

    Txn *txn = query_context->GetTxn();
    u64 segment_id = Catalog::GetNextSegmentID(table_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
    UniquePtr<BlockEntry> block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), 0, 0, table_entry_->ColumnCount(), txn);

    Vector<ColumnVector> column_vectors;
    for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
        auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
        column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
    }

    if (!json_arr.is_array()) {
        auto result_msg = MakeUnique<String>(fmt::format("Invalid json format, IMPORT 0 rows"));
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }

    for (const auto &json_entry : json_arr) {
        if (block_entry->GetAvailableCapacity() <= 0) {
            LOG_DEBUG(fmt::format("Block {} saved", block_entry->block_id()));
            segment_entry->AppendBlockEntry(std::move(block_entry));
            if (segment_entry->Room() <= 0) {
                LOG_DEBUG(fmt::format("Segment {} saved", segment_entry->segment_id()));
                SaveSegmentData(table_entry_, txn, segment_entry);
                u64 segment_id = Catalog::GetNextSegmentID(table_entry_);
                segment_entry = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);
            }

            block_entry = BlockEntry::NewBlockEntry(segment_entry.get(), segment_entry->GetNextBlockID(), 0, table_entry_->ColumnCount(), txn);
            column_vectors.clear();
            for (SizeT i = 0; i < table_entry_->ColumnCount(); ++i) {
                auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
            }
        }

        JSONLRowHandler(json_entry, column_vectors);
        block_entry->IncreaseRowCount(1);
    }

    segment_entry->AppendBlockEntry(std::move(block_entry));
    SaveSegmentData(table_entry_, txn, segment_entry);

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", table_entry_->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
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
    auto *buffer_mgr = txn->buffer_mgr();

    auto segment_entry = parser_context->segment_entry_;
    UniquePtr<BlockEntry> block_entry = std::move(parser_context->block_entry_);

    // if column count is larger than columns defined from schema, extra columns are abandoned
    if (column_count > table_entry->ColumnCount()) {
        UniquePtr<String> err_msg = MakeUnique<String>(
            fmt::format("CSV file column count isn't match with table schema, row id: {}, column_count = {}, table_entry->ColumnCount = {}.",
                        parser_context->row_count_,
                        column_count,
                        table_entry->ColumnCount()));
        for (SizeT i = 0; i < column_count; ++i) {
            ZsvCell cell = parser_context->parser_.GetCell(i);
            LOG_ERROR(fmt::format("Column {}: {}", i, std::string_view((char *)cell.str, cell.len)));
        }
        Status status = Status::ColumnCountMismatch(*err_msg);
        LOG_ERROR(status.message());
        RecoverableError(status);
    }

    // append data to segment entry
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        ZsvCell cell = parser_context->parser_.GetCell(column_idx);
        std::string_view str_view{};
        auto column_def = table_entry->GetColumnDefByID(column_idx);
        if (cell.len) {
            str_view = std::string_view((char *)cell.str, cell.len);
            auto &column_vector = parser_context->column_vectors_[column_idx];
            column_vector.AppendByStringView(str_view, parser_context->delimiter_);
        } else {
            if (column_def->has_default_value()) {
                auto const_expr = dynamic_cast<ConstantExpr *>(column_def->default_expr_.get());
                auto &column_vector = parser_context->column_vectors_[column_idx];
                column_vector.AppendByConstantExpr(const_expr);
            } else {
                Status status = Status::ImportFileFormatError(fmt::format("Column {} is empty.", column_def->name_));
                LOG_ERROR(status.message());
                RecoverableError(status);
            }
        }
    }
    for (SizeT column_idx = column_count; column_idx < table_entry->ColumnCount(); ++column_idx) {
        auto column_def = table_entry->GetColumnDefByID(column_idx);
        auto &column_vector = parser_context->column_vectors_[column_idx];
        if (column_def->has_default_value()) {
            auto const_expr = dynamic_cast<ConstantExpr *>(column_def->default_expr_.get());
            column_vector.AppendByConstantExpr(const_expr);
        } else {
            Status status = Status::ImportFileFormatError(fmt::format("Column {} is empty.", column_def->name_));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
    block_entry->IncreaseRowCount(1);
    ++parser_context->row_count_;

    if (block_entry->GetAvailableCapacity() <= 0) {
        LOG_DEBUG(fmt::format("Block {} saved", block_entry->block_id()));
        segment_entry->AppendBlockEntry(std::move(block_entry));
        // we have already used all space of the segment
        if (segment_entry->Room() <= 0) {
            LOG_DEBUG(fmt::format("Segment {} saved", segment_entry->segment_id()));
            SaveSegmentData(table_entry, txn, segment_entry);
            u64 segment_id = Catalog::GetNextSegmentID(parser_context->table_entry_);
            segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
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

SharedPtr<ConstantExpr> BuildConstantExprFromJson(const nlohmann::json &json_object) {
    switch (json_object.type()) {
        case nlohmann::json::value_t::boolean: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kBoolean);
            res->bool_value_ = json_object.get<bool>();
            return res;
        }
        case nlohmann::json::value_t::number_unsigned:
        case nlohmann::json::value_t::number_integer: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kInteger);
            res->integer_value_ = json_object.get<i64>();
            return res;
        }
        case nlohmann::json::value_t::number_float: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kDouble);
            res->double_value_ = json_object.get<double>();
            return res;
        }
        case nlohmann::json::value_t::string: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kString);
            auto str = json_object.get<String>();
            res->str_value_ = strdup(json_object.get<String>().c_str());
            return res;
        }
        case nlohmann::json::value_t::array: {
            const u32 array_size = json_object.size();
            if (array_size == 0) {
                const auto error_info = "Empty json array!";
                LOG_ERROR(error_info);
                RecoverableError(Status::ImportFileFormatError(error_info));
                return nullptr;
            }
            switch (json_object[0].type()) {
                case nlohmann::json::value_t::boolean:
                case nlohmann::json::value_t::number_unsigned:
                case nlohmann::json::value_t::number_integer: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kIntegerArray);
                    res->long_array_.resize(array_size);
                    for (u32 i = 0; i < array_size; ++i) {
                        res->long_array_[i] = json_object[i].get<i64>();
                    }
                    return res;
                }
                case nlohmann::json::value_t::number_float: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kDoubleArray);
                    res->double_array_.resize(array_size);
                    for (u32 i = 0; i < array_size; ++i) {
                        res->double_array_[i] = json_object[i].get<double>();
                    }
                    return res;
                }
                case nlohmann::json::value_t::array: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kSubArrayArray);
                    res->sub_array_array_.resize(array_size);
                    for (u32 i = 0; i < array_size; ++i) {
                        res->sub_array_array_[i] = BuildConstantExprFromJson(json_object[i]);
                    }
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in array: {}", json_object.type_name());
                    LOG_ERROR(error_info);
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
            }
        }
        case nlohmann::json::value_t::object: {
            SharedPtr<ConstantExpr> res = nullptr;
            HashSet<i64> key_set;
            for (auto iter = json_object.begin(); iter != json_object.end(); ++iter) {
                i64 key = std::stoll(iter.key());
                const auto &value_obj = iter.value();
                switch(value_obj.type()) {
                    case nlohmann::json::value_t::number_unsigned:
                    case nlohmann::json::value_t::number_integer: {
                        if (res.get() == nullptr) {
                            res = MakeShared<ConstantExpr>(LiteralType::kLongSparseArray);
                        } else if (res->literal_type_ != LiteralType::kLongSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                        res->long_sparse_array_.first.push_back(key);
                        res->long_sparse_array_.second.push_back(value_obj.get<i64>());
                        break;
                    }
                    case nlohmann::json::value_t::number_float: {
                        if (res.get() == nullptr) {
                            res = MakeShared<ConstantExpr>(LiteralType::kDoubleSparseArray);
                        } else if (res->literal_type_ != LiteralType::kDoubleSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                        auto [_, insert_ok] = key_set.insert(key);
                        if (!insert_ok) {
                            const auto error_info = fmt::format("Duplicate key {} in sparse array!", key);
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                        res->double_sparse_array_.first.push_back(key);
                        res->double_sparse_array_.second.push_back(value_obj.get<double>());
                        break;
                    }
                    default: {
                        const auto error_info = fmt::format("Unrecognized json object type in array: {}", json_object.type_name());
                        RecoverableError(Status::ImportFileFormatError(error_info));
                        return nullptr;
                    }
                }
            }
            return res;
        }
        default: {
            const auto error_info = fmt::format("Unrecognized json object type: {}", json_object.type_name());
            LOG_ERROR(error_info);
            RecoverableError(Status::ImportFileFormatError(error_info));
            return nullptr;
        }
    }
}

void PhysicalImport::JSONLRowHandler(const nlohmann::json &line_json, Vector<ColumnVector> &column_vectors) {
    for (SizeT i = 0; auto &column_vector : column_vectors) {
        const ColumnDef *column_def = table_entry_->GetColumnDefByID(i++);

        if (line_json.contains(column_def->name_)) {
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
                case kTensor:
                case kTensorArray:
                case kSparse: {
                    // build ConstantExpr
                    SharedPtr<ConstantExpr> const_expr = BuildConstantExprFromJson(line_json[column_def->name_]);
                    column_vector.AppendByConstantExpr(const_expr.get());
                    break;
                }
                default: {
                    UnrecoverableError("Not implement: Invalid data type.");
                }
            }
        } else if (column_def->has_default_value()) {
            auto const_expr = dynamic_cast<ConstantExpr *>(column_def->default_expr_.get());
            column_vector.AppendByConstantExpr(const_expr);
        } else {
            Status status = Status::ImportFileFormatError(fmt::format("Column {} not found in JSON.", column_def->name_));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
}

void PhysicalImport::SaveSegmentData(TableEntry *table_entry, Txn *txn, SharedPtr<SegmentEntry> segment_entry) {
    segment_entry->FlushNewData();
    txn->Import(table_entry, std::move(segment_entry));
}

} // namespace infinity
