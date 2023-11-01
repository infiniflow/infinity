//
// Created by jinhai on 23-11-1.
//

module;

module table;

import stl;
// import std;
//// import logger;
//// import config;
//// import resource_manager;
//// import fragment_scheduler;
//// import storage;
//// import local_file_system;
//// import std;
//// import third_party;
import query_options;
import query_result;
// import table;
import infinity_context;
// import session;
import query_context;
//// import database_object;
import parser;

namespace infinity {

QueryResult Table::CreateIndex(const String &index_name, Vector<String> *column_names, CreateIndexOptions create_index_options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<CreateStatement> create_statement = MakeUnique<CreateStatement>();
    SharedPtr<CreateIndexInfo> create_index_info = MakeShared<CreateIndexInfo>();

    create_index_info->schema_name_ = session_->current_database();
    create_index_info->table_name_ = table_name_;
    create_index_info->index_name_ = index_name;
    create_index_info->column_names_ = column_names;

    QueryResponse response = query_context_ptr->QueryStatement(create_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Table::DropIndex(const String &index_name) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<DropStatement> drop_statement = MakeUnique<DropStatement>();
    SharedPtr<DropIndexInfo> drop_index_info = MakeShared<DropIndexInfo>();

    drop_index_info->schema_name_ = session_->current_database();
    drop_index_info->table_name_ = table_name_;
    drop_index_info->index_name_ = index_name;

    QueryResponse response = query_context_ptr->QueryStatement(drop_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Table::Insert(Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<InsertStatement> insert_statement = MakeUnique<InsertStatement>();

    insert_statement->schema_name_ = session_->current_database();
    insert_statement->table_name_ = table_name_;
    insert_statement->columns_ = columns;
    insert_statement->values_ = values;

    QueryResponse response = query_context_ptr->QueryStatement(insert_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Table::Import(const String &path, ImportOptions import_options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<CopyStatement> import_statement = MakeUnique<CopyStatement>();

    import_statement->copy_from_ = true;
    import_statement->file_path_ = path;
    import_statement->schema_name_ = session_->current_database();
    import_statement->table_name_ = table_name_;

//    bool header_{false};
//    CopyFileType copy_file_type_{CopyFileType::kCSV};
//    char delimiter_{','};

    QueryResponse response = query_context_ptr->QueryStatement(import_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

QueryResult Table::Delete(const String &condition) {}

QueryResult Table::Update(const String &condition, Vector<Pair<String, ParsedExpr *>> &set_lists) {}

QueryResult Table::Search(Vector<Pair<ParsedExpr *, ParsedExpr *>> &vector_expr,
                          Vector<Pair<ParsedExpr *, ParsedExpr *>> &fts_expr,
                          ParsedExpr *filter,
                          Vector<ParsedExpr *> *output_columns,
                          ParsedExpr *offset,
                          ParsedExpr *limit) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().fragment_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager());
    UniquePtr<SelectStatement> select_statement = MakeUnique<SelectStatement>();

    TableReference *table_ref = new TableReference();
    table_ref->db_name_ = session_->current_database();
    table_ref->table_name_ = table_name_;
    select_statement->table_ref_ = table_ref;
    select_statement->select_list_ = output_columns;
    select_statement->where_expr_ = filter;
    select_statement->limit_expr_ = limit;
    select_statement->offset_expr_ = offset;

    QueryResponse response = query_context_ptr->QueryStatement(select_statement.get());
    QueryResult result;
    result.result_table_ = response.result_;
    if(response.result_msg_.get() != nullptr) {
        result.error_message_ = response.result_msg_;
        result.error_code_ = -1;
    }
    return result;
}

} // namespace infinity
