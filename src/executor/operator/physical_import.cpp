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

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <vector>

module infinity_core;

import :stl;
import :query_context;
import :table_def;
import :data_table;
import internal_types;
import logical_type;
import :physical_operator_type;
import :operator_state;
import :expression_state;
import :data_block;
import :logger;
import :third_party;
import :defer_op;

import :buffer_handle;
import :data_file_worker;

import :infinity_exception;
import :zsv;
import :status;
import :column_vector;
import :default_values;
import embedding_info;
import sparse_info;
import array_info;
import column_def;
import constant_expr;
import :wal_entry;
import knn_expr;
import :value;
import :build_fast_rough_filter_task;
import :stream_reader;
import parser_assert;
import :virtual_store;
import :local_file_handle;
import :wal_manager;
import :infinity_context;
import statement_common;
import :new_txn;
import :txn_state;

namespace infinity {

class NewImportCtx {
public:
    NewImportCtx(Vector<SharedPtr<ColumnDef>> column_defs, SizeT block_capacity = DEFAULT_BLOCK_CAPACITY)
        : block_capacity_(block_capacity), column_defs_(std::move(column_defs)) {
        column_types_.reserve(column_defs_.size());
        for (const auto &column_def : column_defs_) {
            column_types_.push_back(column_def->type());
        }
    }

    bool CheckInit() const { return cur_block_ != nullptr; }

    void Init() {
        auto data_block = MakeShared<DataBlock>();
        data_block->Init(column_types_, block_capacity_);

        data_blocks_.push_back(data_block);
        cur_block_ = data_block.get();
    }

    void FinalizeBlock() {
        cur_block_->Finalize();
        cur_block_ = nullptr;
        cur_row_count_ = 0;
    }

    Vector<SharedPtr<DataBlock>> Finalize() && {
        if (CheckInit()) {
            FinalizeBlock();
        }
        return std::move(data_blocks_);
    }

    bool CheckFull() const { return cur_row_count_ >= cur_block_->capacity(); }

    void AddRowCnt() {
        ++row_count_;
        ++cur_row_count_;
    }

    ColumnVector &GetColumnVector(SizeT column_idx) {
        assert(column_idx < column_defs_.size());
        return *cur_block_->column_vectors[column_idx];
    }

    Vector<SharedPtr<ColumnVector>> &GetColumnVectors() { return cur_block_->column_vectors; }

    SharedPtr<ColumnDef> GetColumnDef(SizeT column_idx) {
        assert(column_idx < column_defs_.size());
        return column_defs_[column_idx];
    }

    SizeT row_count() const { return row_count_; }
    SizeT column_count() const { return column_defs_.size(); }

    ZsvParser parser_;

private:
    SizeT block_capacity_ = 0;
    SizeT row_count_ = 0;
    Vector<SharedPtr<ColumnDef>> column_defs_;
    Vector<SharedPtr<DataType>> column_types_;

    Vector<SharedPtr<DataBlock>> data_blocks_;

    DataBlock *cur_block_ = nullptr;
    SizeT cur_row_count_ = 0;
};

class NewZxvParserCtx : public NewImportCtx {
public:
    NewZxvParserCtx(Vector<SharedPtr<ColumnDef>> column_defs, SizeT block_capacity = DEFAULT_BLOCK_CAPACITY)
        : NewImportCtx(std::move(column_defs), block_capacity) {}

public:
    ZsvParser parser_;
    SharedPtr<String> err_msg_;
};

void PhysicalImport::Init(QueryContext *query_context) {}

/**
 * @brief copy statement execute function
 * @param query_context
 * @param input_state
 * @param output_state
 */
bool PhysicalImport::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    if (file_type_ == CopyFileType::kInvalid) {
        String extension = Path(file_path_).filename().extension().string();
        if (extension == ".jsonl") {
            file_type_ = CopyFileType::kJSONL;
        } else if (extension == ".json") {
            file_type_ = CopyFileType::kJSON;
        } else if (extension == ".csv") {
            file_type_ = CopyFileType::kCSV;
        } else if (extension == ".parquet") {
            file_type_ = CopyFileType::kPARQUET;
        } else if (extension == ".fvecs") {
            file_type_ = CopyFileType::kFVECS;
        } else if (extension == ".bvecs") {
            file_type_ = CopyFileType::kBVECS;
        } else if (extension == ".csr") {
            file_type_ = CopyFileType::kCSR;
        }
    }

    ImportOperatorState *import_op_state = static_cast<ImportOperatorState *>(operator_state);

    Vector<SharedPtr<DataBlock>> data_blocks;

