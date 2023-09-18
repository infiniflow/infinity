//
// Created by JinHai on 2022/7/28.
//

#include "physical_import.h"
#include "common/types/internal_types.h"
#include "executor/operator_state.h"
#include "main/query_context.h"
#include "storage/meta/entry/column_data_entry.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/txn/txn_store.h"
#include <cstring>
extern "C" {
#include "third_party/zsv/include/zsv/common.h"
#include "third_party/zsv/include/zsv/api.h"
}


namespace infinity {

void
PhysicalImport::Init() {

}


/**
 * @brief copy statement execute function
 * @param query_context
 * @param input_state
 * @param output_state
 */
void
PhysicalImport::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {

    auto import_input_state = static_cast<ImportInputState*>(input_state);
    auto import_output_state = static_cast<ImportOutputState*>(output_state);
    switch(file_type_) {
        case CopyFileType::kCSV: {
            return ImportCSV(query_context, import_input_state, import_output_state);
        }
        case CopyFileType::kJSON: {
            return ImportJSON(query_context);
        }
    }

}

void
PhysicalImport::Execute(QueryContext* query_context) {
    switch(file_type_) {
        case CopyFileType::kCSV: {
            return ImportCSV(query_context);
        }
        case CopyFileType::kJSON: {
            return ImportJSON(query_context);
        }
    }
}

void
PhysicalImport::ImportCSVHelper(QueryContext* query_context, ParserContext &parser_context) {
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if(!fp) {
        ExecutorError(strerror(errno));
    }
    struct zsv_opts opts{};

    // TODO: redesign parser_context
    parser_context.table_collection_entry_ = table_collection_entry_;

    parser_context.txn_ = query_context->GetTxn();

    parser_context.txn_->AddTxnTableStore(*table_collection_entry_->table_collection_name_,
                                          MakeUnique<TxnTableStore>(
                                                  *table_collection_entry_->table_collection_name_,
                                                                    table_collection_entry_,
                                                                    parser_context.txn_));


    parser_context.segment_entry_ = SegmentEntry::MakeNewSegmentEntry(table_collection_entry_,
                                                                      parser_context.txn_->TxnID(),
                                                                      TableCollectionEntry::GetNextSegmentID(table_collection_entry_),
                                                                      parser_context.txn_->GetBufferMgr());
    parser_context.delimiter_ = delimiter_;

    const String &table_name = *table_collection_entry_->table_collection_name_;

    if(header_) {
        opts.row_handler = CSVHeaderHandler;
    } else {
        opts.row_handler = CSVRowHandler;
    }

    opts.delimiter = delimiter_;
    opts.stream = fp;
    opts.ctx = &parser_context;
    opts.buffsize = (1<<20); // default buffer size 256k, we use 1M

    parser_context.parser_ = zsv_new(&opts);
    enum zsv_status csv_parser_status;
    while((csv_parser_status = zsv_parse_more(parser_context.parser_)) == zsv_status_ok) {
        ;
    }
    

    zsv_finish(parser_context.parser_);
    // flush the last segment entry
    if (parser_context.segment_entry_->current_row_ > 0) {
        auto txn_store = parser_context.txn_->GetTxnTableStore(table_name);
        txn_store->Import(parser_context.segment_entry_);
    }

    zsv_delete(parser_context.parser_);

    fclose(fp);
    if(csv_parser_status != zsv_status_no_more_input) {
        if(parser_context.err_msg_ != nullptr) {
            ExecutorError(*parser_context.err_msg_);
        } else {
            String err_msg = (char*)zsv_parse_status_desc(csv_parser_status);
            ExecutorError(err_msg);
        }
    }
    table_collection_entry_->row_count_ += parser_context.row_count_;
}

void
PhysicalImport::ImportCSV(QueryContext* query_context) {
    ParserContext parser_context{};

    ImportCSVHelper(query_context, parser_context);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs;

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);

    UniquePtr<String> result_msg = MakeUnique<String>(fmt::format("IMPORTED {} Rows", parser_context.row_count_));
    output_->SetResultMsg(std::move(result_msg));
}

