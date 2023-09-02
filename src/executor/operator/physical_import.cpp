//
// Created by JinHai on 2022/7/28.
//

#include "physical_import.h"
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
    Table* table_ptr_{};
};

void
PhysicalImport::ImportCSV(QueryContext* query_context) {
    FILE *fp = fopen(file_path_.c_str(), "rb");
    if(!fp) {
        ExecutorError(strerror(errno));
    }
    struct zsv_opts opts{};

    struct ParserContext parser_context{};
    if(header_) {
        opts.row_handler = CSVHeaderHandler;
        parser_context.table_ptr_ = table_ptr_.get();
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
    String err = fmt::format("IMPORT Table: {}.{} FROM file: {} WITH format JSON",
                             *table_ptr_->schema_name(),
                             *table_ptr_->table_name(),
                             file_path_);
    NotImplementError(err);
}

void
PhysicalImport::CSVHeaderHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext*>(context);
    SizeT csv_column_count = zsv_cell_count(parser_context->parser_);

    SizeT table_column_count = parser_context->table_ptr_->definition_ptr_->column_count();
    if(csv_column_count != table_column_count) {
        parser_context->err_msg_ =
                MakeShared<String>(fmt::format("Unmatched column count ({} != {})",
                                               csv_column_count, table_column_count));

        zsv_abort(parser_context->parser_); // return zsv_status_cancelled
        return ;
    }

    // Not check the header column name

    // This is header, doesn't count in row number

    zsv_set_row_handler(parser_context->parser_, CSVRowHandler);
}

void
PhysicalImport::CSVRowHandler(void *context) {
    ParserContext *parser_context = static_cast<ParserContext*>(context);

    SizeT column_count = zsv_cell_count(parser_context->parser_);

    for(SizeT column_idx = 0; column_idx < column_count; ++ column_idx) {
        struct zsv_cell cell = zsv_get_cell(parser_context->parser_, column_idx);

        if(cell.len > 0) {
            // non-blank cell

        } else {
            // blank cell

        }
    }

    ++ parser_context->row_count_;
}

}