    switch (file_type_) {
        case CopyFileType::kCSV: {
            NewImportCSV(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kJSON: {
            NewImportJSON(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kJSONL: {
            NewImportJSONL(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kFVECS: {
            NewImportFVECS(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kCSR: {
            NewImportCSR(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kBVECS: {
            NewImportBVECS(query_context, import_op_state, data_blocks);
            break;
        }
        case CopyFileType::kPARQUET: {
            NewImportPARQUET(query_context, import_op_state, data_blocks);
            break;
        }
        default: {
            UnrecoverableError("Unimplemented");
        }
    }

    NewTxn *new_txn = query_context->GetNewTxn();
    new_txn->SetTxnType(TransactionType::kImport);
    Status status = new_txn->Import(*table_info_->db_name_, *table_info_->table_name_, data_blocks);
    if (!status.ok()) {
        import_op_state->status_ = status;
    }

    import_op_state->SetComplete();
    return true;
}

void PhysicalImport::NewImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    NewImportTypeVecs(query_context, import_op_state, EmbeddingDataType::kElemFloat, data_blocks);
}

void PhysicalImport::NewImportBVECS(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    NewImportTypeVecs(query_context, import_op_state, EmbeddingDataType::kElemUInt8, data_blocks);
}

void PhysicalImport::NewImportTypeVecs(QueryContext *query_context,
                                       ImportOperatorState *import_op_state,
                                       EmbeddingDataType embedding_data_type,
                                       Vector<SharedPtr<DataBlock>> &data_blocks) {
    if (table_info_->column_count_ != 1) {
        Status status = Status::ImportFileFormatError("file must have only one column.");
        RecoverableError(status);
    }
    auto &column_type = table_info_->GetColumnDefByIdx(0)->column_type_;
    if (column_type->type() != LogicalType::kEmbedding) {
        Status status = Status::ImportFileFormatError("file must have only one embedding column.");
        RecoverableError(status);
    }
    auto embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    if (embedding_info->Type() != embedding_data_type) {
        Status status = Status::ImportFileFormatError(" file must have only one embedding column with float element.");
        RecoverableError(status);
    }

    auto [file_handle, status_open] = VirtualStore::Open(file_path_, FileAccessMode::kRead);
    if (!status_open.ok()) {
        UnrecoverableError(status_open.message());
    }

    i32 dimension = 0;
    auto [nbytes, status_read] = file_handle->Read(&dimension, sizeof(dimension));
    if (!status_read.ok()) {
        RecoverableError(status_read);
    }
    file_handle->Seek(0);
    if (nbytes == 0) {
        // file is empty
        auto result_msg = MakeUnique<String>("IMPORT 0 Rows");
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }

    if (nbytes != sizeof(dimension)) {
        Status status = Status::ImportFileFormatError(fmt::format("Read dimension which length isn't {}.", nbytes));
        RecoverableError(status);
    }
    if ((int)embedding_info->Dimension() != dimension) {
        Status status = Status::ImportFileFormatError(
            fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dimension, embedding_info->Dimension()));
        RecoverableError(status);
    }
    i64 file_size = file_handle->FileSize();
    if (file_size == -1) {
        UnrecoverableError("Can't get file size");
    }
    SizeT embedding_width = EmbeddingType::embedding_type_width[static_cast<int>(embedding_data_type)];
    SizeT row_size = dimension * embedding_width + sizeof(dimension);
    if (file_size % row_size != 0) {
        String error_message = "Weird file size.";
        UnrecoverableError(error_message);
    }
    SizeT vector_n = file_size / row_size;

    auto import_ctx = MakeUnique<NewImportCtx>(table_info_->column_defs_);
    auto buffer = MakeUnique<f32[]>(dimension);
    while (import_ctx->row_count() < vector_n) {
        i32 dim;
        auto [nbytes, status_read] = file_handle->Read(&dim, sizeof(dimension));
        if (!status_read.ok()) {
            RecoverableError(status_read);
        }
        if (dim != dimension or nbytes != sizeof(dimension)) {
            Status status_error =
                Status::ImportFileFormatError(fmt::format("Dimension in file ({}) doesn't match with table definition ({}).", dim, dimension));
            RecoverableError(status_error);
        }
        file_handle->Read(buffer.get(), embedding_width * dimension);

        if (!import_ctx->CheckInit()) {
            import_ctx->Init();
        }
        ColumnVector &column_vector = import_ctx->GetColumnVector(0);
        column_vector.AppendByPtr(reinterpret_cast<char *>(buffer.get()));

        import_ctx->AddRowCnt();
        if (import_ctx->CheckFull()) {
            import_ctx->FinalizeBlock();
        }
    }

    data_blocks = std::move(*import_ctx).Finalize();

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", vector_n));
    import_op_state->result_msg_ = std::move(result_msg);
}

template <typename IdxT>
UniquePtr<char[]> ConvertCSRIndice(const i32 *tmp_indice_ptr, SizeT nnz) {
    auto res = MakeUnique<char[]>(sizeof(IdxT) * nnz);
    auto *ptr = reinterpret_cast<IdxT *>(res.get());
    for (SizeT i = 0; i < nnz; ++i) {
        if (tmp_indice_ptr[i] < 0 || tmp_indice_ptr[i] > std::numeric_limits<IdxT>::max()) {
            String error_message = fmt::format("In compactible idx {} in csr file.", tmp_indice_ptr[i]);
            UnrecoverableError(error_message);
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
            String error_message = fmt::format("Unsupported index type {}.", EmbeddingT::EmbeddingDataType2String(sparse_info->IndexType()));
            UnrecoverableError(error_message);
        }
    }
    return {};
}

void PhysicalImport::NewImportCSR(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    if (table_info_->column_count_ != 1) {
        Status status = Status::ImportFileFormatError("CSR file must have only one column.");
        RecoverableError(status);
    }
    auto &column_type = table_info_->GetColumnDefByIdx(0)->column_type_;
    if (column_type->type() != LogicalType::kSparse) {
        Status status = Status::ImportFileFormatError("CSR file must have only one sparse column.");
        RecoverableError(status);
    }
    // nocheck sort or not sort
    auto sparse_info = std::static_pointer_cast<SparseInfo>(column_type->type_info());
    if (sparse_info->DataType() != EmbeddingDataType::kElemFloat) {
        Status status = Status::ImportFileFormatError("FVECS file must has only one sparse column with float element");
        RecoverableError(status);
    }

    auto [file_handle, status] = VirtualStore::Open(file_path_, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    i64 nrow = 0;
    i64 ncol = 0;
    i64 nnz = 0;
    file_handle->Read(&nrow, sizeof(nrow));
    file_handle->Read(&ncol, sizeof(ncol));
    file_handle->Read(&nnz, sizeof(nnz));

    i64 file_size = file_handle->FileSize();
    if ((SizeT)file_size != 3 * sizeof(i64) + (nrow + 1) * sizeof(i64) + nnz * sizeof(i32) + nnz * sizeof(FloatT)) {
        String error_message = "Invalid CSR file format.";
        UnrecoverableError(error_message);
    }
    i64 prev_off = 0;
    file_handle->Read(&prev_off, sizeof(i64));
    if (prev_off != 0) {
        String error_message = "Invalid CSR file format.";
        UnrecoverableError(error_message);
    }
    auto [idx_file_handle, idx_status] = VirtualStore::Open(file_path_, FileAccessMode::kRead);
    if (!idx_status.ok()) {
        UnrecoverableError(idx_status.message());
    }

    idx_file_handle->Seek(3 * sizeof(i64) + (nrow + 1) * sizeof(i64));
    auto [data_file_handle, data_status] = VirtualStore::Open(file_path_, FileAccessMode::kRead);
    if (!data_status.ok()) {
        UnrecoverableError(data_status.message());
    }

    data_file_handle->Seek(3 * sizeof(i64) + (nrow + 1) * sizeof(i64) + nnz * sizeof(i32));

    // ------------------------------------------------------------------------------------------------------------------------

    auto import_ctx = MakeUnique<NewImportCtx>(table_info_->column_defs_);

    while (i64(import_ctx->row_count()) < nrow) {
        i64 off = 0;
        file_handle->Read(&off, sizeof(i64));
        i64 nnz = off - prev_off;
        prev_off = off;
        SizeT data_len = sparse_info->DataSize(nnz);
        auto tmp_indice_ptr = MakeUnique<char[]>(sizeof(i32) * nnz);
        auto data_ptr = MakeUnique<char[]>(data_len);
        idx_file_handle->Read(tmp_indice_ptr.get(), sizeof(i32) * nnz);
        data_file_handle->Read(data_ptr.get(), data_len);
        auto indice_ptr = ConvertCSRIndice(std::move(tmp_indice_ptr), sparse_info.get(), nnz);

        auto value = Value::MakeSparse(nnz, std::move(indice_ptr), std::move(data_ptr), sparse_info);

        if (!import_ctx->CheckInit()) {
            import_ctx->Init();
        }

        ColumnVector &column_vector = import_ctx->GetColumnVector(0);
        column_vector.AppendValue(value);

        import_ctx->AddRowCnt();

        if (import_ctx->CheckFull()) {
            import_ctx->FinalizeBlock();
        }
    }

    data_blocks = std::move(*import_ctx).Finalize();

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", import_ctx->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::NewImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if (!fp) {
        UnrecoverableError(strerror(errno));
    }
    DeferFn defer_op([&]() {
        if (fp) {
            fclose(fp);
        }
    });

    auto parser_context = MakeUnique<NewZxvParserCtx>(table_info_->column_defs_);

    auto opts = MakeUnique<ZsvOpts>();
    if (header_) {
        opts->row_handler = NewCSVHeaderHandler;
    } else {
        opts->row_handler = NewCSVRowHandler;
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

    data_blocks = std::move(*parser_context).Finalize();

    if (csv_parser_status != zsv_status_no_more_input) {
        if (parser_context->err_msg_.get() != nullptr) {
            UnrecoverableError(*parser_context->err_msg_);
        } else {
            String err_msg = ZsvParser::ParseStatusDesc(csv_parser_status);
            UnrecoverableError(err_msg);
        }
    }

    import_op_state->result_msg_ = MakeUnique<String>(fmt::format("IMPORT {} Rows", parser_context->row_count()));
}

void PhysicalImport::NewImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    UniquePtr<StreamReader> stream_reader = VirtualStore::OpenStreamReader(file_path_);

    auto import_ctx = MakeUnique<NewImportCtx>(table_info_->column_defs_);

    while (true) {
        String json_str;
        if (!stream_reader->ReadLine(json_str)) {
            break;
        }

        if (!import_ctx->CheckInit()) {
            import_ctx->Init();
        }
        JSONLRowHandler(json_str, import_ctx->GetColumnVectors());
        import_ctx->AddRowCnt();

        if (import_ctx->CheckFull()) {
            import_ctx->FinalizeBlock();
        }
    }

    data_blocks = std::move(*import_ctx).Finalize();

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", import_ctx->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::NewImportJSON(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    auto [file_handle, status] = VirtualStore::Open(file_path_, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    i64 file_size = file_handle->FileSize();
    if (file_size == -1) {
        UnrecoverableError("Can't get file size");
    }
    String json_str(file_size, 0);
    auto [read_n, status_read] = file_handle->Read(json_str.data(), file_size);
    if (!status_read.ok()) {
        UnrecoverableError(status_read.message());
    }
    if ((i64)read_n != file_size) {
        String error_message = fmt::format("Read file size {} doesn't match with file size {}.", read_n, file_size);
        UnrecoverableError(error_message);
    }
    if (read_n == 0) {
        auto result_msg = MakeUnique<String>(fmt::format("Empty JSON file, IMPORT 0 Rows"));
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }

    simdjson::padded_string json_arr(json_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_arr);
    if (doc.type() != simdjson::json_type::array) {
        auto result_msg = MakeUnique<String>(fmt::format("Invalid json format, IMPORT 0 rows"));
        import_op_state->result_msg_ = std::move(result_msg);
        return;
    }

    auto import_ctx = MakeUnique<NewImportCtx>(table_info_->column_defs_);
    for (auto element : doc.get_array()) {
        if (!import_ctx->CheckInit()) {
            import_ctx->Init();
        }
        JSONLRowHandler(element.value().raw_json(), import_ctx->GetColumnVectors());
        import_ctx->AddRowCnt();

        if (import_ctx->CheckFull()) {
            import_ctx->FinalizeBlock();
        }
    }
    data_blocks = std::move(*import_ctx).Finalize();

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", import_ctx->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

void PhysicalImport::NewCSVHeaderHandler(void *context_raw_ptr) {
    auto *parser_context = static_cast<NewZxvParserCtx *>(context_raw_ptr);
    ZsvParser &parser = parser_context->parser_;
    SizeT csv_column_count = parser.CellCount();

    SizeT table_column_count = parser_context->column_count();
    if (csv_column_count != table_column_count) {
        parser_context->err_msg_ = MakeShared<String>(fmt::format("Unmatched column count ({} != {})", csv_column_count, table_column_count));

        parser.Abort(); // return zsv_status_cancelled
        return;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(parser.GetCellStr(idx));
        const char *table_col_name = parser_context->GetColumnDef(idx)->name().c_str();
        if (!strcmp(csv_col_name, table_col_name)) {
            parser_context->err_msg_ = MakeShared<String>(fmt::format("Unmatched column name({} != {})", csv_col_name, table_col_name));

            parser.Abort(); // return zsv_status_cancelled
            return;
        }
    }

    // This is header, doesn't count in row number
    parser.SetRowHandler(NewCSVRowHandler);
}

void PhysicalImport::NewCSVRowHandler(void *context_raw_ptr) {
    auto *parser_context = static_cast<NewZxvParserCtx *>(context_raw_ptr);

    if (!parser_context->CheckInit()) {
        parser_context->Init();
    }

    SizeT column_count = parser_context->parser_.CellCount();
    SizeT table_column_count = parser_context->column_count();
    if (column_count > table_column_count) {
        Status status = Status::ColumnCountMismatch(
            fmt::format("CSV file column count isn't match with table schema, row id: {}, column_count: {}, table_entry->ColumnCount: {}.",
                        parser_context->row_count(),
                        column_count,
                        table_column_count));
        RecoverableError(status);
    }

    Vector<String> parsed_cell;
    parsed_cell.reserve(column_count);

    for (SizeT column_idx = 0; column_idx < table_column_count; ++column_idx) {
        ColumnVector &column_vector = parser_context->GetColumnVector(column_idx);

        if (column_idx < column_count) {
            ZsvCell cell = parser_context->parser_.GetCell(column_idx);
            if (cell.len) {
                std::string_view str_view = std::string_view((char *)cell.str, cell.len);
                column_vector.AppendByStringView(str_view);
                continue;
            }
        }
        SharedPtr<ColumnDef> column_def = parser_context->GetColumnDef(column_idx);
        if (column_def->has_default_value()) {
            auto const_expr = dynamic_cast<ConstantExpr *>(column_def->default_expr_.get());
            column_vector.AppendByConstantExpr(const_expr);
        } else {
            Status status = Status::ImportFileFormatError(
                fmt::format("No value in column {} in CSV of row number: {}", column_def->name_, parser_context->row_count()));
            RecoverableError(status);
        }
    }
    parser_context->AddRowCnt();

    if (parser_context->CheckFull()) {
        parser_context->FinalizeBlock();
    }
}

SharedPtr<ConstantExpr> BuildConstantExprFromJson(std::string_view object_sv) {
    simdjson::padded_string json_str(object_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_str);
    switch (doc.type()) {
        case simdjson::json_type::boolean: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kBoolean);
            res->bool_value_ = doc.get<bool>();
            return res;
        }
        case simdjson::json_type::number: {
            simdjson::number num = doc.get_number();
            switch (num.get_number_type()) {
                case simdjson::number_type::unsigned_integer:
                case simdjson::number_type::signed_integer: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kInteger);
                    res->integer_value_ = (i64)num;
                    return res;
                }
                case simdjson::number_type::floating_point_number: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kDouble);
                    res->double_value_ = (double)num;
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized object number type");
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
            }
        }
        case simdjson::json_type::string: {
            auto res = MakeShared<ConstantExpr>(LiteralType::kString);
            const String str = doc.get<String>();
            res->str_value_ = strdup(str.c_str());
            return res;
        }
        case simdjson::json_type::array: {
            const u32 array_size = doc.count_elements();
            if (array_size == 0) {
                const auto error_info = "Empty json array!";
                RecoverableError(Status::ImportFileFormatError(error_info));
                return nullptr;
            }
            std::vector<std::string_view> json_strs(array_size);
            std::vector<simdjson::value> values(array_size);
            for (size_t index = 0; auto field : doc.get_array()) {
                values[index] = field.value();
                json_strs[index++] = values[index].raw_json();
            }
            switch (values[0].type()) {
                case simdjson::json_type::boolean:
                case simdjson::json_type::number: {
                    std::vector<simdjson::number> nums(array_size);
                    for (size_t index = 0; auto item : values) {
                        nums[index++] = item.get_number();
                    }
                    switch (nums[0].get_number_type()) {
                        case simdjson::number_type::unsigned_integer:
                        case simdjson::number_type::signed_integer: {
                            auto res = MakeShared<ConstantExpr>(LiteralType::kIntegerArray);
                            res->long_array_.resize(array_size);
                            for (u32 i = 0; i < array_size; ++i) {
                                res->long_array_[i] = (i64)nums[i];
                            }
                            return res;
                        }
                        case simdjson::number_type::floating_point_number: {
                            auto res = MakeShared<ConstantExpr>(LiteralType::kDoubleArray);
                            res->double_array_.resize(array_size);
                            for (u32 i = 0; i < array_size; ++i) {
                                res->double_array_[i] = (double)nums[i];
                            }
                            return res;
                        }
                        default: {
                            const auto error_info = fmt::format("Unrecognized object number type");
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                    }
                }
                case simdjson::json_type::array: {
                    auto res = MakeShared<ConstantExpr>(LiteralType::kSubArrayArray);
                    res->sub_array_array_.resize(array_size);
                    for (u32 i = 0; i < array_size; ++i) {
                        res->sub_array_array_[i] = BuildConstantExprFromJson(json_strs[i]);
                    }
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
            }
        }
        case simdjson::json_type::object: {
            const u32 array_size = doc.count_fields();
            if (array_size != 1) {
                const auto error_info = fmt::format("Unrecognized json object size: Expacted 1, but got {}", array_size);
                RecoverableError(Status::ImportFileFormatError(error_info));
                return nullptr;
            }
            for (auto obj : doc.get_object()) {
                String key = String((std::string_view)obj.unescaped_key());
                if (key != "array") {
                    const auto error_info = fmt::format("Unrecognized json key name: Expacted array, but got {}", key);
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
                auto value = obj.value();
                if (value.type() != simdjson::json_type::array) {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
                auto res = MakeShared<ConstantExpr>(LiteralType::kCurlyBracketsArray);
                for (auto elem : value) {
                    auto elem_expr = BuildConstantExprFromJson(elem.raw_json());
                    if (!elem_expr) {
                        RecoverableError(Status::ImportFileFormatError("Failed to build expr for element of array!"));
                        return nullptr;
                    }
                    res->curly_brackets_array_.push_back(std::move(elem_expr));
                }
                return res;
            }
        }
        default: {
            const auto error_info = fmt::format("Unrecognized json object type");
            RecoverableError(Status::ImportFileFormatError(error_info));
            return nullptr;
        }
    }
}

SharedPtr<ConstantExpr> BuildConstantSparseExprFromJson(std::string_view object_sv, const SparseInfo *sparse_info) {
    SharedPtr<ConstantExpr> res = nullptr;
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemBit: {
            res = MakeShared<ConstantExpr>(LiteralType::kIntegerArray);
            break;
        }
        case EmbeddingDataType::kElemUInt8:
        case EmbeddingDataType::kElemInt8:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64: {
            res = MakeShared<ConstantExpr>(LiteralType::kLongSparseArray);
            break;
        }
        case EmbeddingDataType::kElemFloat16:
        case EmbeddingDataType::kElemBFloat16:
        case EmbeddingDataType::kElemFloat:
        case EmbeddingDataType::kElemDouble: {
            res = MakeShared<ConstantExpr>(LiteralType::kDoubleSparseArray);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            const auto error_info = fmt::format("Unsupported sparse data type: {}", EmbeddingT::EmbeddingDataType2String(sparse_info->DataType()));
            RecoverableError(Status::ImportFileFormatError(error_info));
            return nullptr;
        }
    }
    simdjson::padded_string json(object_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json);
    switch (doc.type()) {
        case simdjson::json_type::array: {
            const u32 array_size = doc.count_elements();
            if (array_size == 0) {
                return res;
            }
            std::vector<std::string_view> json_strs(array_size);
            std::vector<simdjson::value> values(array_size);
            for (size_t index = 0; auto field : doc.get_array()) {
                values[index] = field.value();
                json_strs[index++] = values[index].raw_json();
            }
            if (values[0].type() != simdjson::json_type::number) {
                const auto error_info = fmt::format("Unrecognized json object type");
                RecoverableError(Status::ImportFileFormatError(error_info));
                return nullptr;
            }
            std::vector<simdjson::number> nums(array_size);
            for (size_t index = 0; auto item : values) {
                nums[index++] = item.get_number();
            }
            switch (nums[0].get_number_type()) {
                case simdjson::number_type::unsigned_integer:
                case simdjson::number_type::signed_integer: {
                    res->long_array_.resize(array_size);
                    for (u32 i = 0; i < array_size; ++i) {
                        res->long_array_[i] = (i64)nums[i];
                    }
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
            }
        }
        case simdjson::json_type::object: {
            const u32 object_size = doc.count_fields();
            if (object_size == 0) {
                return res;
            }
            HashSet<i64> key_set;
            for (auto field : doc.get_object()) {
                i64 field_key = std::stoll(String((std::string_view)field.unescaped_key()));
                auto field_value = field.value();
                auto [_, insert_ok] = key_set.insert(field_key);
                if (!insert_ok) {
                    const auto error_info = fmt::format("Duplicate key {} in sparse array!", field_key);
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }
                if (field_value.type() != simdjson::json_type::number) {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    RecoverableError(Status::ImportFileFormatError(error_info));
                    return nullptr;
                }

                simdjson::number num = field_value.get_number();
                if (res->literal_type_ == LiteralType::kLongSparseArray) {
                    switch (num.get_number_type()) {
                        case simdjson::number_type::unsigned_integer:
                        case simdjson::number_type::signed_integer: {
                            res->long_sparse_array_.first.push_back(field_key);
                            res->long_sparse_array_.second.push_back((i64)num);
                            break;
                        }
                        default: {
                            const auto error_info = fmt::format("Unrecognized json object type in array");
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                    }
                } else {
                    switch (num.get_number_type()) {
                        case simdjson::number_type::floating_point_number: {
                            res->double_sparse_array_.first.push_back(field_key);
                            res->double_sparse_array_.second.push_back((double)num);
                            break;
                        }
                        default: {
                            const auto error_info = fmt::format("Unrecognized json object type in array");
                            RecoverableError(Status::ImportFileFormatError(error_info));
                            return nullptr;
                        }
                    }
                }
            }
            return res;
        }
        default: {
            const auto error_info = fmt::format("Unrecognized json object type");
            RecoverableError(Status::ImportFileFormatError(error_info));
            return nullptr;
        }
    }
}

void PhysicalImport::JSONLRowHandler(std::string_view line_sv, Vector<SharedPtr<ColumnVector>> &column_vectors) {
    simdjson::padded_string json_str(line_sv);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json_str);
    for (SizeT i = 0; auto &column_vector_ptr : column_vectors) {
        ColumnVector &column_vector = *column_vector_ptr;
        const ColumnDef *column_def = table_info_->GetColumnDefByIdx(i++);

        if (simdjson::value val; doc[column_def->name_].get(val) == simdjson::SUCCESS) {
            switch (column_vector.data_type()->type()) {
                case LogicalType::kBoolean: {
                    bool v = val.get<bool>();
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kTinyInt: {
                    i8 v = val.get<i8>();
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kSmallInt: {
                    i16 v = val.get<i16>();
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kInteger: {
                    i32 v = val.get<i32>();
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kBigInt: {
                    i64 v = val.get<i64>();
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kFloat16: {
                    float v = val.get<float>();
                    Float16T float16_v(v);
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&float16_v));
                    break;
                }
                case LogicalType::kBFloat16: {
                    float v = val.get<float>(v);
                    BFloat16T bfloat16_v(v);
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&bfloat16_v));
                    break;
                }
                case LogicalType::kFloat: {
                    float v = val.get<float>(v);
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kDouble: {
                    double v = val.get<double>(v);
                    column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&v));
                    break;
                }
                case LogicalType::kDate:
                case LogicalType::kTime:
                case LogicalType::kDateTime:
                case LogicalType::kTimestamp:
                case LogicalType::kVarchar: {
                    std::string_view str_view = doc[column_def->name_];
                    column_vector.AppendByStringView(str_view);
                    break;
                }
                case LogicalType::kEmbedding: {
                    auto embedding_info = static_cast<EmbeddingInfo *>(column_vector.data_type()->type_info().get());
                    SizeT dim = embedding_info->Dimension();
                    switch (embedding_info->Type()) {
                        case EmbeddingDataType::kElemBit: {
                            const Vector<i8> i8_embedding = doc[column_def->name_].get<Vector<i8>>();
                            const SizeT embedding_dim = i8_embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            if (embedding_dim == 0 or embedding_dim % 8 != 0) {
                                Status status = Status::InvalidJsonFormat(fmt::format("Invalid bit vector length: {}", embedding_dim));
                                RecoverableError(status);
                            }
                            Vector<u8> bit_vector(embedding_dim / 8);
                            for (SizeT k = 0; k < embedding_dim; ++k) {
                                if (i8_embedding[k]) {
                                    bit_vector[k / 8] |= 1u << (k % 8);
                                }
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(bit_vector.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemUInt8: {
                            const Vector<u8> embedding = doc[column_def->name_].get<Vector<u8>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemInt8: {
                            const Vector<i8> embedding = doc[column_def->name_].get<Vector<i8>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemInt16: {
                            const Vector<i16> embedding = doc[column_def->name_].get<Vector<i16>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemInt32: {
                            const Vector<i32> embedding = doc[column_def->name_].get<Vector<i32>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemInt64: {
                            const Vector<i64> embedding = doc[column_def->name_].get<Vector<i64>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemFloat16: {
                            const Vector<float> f_embedding = doc[column_def->name_].get<Vector<float>>();
                            SizeT embedding_dim = f_embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            Vector<Float16T> embedding(embedding_dim);
                            for (SizeT k = 0; k < embedding_dim; ++k) {
                                embedding[k] = f_embedding[k];
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemBFloat16: {
                            const Vector<float> f_embedding = doc[column_def->name_].get<Vector<float>>();
                            SizeT embedding_dim = f_embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            Vector<BFloat16T> embedding(embedding_dim);
                            for (SizeT k = 0; k < embedding_dim; ++k) {
                                embedding[k] = f_embedding[k];
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemFloat: {
                            const Vector<float> embedding = doc[column_def->name_].get<Vector<float>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemDouble: {
                            const Vector<double> embedding = doc[column_def->name_].get<Vector<double>>();
                            SizeT embedding_dim = embedding.size();
                            if (embedding_dim != dim) {
                                Status status = Status::InvalidJsonFormat(
                                    fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, embedding_dim));
                                RecoverableError(status);
                            }
                            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(embedding.data()));
                            break;
                        }
                        case EmbeddingDataType::kElemInvalid: {
                            String error_message = "Not implement: Embedding type.";
                            UnrecoverableError(error_message);
                            break;
                        }
                    }
                    break;
                }
                case LogicalType::kArray:
                case LogicalType::kMultiVector:
                case LogicalType::kTensor:
                case LogicalType::kTensorArray: {
                    // build ConstantExpr
                    SharedPtr<ConstantExpr> const_expr = BuildConstantExprFromJson(doc[column_def->name_].raw_json());
                    if (const_expr.get() == nullptr) {
                        RecoverableError(Status::ImportFileFormatError("Invalid json object."));
                    }
                    column_vector.AppendByConstantExpr(const_expr.get());
                    break;
                }
                case LogicalType::kSparse: {
                    const auto *sparse_info = static_cast<SparseInfo *>(column_vector.data_type()->type_info().get());
                    SharedPtr<ConstantExpr> const_expr = BuildConstantSparseExprFromJson(doc[column_def->name_].raw_json(), sparse_info);
                    const_expr->TrySortSparseVec(column_def);
                    if (const_expr.get() == nullptr) {
                        RecoverableError(Status::ImportFileFormatError("Invalid json object."));
                    }
                    column_vector.AppendByConstantExpr(const_expr.get());
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
                    String error_message = "Not implement: Invalid data type.";
                    UnrecoverableError(error_message);
                }
            }
        } else if (column_def->has_default_value()) {
            auto const_expr = dynamic_cast<ConstantExpr *>(column_def->default_expr_.get());
            column_vector.AppendByConstantExpr(const_expr);
        } else {
            Status status = Status::ImportFileFormatError(fmt::format("Column {} not found in JSON.", column_def->name_));
            RecoverableError(status);
        }
    }
}

namespace {

Status CheckParquetColumns(TableInfo *table_info, arrow::ParquetFileReader *arrow_reader) {
    SharedPtr<arrow::Schema> schema;
    arrow::Status status = arrow_reader->GetSchema(&schema);
    if (!status.ok()) {
        return Status::ImportFileFormatError(status.ToString());
    }
    const arrow::FieldVector &fields = schema->fields();
    const Vector<SharedPtr<ColumnDef>> &column_defs = table_info->column_defs_;
    if (fields.size() != column_defs.size()) {
        return Status::ColumnCountMismatch(fmt::format("Column count mismatch: {} != {}", fields.size(), column_defs.size()));
    }
    for (SizeT i = 0; i < fields.size(); ++i) {
        const auto &field = fields[i];
        const auto &column_def = column_defs[i];

        if (*column_def->type() != *field->type()) {
            return Status::ImportFileFormatError(
                fmt::format("Column {} mismatch, {} != {}", i, column_def->type()->ToString(), field->type()->ToString()));
        }
    }

    return Status::OK();
}

} // namespace

void PhysicalImport::NewImportPARQUET(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks) {
    arrow::MemoryPool *pool = arrow::DefaultMemoryPool();

    // Configure general Parquet reader settings
    auto reader_properties = arrow::ParquetReaderProperties(pool);
    reader_properties.enable_buffered_stream();

    // Configure Arrow-specific Parquet reader settings
    auto arrow_reader_props = arrow::ParquetArrowReaderProperties();
    arrow_reader_props.set_batch_size(DEFAULT_BLOCK_CAPACITY);

    arrow::ParquetFileReaderBuilder reader_builder;
    if (const auto status = reader_builder.OpenFile(file_path_, /*memory_map=*/true, reader_properties); !status.ok()) {
        RecoverableError(Status::IOError(status.ToString()));
    }
    reader_builder.memory_pool(pool);
    reader_builder.properties(arrow_reader_props);

    auto build_result = reader_builder.Build();
    if (!build_result.ok()) {
        RecoverableError(Status::ImportFileFormatError(build_result.status().ToString()));
    }
    std::unique_ptr<arrow::ParquetFileReader> arrow_reader = build_result.MoveValueUnsafe();

    if (Status status = CheckParquetColumns(table_info_.get(), arrow_reader.get()); !status.ok()) {
        RecoverableError(status);
    }

    std::shared_ptr<arrow::RecordBatchReader> rb_reader;
    if (auto status = arrow_reader->GetRecordBatchReader(&rb_reader); !status.ok()) {
        RecoverableError(Status::ImportFileFormatError(status.ToString()));
    }

    auto import_ctx = MakeUnique<NewImportCtx>(table_info_->column_defs_);

    for (arrow::ArrowResult<std::shared_ptr<arrow::RecordBatch>> maybe_batch : *rb_reader) {
        if (!maybe_batch.ok()) {
            continue;
        }
        auto batch = maybe_batch.MoveValueUnsafe();
        const auto batch_row_count = batch->num_rows();
        const auto batch_col_count = batch->num_columns();

        for (i64 batch_row_id = 0; batch_row_id < batch_row_count; ++batch_row_id) {
            if (!import_ctx->CheckInit()) {
                import_ctx->Init();
            }
            for (int column_idx = 0; column_idx < batch_col_count; ++column_idx) {
                SharedPtr<arrow::Array> column = batch->column(column_idx);
                if (column->length() != batch_row_count) {
                    UnrecoverableError("column length mismatch");
                }
                ParquetValueHandler(column, import_ctx->GetColumnVector(column_idx), batch_row_id);
            }
            import_ctx->AddRowCnt();
            if (import_ctx->CheckFull()) {
                import_ctx->FinalizeBlock();
            }
        }
    }

    data_blocks = std::move(*import_ctx).Finalize();

    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", import_ctx->row_count()));
    import_op_state->result_msg_ = std::move(result_msg);
}

template <typename IndexType, typename IndexArray, typename DataType, typename DataArray>
void ParquetSparseValueHandler(const SparseInfo *sparse_info,
                               SharedPtr<IndexArray> index_array,
                               SharedPtr<DataArray> data_array,
                               ColumnVector &column_vector,
                               i64 start_offset,
                               i64 end_offset) {
    Vector<IndexType> index_vec;
    if constexpr (std::is_same_v<DataType, bool>) {
        for (i64 j = start_offset; j < end_offset; ++j) {
            index_vec.push_back(index_array->Value(j));
        }
        SizeT nnz = index_vec.size();
        column_vector.AppendSparse(nnz, static_cast<DataType *>(nullptr), index_vec.data());
    } else {
        Vector<DataType> data_vec;
        for (i64 j = start_offset; j < end_offset; ++j) {
            index_vec.push_back(index_array->Value(j));
            data_vec.push_back(data_array->Value(j));
        }
        SizeT nnz = index_vec.size();
        column_vector.AppendSparse(nnz, data_vec.data(), index_vec.data());
    }
}

template <typename IndexType, typename IndexArray>
void ParquetSparseValueHandler(const SparseInfo *sparse_info,
                               SharedPtr<IndexArray> index_array,
                               SharedPtr<arrow::ListArray> data_array,
                               ColumnVector &column_vector,
                               i64 start_offset,
                               i64 end_offset) {
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemBit: {
            ParquetSparseValueHandler<IndexType, IndexArray, bool, arrow::BooleanArray>(sparse_info,
                                                                                        index_array,
                                                                                        nullptr,
                                                                                        column_vector,
                                                                                        start_offset,
                                                                                        end_offset);
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            auto uint8_value_array = std::static_pointer_cast<arrow::UInt8Array>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, u8, arrow::UInt8Array>(sparse_info,
                                                                                    index_array,
                                                                                    uint8_value_array,
                                                                                    column_vector,
                                                                                    start_offset,
                                                                                    end_offset);
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            auto int8_value_array = std::static_pointer_cast<arrow::Int8Array>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, i8, arrow::Int8Array>(sparse_info,
                                                                                   index_array,
                                                                                   int8_value_array,
                                                                                   column_vector,
                                                                                   start_offset,
                                                                                   end_offset);
            break;
        }
        case EmbeddingDataType::kElemInt16: {
            auto int16_value_array = std::static_pointer_cast<arrow::Int16Array>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, i16, arrow::Int16Array>(sparse_info,
                                                                                     index_array,
                                                                                     int16_value_array,
                                                                                     column_vector,
                                                                                     start_offset,
                                                                                     end_offset);
            break;
        }
        case EmbeddingDataType::kElemInt32: {
            auto int32_value_array = std::static_pointer_cast<arrow::Int32Array>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, i32, arrow::Int32Array>(sparse_info,
                                                                                     index_array,
                                                                                     int32_value_array,
                                                                                     column_vector,
                                                                                     start_offset,
                                                                                     end_offset);
            break;
        }
        case EmbeddingDataType::kElemInt64: {
            auto int64_value_array = std::static_pointer_cast<arrow::Int64Array>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, i64, arrow::Int64Array>(sparse_info,
                                                                                     index_array,
                                                                                     int64_value_array,
                                                                                     column_vector,
                                                                                     start_offset,
                                                                                     end_offset);
            break;
        }
        case EmbeddingDataType::kElemFloat16: {
            auto float16_value_array = std::static_pointer_cast<arrow::HalfFloatArray>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, Float16T, arrow::HalfFloatArray>(sparse_info,
                                                                                              index_array,
                                                                                              float16_value_array,
                                                                                              column_vector,
                                                                                              start_offset,
                                                                                              end_offset);
            break;
        }
        case EmbeddingDataType::kElemBFloat16: {
            auto float_value_array = std::static_pointer_cast<arrow::FloatArray>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, BFloat16T, arrow::FloatArray>(sparse_info,
                                                                                           index_array,
                                                                                           float_value_array,
                                                                                           column_vector,
                                                                                           start_offset,
                                                                                           end_offset);
            break;
        }
        case EmbeddingDataType::kElemFloat: {
            auto float_value_array = std::static_pointer_cast<arrow::FloatArray>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, float, arrow::FloatArray>(sparse_info,
                                                                                       index_array,
                                                                                       float_value_array,
                                                                                       column_vector,
                                                                                       start_offset,
                                                                                       end_offset);
            break;
        }
        case EmbeddingDataType::kElemDouble: {
            auto double_value_array = std::static_pointer_cast<arrow::DoubleArray>(data_array->values());
            ParquetSparseValueHandler<IndexType, IndexArray, double, arrow::DoubleArray>(sparse_info,
                                                                                         index_array,
                                                                                         double_value_array,
                                                                                         column_vector,
                                                                                         start_offset,
                                                                                         end_offset);
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Invalid sparse data type.");
        }
    }
}

namespace {

template <typename LstArray>
Pair<UniquePtr<char[]>, SizeT> ParquetEmbeddingHandler(SharedPtr<LstArray> list_array, const EmbeddingInfo *embedding_info, u64 value_idx) {
    SizeT dim = embedding_info->Dimension();
    i64 start_offset = list_array->value_offset(value_idx);
    i64 end_offset = list_array->value_offset(value_idx + 1);
    if (end_offset - start_offset != (i64)dim) {
        String error_message = fmt::format("Attempt to import {} dimension embedding into {} dimension column.", dim, end_offset - start_offset);
        LOG_ERROR(error_message);
        UnrecoverableError(error_message);
    }
    switch (embedding_info->Type()) {
        case EmbeddingDataType::kElemBit: {
            if (dim == 0 or dim % 8 != 0) {
                String error_message = fmt::format("Invalid bit embedding dimension: {}", dim);
                LOG_ERROR(error_message);
                UnrecoverableError(error_message);
            }
            const SizeT byte_size = dim / 8;
            auto embedding = MakeUnique<char[]>(byte_size);
            auto bool_array = std::static_pointer_cast<arrow::BooleanArray>(list_array->values());
            auto *raw_u8_ptr = reinterpret_cast<u8 *>(embedding.get());
            for (i64 j = start_offset; j < end_offset; ++j) {
                if (bool_array->Value(j)) {
                    const auto offset = j - start_offset;
                    raw_u8_ptr[offset / 8] |= (1u << (offset % 8));
                }
            }
            return {std::move(embedding), byte_size};
        }
        case EmbeddingDataType::kElemUInt8: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(u8));
            auto uint8_array = std::static_pointer_cast<arrow::UInt8Array>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                const u8 value = uint8_array->Value(j);
                reinterpret_cast<u8 *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(u8)};
        }
        case EmbeddingDataType::kElemInt8: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(i8));
            auto int8_array = std::static_pointer_cast<arrow::Int8Array>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                i8 value = int8_array->Value(j);
                reinterpret_cast<i8 *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(i8)};
        }
        case EmbeddingDataType::kElemInt16: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(i16));
            auto int16_array = std::static_pointer_cast<arrow::Int16Array>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                i16 value = int16_array->Value(j);
                reinterpret_cast<i16 *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(i16)};
        }
        case EmbeddingDataType::kElemInt32: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(i32));
            auto int32_array = std::static_pointer_cast<arrow::Int32Array>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                i32 value = int32_array->Value(j);
                reinterpret_cast<i32 *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(i32)};
        }
        case EmbeddingDataType::kElemInt64: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(i64));
            auto int64_array = std::static_pointer_cast<arrow::Int64Array>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                i64 value = int64_array->Value(j);
                reinterpret_cast<i64 *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(i64)};
        }
        case EmbeddingDataType::kElemFloat16: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(Float16T));
            auto float16_array = std::static_pointer_cast<arrow::HalfFloatArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                const u16 value = float16_array->Value(j);
                reinterpret_cast<Float16T *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(Float16T)};
        }
        case EmbeddingDataType::kElemBFloat16: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(BFloat16T));
            auto float_array = std::static_pointer_cast<arrow::FloatArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                const float value = float_array->Value(j);
                reinterpret_cast<BFloat16T *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(BFloat16T)};
        }
        case EmbeddingDataType::kElemFloat: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(float));
            auto float_array = std::static_pointer_cast<arrow::FloatArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                float value = float_array->Value(j);
                reinterpret_cast<float *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(float)};
        }
        case EmbeddingDataType::kElemDouble: {
            auto embedding = MakeUnique<char[]>(dim * sizeof(double));
            auto double_array = std::static_pointer_cast<arrow::DoubleArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                double value = double_array->Value(j);
                reinterpret_cast<double *>(embedding.get())[j - start_offset] = value;
            }
            return {std::move(embedding), dim * sizeof(double)};
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Invalid embedding data type.");
            return {};
        }
    }
}

