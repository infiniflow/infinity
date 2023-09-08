//
// Created by JinHai on 2022/7/28.
//

#include "physical_import.h"
#include "common/types/internal_types.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/meta/entry/column_data_entry.h"
#include "storage/meta/entry/segment_entry.h"
#include "storage/txn/txn_manager.h"
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

struct ParserContext {
    zsv_parser parser_{};
    size_t row_count_{};
    SharedPtr<String> err_msg_{};
    TableCollectionEntry* table_collection_entry_{};
    Txn* txn_{};
    SharedPtr<SegmentEntry> segment_entry_{};
};

void
PhysicalImport::ImportCSV(QueryContext* query_context) {
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if(!fp) {
        ExecutorError(strerror(errno));
    }
    struct zsv_opts opts{};

    struct ParserContext parser_context{};
    // TODO: redesign parser_context
    parser_context.table_collection_entry_ = table_collection_entry_;
    parser_context.txn_ = query_context->GetTxn();
    parser_context.txn_->AddTxnTableStore(
        *table_collection_entry_->table_collection_name_,
        MakeUnique<TxnTableStore>(
            *table_collection_entry_->table_collection_name_,
            table_collection_entry_, parser_context.txn_));
    parser_context.segment_entry_ = SegmentEntry::MakeNewSegmentEntry(
        table_collection_entry_, parser_context.txn_->TxnID(),
        TableCollectionEntry::GetNextSegmentID(table_collection_entry_),
        parser_context.txn_->GetBufferMgr());

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

    // flush the last segment entry
    if (parser_context.segment_entry_->current_row_ > 0) {
        auto txn_store = parser_context.txn_->GetTxnTableStore(table_name);
        // flush the segment entry
        SegmentEntry::PrepareFlush(parser_context.segment_entry_.get());
        SegmentEntry::Flush(parser_context.segment_entry_.get());
        txn_store->Import(parser_context.segment_entry_);
    }

    

    zsv_finish(parser_context.parser_);
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

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs;

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);

    SharedPtr<String> result_msg = MakeShared<String>(fmt::format("IMPORTED {} Rows", parser_context.row_count_));
    output_->SetResultMsg(result_msg);
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

void
PhysicalImport::CSVHeaderHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext*>(context);
    SizeT csv_column_count = zsv_cell_count(parser_context->parser_);

    SizeT table_column_count = parser_context->table_collection_entry_->columns_.size();
    if(csv_column_count != table_column_count) {
        parser_context->err_msg_ =
                MakeShared<String>(fmt::format("Unmatched column count ({} != {})",
                                               csv_column_count, table_column_count));

        zsv_abort(parser_context->parser_); // return zsv_status_cancelled
        return ;
    }

    // Not check the header column name
    for (SizeT idx = 0; idx < csv_column_count; ++idx) {
        auto *csv_col_name = reinterpret_cast<const char *>(
            zsv_get_cell_str(parser_context->parser_, idx));
        auto *table_col_name =
            parser_context->table_collection_entry_->columns_[idx]
                ->name()
                .c_str();
        if (!strcmp(csv_col_name, table_col_name)) {
            parser_context->err_msg_ = MakeShared<String>(
                fmt::format("Unmatched column name({} != {})", csv_col_name,
                            table_col_name));

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
    // we have already eat all space in the segment
    if (segment_entry->AvailableCapacity() == 0) {
        // flush the segment entry
        SegmentEntry::PrepareFlush(segment_entry.get());
        SegmentEntry::Flush(segment_entry.get());
        
        // add to txn_store
        txn_store->Import(segment_entry);
        
        
        // create new segment entry
        parser_context->segment_entry_ = SegmentEntry::MakeNewSegmentEntry(
            table, txn->TxnID(), TableCollectionEntry::GetNextSegmentID(table),
            txn->GetBufferMgr());
        segment_entry = parser_context->segment_entry_;
    }

    SizeT write_row = segment_entry->current_row_;

    // append data to segment entry
    for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
        struct zsv_cell cell =
            zsv_get_cell(parser_context->parser_, column_idx);
        if (cell.len > 0) {
            ColumnDataEntry::Append(segment_entry->columns_[column_idx].get(),
                                    String((char *)cell.str, cell.len),
                                    write_row);
        } else {
            ColumnDataEntry::Append(segment_entry->columns_[column_idx].get(),
                                    "", write_row);
        }
    }

    ++parser_context->row_count_;
    ++segment_entry->current_row_;
}

}