void
PhysicalImport::ImportJSON(QueryContext* query_context) {
    DBEntry* db_entry = TableCollectionMeta::GetDBEntry(table_collection_entry_->table_collection_meta_);
    String err = fmt::format("IMPORT Table: {}.{} FROM file: {} WITH format JSON",
                             *db_entry->db_name_,
                             *table_collection_entry_->table_collection_name_,
                             file_path_);
    NotImplementError(err);
}

/**
 * @brief copy statement import json function
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalImport::ImportCSV(QueryContext *query_context,
                               ImportInputState *input_state,
                               ImportOutputState *output_state) {

    ParserContext parser_context{};
    ImportCSVHelper(query_context, parser_context);

//    // Generate the result
//    Vector<SharedPtr<ColumnDef>> column_defs;
//    auto result_table_def_ptr
//                = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
//    output_state->table_def_ = std::move(result_table_def_ptr);


    auto result_msg = MakeUnique<String>(fmt::format("IMPORT {} Rows", parser_context.row_count_));
    output_state->result_msg_ = std::move(result_msg);
}

/**
 * @brief copy statement import csv function
 * @param query_context
 * @param input_state
 * @param output_state
 */
void
PhysicalImport::ImportJSON(QueryContext *query_context,
                           ImportInputState *input_state,
                           ImportOutputState *output_state) {

}


void
PhysicalImport::CSVHeaderHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext*>(context);
    SizeT csv_column_count = zsv_cell_count(parser_context->parser_);

    SizeT table_column_count = parser_context->table_collection_entry_->columns_.size();
    if(csv_column_count != table_column_count) {
        parser_context->err_msg_ = MakeShared<String>(fmt::format("Unmatched column count ({} != {})",
                                                                  csv_column_count, table_column_count));

        zsv_abort(parser_context->parser_); // return zsv_status_cancelled
        return ;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(
            zsv_get_cell_str(parser_context->parser_, idx));
        auto *table_col_name = parser_context->table_collection_entry_->columns_[idx]->name().c_str();
        if (!strcmp(csv_col_name, table_col_name)) {
            parser_context->err_msg_ = MakeShared<String>(
                fmt::format("Unmatched column name({} != {})", csv_col_name, table_col_name));

            zsv_abort(parser_context->parser_); // return zsv_status_cancelled
            return;
        }
    }

    // This is header, doesn't count in row number

    zsv_set_row_handler(parser_context->parser_, CSVRowHandler);
}

void
PhysicalImport::CSVRowHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext*>(context);

    auto *table = parser_context->table_collection_entry_;
    SizeT column_count = zsv_cell_count(parser_context->parser_);
    auto *txn = parser_context->txn_;
    auto txn_store = txn->GetTxnTableStore(*table->table_collection_name_);

    auto segment_entry = parser_context->segment_entry_;
    // we have already used all space of the segment
    if (segment_entry->AvailableCapacity() == 0) {
        // add to txn_store
        txn_store->Import(segment_entry);

        // create new segment entry
        // TODO the segment_id is wrong
        parser_context->segment_entry_ = SegmentEntry::MakeNewSegmentEntry(table,
                                                                           txn->TxnID(),
                                                                           TableCollectionEntry::GetNextSegmentID(table),
                                                                           txn->GetBufferMgr());
        segment_entry = parser_context->segment_entry_;
    }

    SizeT write_row = segment_entry->current_row_;

    // append data to segment entry
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        struct zsv_cell cell = zsv_get_cell(parser_context->parser_, column_idx);
        StringView str_view{};
        if (cell.len) {
            str_view = StringView((char *)cell.str, cell.len);
        }

        auto column_data_entry = segment_entry->columns_[column_idx];
        if (segment_entry->columns_[column_idx]->column_type_->IsEmbedding()) {
            ColumnDataEntry::AppendEmbedding(column_data_entry.get(), str_view, write_row, parser_context->delimiter_);
        } else {
            ColumnDataEntry::Append(column_data_entry.get(), str_view, write_row);
        }
    }

    ++parser_context->row_count_;
    ++segment_entry->current_row_;
}

} // namespace infinity