Vector<Pair<UniquePtr<char[]>, SizeT>>
ParquetTensorHandler(SharedPtr<arrow::ListArray> list_array, const EmbeddingInfo *embedding_info, u64 value_idx) {
    i64 start_offset = list_array->value_offset(value_idx);
    i64 end_offset = list_array->value_offset(value_idx + 1);

    Vector<Pair<UniquePtr<char[]>, SizeT>> embedding_vec;
    if (auto fixed_tensor_ele_array = std::dynamic_pointer_cast<arrow::FixedSizeListArray>(list_array->values());
        fixed_tensor_ele_array.get() != nullptr) {
        for (i64 j = start_offset; j < end_offset; ++j) {
            auto data = ParquetEmbeddingHandler(fixed_tensor_ele_array, embedding_info, j);
            embedding_vec.push_back(std::move(data));
        }
    } else {
        auto tensor_ele_array = std::static_pointer_cast<arrow::ListArray>(list_array->values());
        for (i64 j = start_offset; j < end_offset; ++j) {
            auto data = ParquetEmbeddingHandler(tensor_ele_array, embedding_info, j);
            embedding_vec.push_back(std::move(data));
        }
    }
    return embedding_vec;
}

} // namespace

Value GetValueFromParquetRecursively(const DataType &data_type, const SharedPtr<arrow::Array> &array, u64 value_idx);

