//
// Created by JinHai on 2022/7/28.
//

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
import block_column_entry;
import block_entry;
import db_entry;
import table_collection_meta;
import wal_entry;
import file_system_type;
import file_system;
import buffer_handle;

import infinity_exception;
import table_collection_entry;
import segment_entry;
import zsv;
#include "statement/statement_common.h"
#include "type/info/embedding_info.h"
#include "type/info/varchar_info.h"

module physical_import;

namespace infinity {

void PhysicalImport::Init() {}

/**
 * @brief copy statement execute function
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalImport::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto import_input_state = static_cast<ImportInputState *>(input_state);
    auto import_output_state = static_cast<ImportOutputState *>(output_state);
    switch (file_type_) {
        case CopyFileType::kCSV: {
            ImportCSV(query_context, import_input_state, import_output_state);
            break;
        }
        case CopyFileType::kJSON: {
            ImportJSON(query_context, import_input_state, import_output_state);
            break;
        }
        case CopyFileType::kFVECS: {
            ImportFVECS(query_context, import_input_state, import_output_state);
            break;
        }
    }
    output_state->SetComplete();
}

void PhysicalImport::Execute(QueryContext *query_context) {}

void PhysicalImport::ImportFVECS(QueryContext *query_context, ImportInputState *input_state, ImportOutputState *output_state) {
    if (table_collection_entry_->columns_.size() != 1) {
        Error<ExecutorException>("FVECS file must have only one column.");
    }
    auto &column_type = table_collection_entry_->columns_[0]->column_type_;
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
    if (embedding_info->Dimension() != dimension) {
        Error<ExecutorException>(Format("Dimension in file ({}) doesn't match with table definition ({}).", dimension, embedding_info->Dimension()));
    }
    SizeT file_size = fs.GetFileSize(*file_handler);
    SizeT row_size = dimension * sizeof(FloatT) + sizeof(dimension);
    if (file_size % row_size != 0) {
        Error<ExecutorException>("Weird file size.");
    }
    SizeT vector_n = file_size / row_size;

    Txn *txn = query_context->GetTxn();
    const String &db_name = *TableCollectionEntry::GetDBEntry(table_collection_entry_)->db_name_;
    const String &table_name = *table_collection_entry_->table_collection_name_;
    txn->AddTxnTableStore(table_name, MakeUnique<TxnTableStore>(table_name, table_collection_entry_, txn));
    TxnTableStore *txn_store = txn->GetTxnTableStore(table_name);

    u64 segment_id = TableCollectionEntry::GetNextSegmentID(table_collection_entry_);
    SharedPtr<SegmentEntry> segment_entry =
        SegmentEntry::MakeNewSegmentEntry(table_collection_entry_, segment_id, query_context->GetTxn()->GetBufferMgr());
    BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
    BufferHandle buffer_handle = last_block_entry->columns_[0]->buffer_->Load();
    SizeT row_idx = 0;

    while (true) {
        int dim;
        nbytes = fs.Read(*file_handler, &dim, sizeof(dimension));
        if (dim != dimension or nbytes != sizeof(dimension)) {
            Error<ExecutorException>(Format("Dimension in file ({}) doesn't match with table definition ({}).", dim, dimension));
        }
        ptr_t dst_ptr = static_cast<ptr_t>(buffer_handle.GetDataMut()) + row_idx * sizeof(FloatT) * dimension;
        fs.Read(*file_handler, dst_ptr, sizeof(FloatT) * dimension);
        ++segment_entry->row_count_;
        ++last_block_entry->row_count_;

        if (BlockEntry::Room(last_block_entry) <= 0) {
            segment_entry->block_entries_.emplace_back(MakeUnique<BlockEntry>(segment_entry.get(),
                                                                              segment_entry->block_entries_.size(),
                                                                              0,
                                                                              segment_entry->column_count_,
                                                                              txn->GetBufferMgr()));
            last_block_entry = segment_entry->block_entries_.back().get();
        }

        row_idx++;
        if (row_idx == vector_n) {
            SaveSegmentData(txn, segment_entry, db_name, table_name);
            break;
        }
        if (SegmentEntry::Room(segment_entry.get()) <= 0) {
            SaveSegmentData(txn, segment_entry, db_name, table_name);

            segment_id = TableCollectionEntry::GetNextSegmentID(table_collection_entry_);
            segment_entry = SegmentEntry::MakeNewSegmentEntry(table_collection_entry_, segment_id, query_context->GetTxn()->GetBufferMgr());

            last_block_entry = segment_entry->block_entries_.back().get();
            buffer_handle = last_block_entry->columns_[0]->buffer_->Load();
        }
    }
    auto result_msg = MakeUnique<String>(Format("IMPORT {} Rows", vector_n));
    output_state->result_msg_ = Move(result_msg);
}

void PhysicalImport::ImportCSV(QueryContext *query_context, ImportInputState *input_state, ImportOutputState *output_state) {
    // opts, parser and parser_context points to each other.
    // opt -> parser_context
    // parser->opt
    // parser_context -> parser
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if (!fp) {
        Error<ExecutorException>(strerror(errno));
    }
    Txn *txn = query_context->GetTxn();
    txn->AddTxnTableStore(*table_collection_entry_->table_collection_name_,
                          MakeUnique<TxnTableStore>(*table_collection_entry_->table_collection_name_, table_collection_entry_, txn));
    u64 segment_id = TableCollectionEntry::GetNextSegmentID(table_collection_entry_);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::MakeNewSegmentEntry(table_collection_entry_, segment_id, txn->GetBufferMgr());

    auto parser_context = MakeUnique<ParserContext>(table_collection_entry_, txn, segment_entry, delimiter_);

    auto opts = MakeUnique<ZsvOpts>();
    if (header_) {
        opts->row_handler = CSVHeaderHandler;
    } else {
        opts->row_handler = CSVRowHandler;
    }
    opts->delimiter = delimiter_;
    opts->stream = fp;
    opts->ctx = parser_context.get();
    opts->buffsize = (1 << 20); // default buffer size 256k, we use 1M\

    parser_context->parser_ = ZsvParser(opts.get());

    ZsvStatus csv_parser_status;
    while ((csv_parser_status = parser_context->parser_.ParseMore()) == zsv_status_ok) {
        ;
    }
    parser_context->parser_.Finish();

    // add the last segment entry
    if (parser_context->segment_entry_->row_count_ > 0) {
        const String &db_name = *TableCollectionEntry::GetDBEntry(table_collection_entry_)->db_name_;
        const String &table_name = *table_collection_entry_->table_collection_name_;
        SaveSegmentData(parser_context->txn_, parser_context->segment_entry_, db_name, table_name);
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
    table_collection_entry_->row_count_ += parser_context->row_count_;

    auto result_msg = MakeUnique<String>(Format("IMPORT {} Rows", parser_context->row_count_));
    output_state->result_msg_ = Move(result_msg);
}

void PhysicalImport::ImportJSON(QueryContext *query_context, ImportInputState *input_state, ImportOutputState *output_state) {}

void PhysicalImport::CSVHeaderHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext *>(context);
    ZsvParser &parser = parser_context->parser_;
    SizeT csv_column_count = parser.CellCount();

    SizeT table_column_count = parser_context->table_collection_entry_->columns_.size();
    if (csv_column_count != table_column_count) {
        parser_context->err_msg_ = MakeShared<String>(Format("Unmatched column count ({} != {})", csv_column_count, table_column_count));

        parser.Abort(); // return zsv_status_cancelled
        return;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(parser.GetCellStr(idx));
        auto *table_col_name = parser_context->table_collection_entry_->columns_[idx]->name().c_str();
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
    BlockColumnEntry::AppendRaw(column_data_entry, dst_offset, reinterpret_cast<ptr_t>(&ele), sizeof(T));
}

template <typename T>
void AppendEmbeddingData(BlockColumnEntry *column_data_entry, const Vector<StringView> &ele_str_views, SizeT dst_offset) {
    SizeT arr_len = ele_str_views.size();
    auto tmp_buffer = MakeUnique<T[]>(arr_len);
    for (SizeT ele_idx = 0; auto &ele_str_view : ele_str_views) {
        T ele = DataType::StringToValue<T>(ele_str_view);
        tmp_buffer[ele_idx++] = ele;
    }
    BlockColumnEntry::AppendRaw(column_data_entry, dst_offset, reinterpret_cast<ptr_t>(tmp_buffer.get()), sizeof(T) * arr_len);
}

void AppendVarcharData(BlockColumnEntry *column_data_entry, const StringView &str_view, SizeT dst_offset) {
    const char_t *tmp_buffer = str_view.data();
    auto varchar_type = MakeUnique<VarcharT>(str_view.data(), str_view.size());
    // TODO shenyushi: unnecessary copy here.
    BlockColumnEntry::AppendRaw(column_data_entry, dst_offset, reinterpret_cast<ptr_t>(varchar_type.get()), sizeof(VarcharT));
}

} // namespace

void PhysicalImport::CSVRowHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext *>(context);
    auto *table = parser_context->table_collection_entry_;
    SizeT column_count = parser_context->parser_.CellCount();
    auto *txn = parser_context->txn_;
    auto txn_store = txn->GetTxnTableStore(*table->table_collection_name_);

    auto segment_entry = parser_context->segment_entry_;
    const String &db_name = *TableCollectionEntry::GetDBEntry(table)->db_name_;
    const String &table_name = *table->table_collection_name_;
    // we have already used all space of the segment
    if (SegmentEntry::Room(segment_entry.get()) <= 0) {
        SaveSegmentData(txn, segment_entry, db_name, table_name);
        parser_context->segment_entry_ = SegmentEntry::MakeNewSegmentEntry(table, txn->TxnID(), txn->GetBufferMgr());
        segment_entry = parser_context->segment_entry_;
    }

    BlockEntry *last_block_entry = segment_entry->block_entries_.back().get();
    if (BlockEntry::Room(last_block_entry) <= 0) {
        segment_entry->block_entries_.emplace_back(
            MakeUnique<BlockEntry>(segment_entry.get(), segment_entry->block_entries_.size(), 0, segment_entry->column_count_, txn->GetBufferMgr()));
        last_block_entry = segment_entry->block_entries_.back().get();
    }

    // append data to segment entry
    SizeT write_row = last_block_entry->row_count_;
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        ZsvCell cell = parser_context->parser_.GetCell(column_idx);
        StringView str_view{};
        if (cell.len) {
            str_view = StringView((char *)cell.str, cell.len);
        }
        BlockColumnEntry *block_column_entry = last_block_entry->columns_[column_idx].get();
        auto column_type = block_column_entry->column_type_.get();
        SizeT dst_offset = write_row * column_type->Size();
        if (column_type->type() == kVarchar) {
            auto varchar_info = dynamic_cast<VarcharInfo *>(column_type->type_info().get());
            if (varchar_info->dimension() < str_view.size()) {
                Error<ExecutorException>("Varchar data size exceeds dimension.");
            }

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
    ++last_block_entry->row_count_;
    ++segment_entry->row_count_;
    ++parser_context->row_count_;
}
void PhysicalImport::SaveSegmentData(Txn *txn, SharedPtr<SegmentEntry> &segment_entry, const String &db_name, const String &table_name) {
    Vector<u16> block_row_counts;

    block_row_counts.reserve(segment_entry->block_entries_.size());
    for (auto &block_entry : segment_entry->block_entries_) {
        BlockEntry::FlushData(block_entry.get(), block_entry->row_count_);
        auto size = std::max(segment_entry->block_entries_.size(), static_cast<SizeT>(block_entry->block_id_ + 1));
        block_row_counts.resize(size);
        block_row_counts[block_entry->block_id_] = block_entry->row_count_;
    }

    LOG_TRACE(Format("Block rows count {}", block_row_counts.size()));
    for (int i = 0; i < block_row_counts.size(); ++i) {
        LOG_TRACE(Format("Block {} rows count {}", i, block_row_counts[i]));
    }

    txn->AddWalCmd(MakeShared<WalCmdImport>(db_name,
                                            table_name,
                                            *segment_entry->segment_dir_,
                                            segment_entry->segment_id_,
                                            segment_entry->block_entries_.size(),
                                            block_row_counts));

    txn->GetTxnTableStore(table_name)->Import(segment_entry);
}

} // namespace infinity