void PhysicalImport::ParquetValueHandler(const SharedPtr<arrow::Array> &array, ColumnVector &column_vector, u64 value_idx) {
    switch (const auto column_data_logical_type = column_vector.data_type()->type(); column_data_logical_type) {
        case LogicalType::kBoolean: {
            auto value = std::static_pointer_cast<arrow::BooleanArray>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kTinyInt: {
            auto value = std::static_pointer_cast<arrow::Int8Array>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kSmallInt: {
            auto value = std::static_pointer_cast<arrow::Int16Array>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kInteger: {
            auto value = std::static_pointer_cast<arrow::Int32Array>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kBigInt: {
            auto value = std::static_pointer_cast<arrow::Int64Array>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kFloat16: {
            auto value = std::static_pointer_cast<arrow::HalfFloatArray>(array)->Value(value_idx);
            const Float16T float16_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&float16_value));
            break;
        }
        case LogicalType::kBFloat16: {
            auto value = std::static_pointer_cast<arrow::FloatArray>(array)->Value(value_idx);
            const BFloat16T bfloat16_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&bfloat16_value));
            break;
        }
        case LogicalType::kFloat: {
            auto value = std::static_pointer_cast<arrow::FloatArray>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kDouble: {
            auto value = std::static_pointer_cast<arrow::DoubleArray>(array)->Value(value_idx);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&value));
            break;
        }
        case LogicalType::kDate: {
            auto value = std::static_pointer_cast<arrow::Date32Array>(array)->Value(value_idx);
            const DateT date_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&date_value));
            break;
        }
        case LogicalType::kTime: {
            auto value = std::static_pointer_cast<arrow::Time32Array>(array)->Value(value_idx);
            const TimeT time_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&time_value));
            break;
        }
        case LogicalType::kDateTime: {
            auto value = std::static_pointer_cast<arrow::TimestampArray>(array)->Value(value_idx);
            const DateTimeT datetime_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&datetime_value));
            break;
        }
        case LogicalType::kTimestamp: {
            auto value = std::static_pointer_cast<arrow::TimestampArray>(array)->Value(value_idx);
            const TimestampT timestamp_value(value);
            column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(&timestamp_value));
            break;
        }
        case LogicalType::kVarchar: {
            String value_str = std::static_pointer_cast<arrow::StringArray>(array)->GetString(value_idx);
            std::string_view value(value_str);
            column_vector.AppendByStringView(value);
            break;
        }
        case LogicalType::kEmbedding: {
            auto embedding_info = static_cast<EmbeddingInfo *>(column_vector.data_type()->type_info().get());
            if (auto fixed_list_array = std::dynamic_pointer_cast<arrow::FixedSizeListArray>(array); fixed_list_array.get() != nullptr) {
                auto [data, size] = ParquetEmbeddingHandler(fixed_list_array, embedding_info, value_idx);
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(data.get()));
            } else {
                auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
                auto [data, size] = ParquetEmbeddingHandler(list_array, embedding_info, value_idx);
                column_vector.AppendByPtr(reinterpret_cast<const_ptr_t>(data.get()));
            }
            break;
        }
        case LogicalType::kSparse: {
            const auto *sparse_info = static_cast<SparseInfo *>(column_vector.data_type()->type_info().get());

            auto struct_array = std::static_pointer_cast<arrow::StructArray>(array);

            auto index_raw_array = struct_array->GetFieldByName("index");
            auto index_array = std::static_pointer_cast<arrow::ListArray>(index_raw_array);
            i64 start_offset = index_array->value_offset(value_idx);
            i64 end_offset = index_array->value_offset(value_idx + 1);

            SharedPtr<arrow::ListArray> data_array;
            auto value_raw_array = struct_array->GetFieldByName("value");
            if (value_raw_array.get() != nullptr) {
                data_array = std::static_pointer_cast<arrow::ListArray>(value_raw_array);

                i64 start_offset1 = index_array->value_offset(value_idx);
                i64 end_offset1 = index_array->value_offset(value_idx + 1);
                if (start_offset != start_offset1 || end_offset != end_offset1) {
                    RecoverableError(Status::ImportFileFormatError("Invalid parquet file format."));
                }
            }

            switch (sparse_info->IndexType()) {
                case EmbeddingDataType::kElemInt8: {
                    auto int8_index_array = std::static_pointer_cast<arrow::Int8Array>(index_array->values());
                    ParquetSparseValueHandler<i8, arrow::Int8Array>(sparse_info,
                                                                    int8_index_array,
                                                                    data_array,
                                                                    column_vector,
                                                                    start_offset,
                                                                    end_offset);
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    auto int16_index_array = std::static_pointer_cast<arrow::Int16Array>(index_array->values());
                    ParquetSparseValueHandler<i16, arrow::Int16Array>(sparse_info,
                                                                      int16_index_array,
                                                                      data_array,
                                                                      column_vector,
                                                                      start_offset,
                                                                      end_offset);
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    auto int32_index_array = std::static_pointer_cast<arrow::Int32Array>(index_array->values());
                    ParquetSparseValueHandler<i32, arrow::Int32Array>(sparse_info,
                                                                      int32_index_array,
                                                                      data_array,
                                                                      column_vector,
                                                                      start_offset,
                                                                      end_offset);
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    auto int64_index_array = std::static_pointer_cast<arrow::Int64Array>(index_array->values());
                    ParquetSparseValueHandler<i64, arrow::Int64Array>(sparse_info,
                                                                      int64_index_array,
                                                                      data_array,
                                                                      column_vector,
                                                                      start_offset,
                                                                      end_offset);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid sparse index type.");
                }
            }
            break;
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            auto embedding_info = std::static_pointer_cast<EmbeddingInfo>(column_vector.data_type()->type_info());
            auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            Vector<Pair<UniquePtr<char[]>, SizeT>> embedding_vec = ParquetTensorHandler(list_array, embedding_info.get(), value_idx);
            Vector<Pair<ptr_t, SizeT>> embedding_data;
            for (const auto &[data_ptr, data_bytes] : embedding_vec) {
                embedding_data.emplace_back(data_ptr.get(), data_bytes);
            }
            if (column_data_logical_type == LogicalType::kMultiVector) {
                column_vector.AppendValue(Value::MakeMultiVector(embedding_data, std::move(embedding_info)));
            } else if (column_data_logical_type == LogicalType::kTensor) {
                column_vector.AppendValue(Value::MakeTensor(embedding_data, std::move(embedding_info)));
            } else {
                UnrecoverableError("Unhandled case.");
            }
            break;
        }
        case LogicalType::kTensorArray: {
            const auto embedding_info = std::static_pointer_cast<EmbeddingInfo>(column_vector.data_type()->type_info());
            auto value = Value::MakeTensorArray(embedding_info);
            auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            i64 start_offset = list_array->value_offset(value_idx);
            i64 end_offset = list_array->value_offset(value_idx + 1);
            auto tensor_array = std::static_pointer_cast<arrow::ListArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                Vector<Pair<UniquePtr<char[]>, SizeT>> embedding_vec = ParquetTensorHandler(tensor_array, embedding_info.get(), j);
                Vector<Pair<ptr_t, SizeT>> embedding_data;
                for (auto &data : embedding_vec) {
                    embedding_data.push_back({data.first.get(), data.second});
                }
                value.AppendToTensorArray(embedding_data);
            }
            column_vector.AppendValue(std::move(value));
            break;
        }
        case LogicalType::kArray: {
            auto v = GetValueFromParquetRecursively(*column_vector.data_type(), array, value_idx);
            column_vector.AppendValue(std::move(v));
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
            String error_message = "Not implement: Invalid data type.";
            UnrecoverableError(error_message);
        }
    }
}

template <typename IndexType, typename IndexArray, typename DataType, typename DataArray>
Value GetSparseValueFromParquet(const SharedPtr<SparseInfo> &sparse_info,
                                const SharedPtr<IndexArray> &index_array,
                                const SharedPtr<DataArray> &data_array,
                                const i64 start_offset,
                                const i64 end_offset) {
    Vector<IndexType> index_vec;
    Vector<DataType> data_vec;
    for (i64 j = start_offset; j < end_offset; ++j) {
        index_vec.push_back(index_array->Value(j));
        if constexpr (!std::is_same_v<DataType, bool>) {
            data_vec.push_back(data_array->Value(j));
        }
    }
    const char *raw_data_ptr = nullptr;
    if constexpr (!std::is_same_v<DataType, bool>) {
        raw_data_ptr = reinterpret_cast<const char *>(data_vec.data());
    }
    return Value::MakeSparse(raw_data_ptr, reinterpret_cast<const char *>(index_vec.data()), index_vec.size(), sparse_info);
}

template <typename IndexType, typename IndexArray>
Value GetSparseValueFromParquet(const SharedPtr<SparseInfo> &sparse_info,
                                const SharedPtr<IndexArray> &index_array,
                                const SharedPtr<arrow::ListArray> &data_array,
                                const i64 start_offset,
                                const i64 end_offset) {
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemBit: {
            return GetSparseValueFromParquet<IndexType, IndexArray, bool, arrow::BooleanArray>(sparse_info,
                                                                                               index_array,
                                                                                               nullptr,
                                                                                               start_offset,
                                                                                               end_offset);
        }
        case EmbeddingDataType::kElemUInt8: {
            const auto uint8_value_array = std::static_pointer_cast<arrow::UInt8Array>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, u8, arrow::UInt8Array>(sparse_info,
                                                                                           index_array,
                                                                                           uint8_value_array,
                                                                                           start_offset,
                                                                                           end_offset);
        }
        case EmbeddingDataType::kElemInt8: {
            const auto int8_value_array = std::static_pointer_cast<arrow::Int8Array>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, i8, arrow::Int8Array>(sparse_info,
                                                                                          index_array,
                                                                                          int8_value_array,
                                                                                          start_offset,
                                                                                          end_offset);
        }
        case EmbeddingDataType::kElemInt16: {
            const auto int16_value_array = std::static_pointer_cast<arrow::Int16Array>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, i16, arrow::Int16Array>(sparse_info,
                                                                                            index_array,
                                                                                            int16_value_array,
                                                                                            start_offset,
                                                                                            end_offset);
        }
        case EmbeddingDataType::kElemInt32: {
            const auto int32_value_array = std::static_pointer_cast<arrow::Int32Array>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, i32, arrow::Int32Array>(sparse_info,
                                                                                            index_array,
                                                                                            int32_value_array,
                                                                                            start_offset,
                                                                                            end_offset);
        }
        case EmbeddingDataType::kElemInt64: {
            const auto int64_value_array = std::static_pointer_cast<arrow::Int64Array>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, i64, arrow::Int64Array>(sparse_info,
                                                                                            index_array,
                                                                                            int64_value_array,
                                                                                            start_offset,
                                                                                            end_offset);
        }
        case EmbeddingDataType::kElemFloat16: {
            const auto float16_value_array = std::static_pointer_cast<arrow::HalfFloatArray>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, Float16T, arrow::HalfFloatArray>(sparse_info,
                                                                                                     index_array,
                                                                                                     float16_value_array,
                                                                                                     start_offset,
                                                                                                     end_offset);
        }
        case EmbeddingDataType::kElemBFloat16: {
            const auto float_value_array = std::static_pointer_cast<arrow::FloatArray>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, BFloat16T, arrow::FloatArray>(sparse_info,
                                                                                                  index_array,
                                                                                                  float_value_array,
                                                                                                  start_offset,
                                                                                                  end_offset);
        }
        case EmbeddingDataType::kElemFloat: {
            const auto float_value_array = std::static_pointer_cast<arrow::FloatArray>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, float, arrow::FloatArray>(sparse_info,
                                                                                              index_array,
                                                                                              float_value_array,
                                                                                              start_offset,
                                                                                              end_offset);
        }
        case EmbeddingDataType::kElemDouble: {
            const auto double_value_array = std::static_pointer_cast<arrow::DoubleArray>(data_array->values());
            return GetSparseValueFromParquet<IndexType, IndexArray, double, arrow::DoubleArray>(sparse_info,
                                                                                                index_array,
                                                                                                double_value_array,
                                                                                                start_offset,
                                                                                                end_offset);
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Invalid sparse data type.");
            return Value::MakeInvalid();
        }
    }
}

Value GetValueFromParquetRecursively(const DataType &data_type, const SharedPtr<arrow::Array> &array, u64 value_idx) {
    switch (const auto data_logical_type = data_type.type(); data_logical_type) {
        case LogicalType::kBoolean: {
            auto value = std::static_pointer_cast<arrow::BooleanArray>(array)->Value(value_idx);
            return Value::MakeBool(value);
        }
        case LogicalType::kTinyInt: {
            auto value = std::static_pointer_cast<arrow::Int8Array>(array)->Value(value_idx);
            return Value::MakeTinyInt(value);
        }
        case LogicalType::kSmallInt: {
            auto value = std::static_pointer_cast<arrow::Int16Array>(array)->Value(value_idx);
            return Value::MakeSmallInt(value);
        }
        case LogicalType::kInteger: {
            auto value = std::static_pointer_cast<arrow::Int32Array>(array)->Value(value_idx);
            return Value::MakeInt(value);
        }
        case LogicalType::kBigInt: {
            auto value = std::static_pointer_cast<arrow::Int64Array>(array)->Value(value_idx);
            return Value::MakeBigInt(value);
        }
        case LogicalType::kFloat16: {
            auto value = std::static_pointer_cast<arrow::HalfFloatArray>(array)->Value(value_idx);
            const Float16T float16_value(value);
            return Value::MakeFloat16(float16_value);
        }
        case LogicalType::kBFloat16: {
            auto value = std::static_pointer_cast<arrow::FloatArray>(array)->Value(value_idx);
            const BFloat16T bfloat16_value(value);
            return Value::MakeBFloat16(bfloat16_value);
        }
        case LogicalType::kFloat: {
            auto value = std::static_pointer_cast<arrow::FloatArray>(array)->Value(value_idx);
            return Value::MakeFloat(value);
        }
        case LogicalType::kDouble: {
            auto value = std::static_pointer_cast<arrow::DoubleArray>(array)->Value(value_idx);
            return Value::MakeDouble(value);
        }
        case LogicalType::kDate: {
            auto value = std::static_pointer_cast<arrow::Date32Array>(array)->Value(value_idx);
            const DateT date_value(value);
            return Value::MakeDate(date_value);
        }
        case LogicalType::kTime: {
            auto value = std::static_pointer_cast<arrow::Time32Array>(array)->Value(value_idx);
            const TimeT time_value(value);
            return Value::MakeTime(time_value);
        }
        case LogicalType::kDateTime: {
            auto value = std::static_pointer_cast<arrow::TimestampArray>(array)->Value(value_idx);
            const DateTimeT datetime_value(value);
            return Value::MakeDateTime(datetime_value);
        }
        case LogicalType::kTimestamp: {
            auto value = std::static_pointer_cast<arrow::TimestampArray>(array)->Value(value_idx);
            const TimestampT timestamp_value(value);
            return Value::MakeTimestamp(timestamp_value);
        }
        case LogicalType::kVarchar: {
            String value_str = std::static_pointer_cast<arrow::StringArray>(array)->GetString(value_idx);
            return Value::MakeVarchar(std::move(value_str));
        }
        case LogicalType::kEmbedding: {
            auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
            if (auto fixed_list_array = std::dynamic_pointer_cast<arrow::FixedSizeListArray>(array); fixed_list_array.get() != nullptr) {
                auto [data, size] = ParquetEmbeddingHandler(fixed_list_array, embedding_info, value_idx);
                return Value::MakeEmbedding(data.get(), data_type.type_info());
            }
            auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            auto [data, size] = ParquetEmbeddingHandler(list_array, embedding_info, value_idx);
            return Value::MakeEmbedding(data.get(), data_type.type_info());
        }
        case LogicalType::kSparse: {
            const auto sparse_info = std::dynamic_pointer_cast<SparseInfo>(data_type.type_info());
            auto struct_array = std::static_pointer_cast<arrow::StructArray>(array);
            auto index_raw_array = struct_array->GetFieldByName("index");
            auto index_array = std::static_pointer_cast<arrow::ListArray>(index_raw_array);
            i64 start_offset = index_array->value_offset(value_idx);
            i64 end_offset = index_array->value_offset(value_idx + 1);
            SharedPtr<arrow::ListArray> data_array;
            auto value_raw_array = struct_array->GetFieldByName("value");
            if (value_raw_array.get() != nullptr) {
                data_array = std::static_pointer_cast<arrow::ListArray>(value_raw_array);
                i64 start_offset1 = index_array->value_offset(value_idx);
                i64 end_offset1 = index_array->value_offset(value_idx + 1);
                if (start_offset != start_offset1 || end_offset != end_offset1) {
                    RecoverableError(Status::ImportFileFormatError("Invalid parquet file format."));
                }
            }
            switch (sparse_info->IndexType()) {
                case EmbeddingDataType::kElemInt8: {
                    const auto int8_index_array = std::static_pointer_cast<arrow::Int8Array>(index_array->values());
                    return GetSparseValueFromParquet<i8, arrow::Int8Array>(sparse_info, int8_index_array, data_array, start_offset, end_offset);
                }
                case EmbeddingDataType::kElemInt16: {
                    const auto int16_index_array = std::static_pointer_cast<arrow::Int16Array>(index_array->values());
                    return GetSparseValueFromParquet<i16, arrow::Int16Array>(sparse_info, int16_index_array, data_array, start_offset, end_offset);
                }
                case EmbeddingDataType::kElemInt32: {
                    const auto int32_index_array = std::static_pointer_cast<arrow::Int32Array>(index_array->values());
                    return GetSparseValueFromParquet<i32, arrow::Int32Array>(sparse_info, int32_index_array, data_array, start_offset, end_offset);
                }
                case EmbeddingDataType::kElemInt64: {
                    const auto int64_index_array = std::static_pointer_cast<arrow::Int64Array>(index_array->values());
                    return GetSparseValueFromParquet<i64, arrow::Int64Array>(sparse_info, int64_index_array, data_array, start_offset, end_offset);
                }
                default: {
                    UnrecoverableError("Invalid sparse index type.");
                    return Value::MakeInvalid();
                }
            }
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor: {
            auto embedding_info = std::static_pointer_cast<EmbeddingInfo>(data_type.type_info());
            auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            Vector<Pair<UniquePtr<char[]>, SizeT>> embedding_vec = ParquetTensorHandler(list_array, embedding_info.get(), value_idx);
            Vector<Pair<ptr_t, SizeT>> embedding_data;
            for (const auto &[data_ptr, data_bytes] : embedding_vec) {
                embedding_data.emplace_back(data_ptr.get(), data_bytes);
            }
            if (data_logical_type == LogicalType::kMultiVector) {
                return Value::MakeMultiVector(embedding_data, std::move(embedding_info));
            }
            assert(data_logical_type == LogicalType::kTensor);
            return Value::MakeTensor(embedding_data, std::move(embedding_info));
        }
        case LogicalType::kTensorArray: {
            const auto embedding_info = std::static_pointer_cast<EmbeddingInfo>(data_type.type_info());
            auto value = Value::MakeTensorArray(embedding_info);
            auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            i64 start_offset = list_array->value_offset(value_idx);
            i64 end_offset = list_array->value_offset(value_idx + 1);
            auto tensor_array = std::static_pointer_cast<arrow::ListArray>(list_array->values());
            for (i64 j = start_offset; j < end_offset; ++j) {
                Vector<Pair<UniquePtr<char[]>, SizeT>> embedding_vec = ParquetTensorHandler(tensor_array, embedding_info.get(), j);
                Vector<Pair<ptr_t, SizeT>> embedding_data;
                for (auto &data : embedding_vec) {
                    embedding_data.push_back({data.first.get(), data.second});
                }
                value.AppendToTensorArray(embedding_data);
            }
            return value;
        }
        case LogicalType::kArray: {
            auto array_info = std::static_pointer_cast<ArrayInfo>(data_type.type_info());
            const auto &elem_type = array_info->ElemType();
            const auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
            const auto start_offset = list_array->value_offset(value_idx);
            const auto end_offset = list_array->value_offset(value_idx + 1);
            const auto array_elements = list_array->values();
            Vector<Value> array_value_vec;
            for (auto j = start_offset; j < end_offset; ++j) {
                auto v = GetValueFromParquetRecursively(elem_type, array_elements, j);
                array_value_vec.push_back(std::move(v));
            }
            return Value::MakeArray(std::move(array_value_vec), std::move(array_info));
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
            String error_message = "Not implement: Invalid data type.";
            UnrecoverableError(error_message);
            return Value::MakeInvalid();
        }
    }
}
} // namespace infinity
