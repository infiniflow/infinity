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

module table;

import stl;
import query_options;
import query_result;
import infinity_context;
import query_context;
import parser;
import infinity_exception;

namespace infinity {

QueryResult Table::CreateIndex(const String &index_name, Vector<IndexInfo *> *index_info_list, CreateIndexOptions) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());

    UniquePtr<CreateStatement> create_statement = MakeUnique<CreateStatement>();
    SharedPtr<CreateIndexInfo> create_index_info = MakeShared<CreateIndexInfo>();

    create_index_info->schema_name_ = session_->current_database();
    create_index_info->table_name_ = table_name_;
    create_index_info->index_name_ = index_name;
    create_index_info->index_info_list_ = index_info_list;

    create_statement->create_info_ = create_index_info;
    create_statement->create_info_->conflict_type_ = ConflictType::kIgnore;

    QueryResult result = query_context_ptr->QueryStatement(create_statement.get());
    return result;
}

QueryResult Table::DropIndex(const String &index_name) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<DropStatement> drop_statement = MakeUnique<DropStatement>();
    SharedPtr<DropIndexInfo> drop_index_info = MakeShared<DropIndexInfo>();

    drop_index_info->schema_name_ = session_->current_database();
    drop_index_info->table_name_ = table_name_;
    drop_index_info->index_name_ = index_name;
    drop_statement->drop_info_ = drop_index_info;

    drop_statement->drop_info_->conflict_type_ = ConflictType::kIgnore;

    QueryResult result = query_context_ptr->QueryStatement(drop_statement.get());
    return result;
}

QueryResult Table::Insert(Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<InsertStatement> insert_statement = MakeUnique<InsertStatement>();

    insert_statement->schema_name_ = session_->current_database();
    insert_statement->table_name_ = table_name_;
    insert_statement->columns_ = columns;
    insert_statement->values_ = values;

    QueryResult result = query_context_ptr->QueryStatement(insert_statement.get());
    return result;
}

QueryResult Table::Import(const String &path, ImportOptions import_options) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<CopyStatement> import_statement = MakeUnique<CopyStatement>();

    import_statement->copy_from_ = true;
    import_statement->file_path_ = path;
    import_statement->schema_name_ = session_->current_database();
    import_statement->table_name_ = table_name_;

    import_statement->header_ = false;
    import_statement->copy_file_type_ = import_options.copy_file_type_;
    import_statement->delimiter_ = ',';

    QueryResult result = query_context_ptr->QueryStatement(import_statement.get());
    return result;
}

QueryResult Table::Delete(ParsedExpr *filter) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<DeleteStatement> delete_statement = MakeUnique<DeleteStatement>();
    delete_statement->schema_name_ = session_->current_database();
    delete_statement->table_name_ = table_name_;
    delete_statement->where_expr_ = filter;
    QueryResult result = query_context_ptr->QueryStatement(delete_statement.get());
    return result;
}

QueryResult Table::Update(ParsedExpr *filter, Vector<UpdateExpr *> *update_list) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<UpdateStatement> update_statement = MakeUnique<UpdateStatement>();
    update_statement->schema_name_ = session_->current_database();
    update_statement->table_name_ = table_name_;
    update_statement->where_expr_ = filter;
    update_statement->update_expr_array_ = update_list;
    QueryResult result = query_context_ptr->QueryStatement(update_statement.get());
    return result;
}

QueryResult Table::Search(Vector<ParsedExpr *> &knn_exprs,
                          Vector<Pair<ParsedExpr *, ParsedExpr *>> &,
                          ParsedExpr *filter,
                          Vector<ParsedExpr *> *output_columns,
                          ParsedExpr *offset,
                          ParsedExpr *limit) {
    UniquePtr<QueryContext> query_context_ptr = MakeUnique<QueryContext>(session_.get());
    query_context_ptr->Init(InfinityContext::instance().config(),
                            InfinityContext::instance().task_scheduler(),
                            InfinityContext::instance().storage(),
                            InfinityContext::instance().resource_manager(),
                            InfinityContext::instance().session_manager());
    UniquePtr<SelectStatement> select_statement = MakeUnique<SelectStatement>();

    auto *table_ref = new TableReference();
    table_ref->db_name_ = session_->current_database();
    table_ref->table_name_ = table_name_;
    select_statement->table_ref_ = table_ref;
    select_statement->select_list_ = output_columns;
    select_statement->where_expr_ = filter;
    select_statement->limit_expr_ = limit;
    select_statement->offset_expr_ = offset;

    select_statement->order_by_list = new Vector<OrderByExpr *>;
    SizeT knn_expr_size = knn_exprs.size();
    for(SizeT idx = 0; idx < knn_expr_size; ++ idx) {
        OrderByExpr* order_by_expr = new OrderByExpr();
        order_by_expr->expr_ = knn_exprs[idx];
        if(knn_exprs[idx]->type_ != ParsedExprType::kKnn) {
            Error<PlannerException>("Unmatched expression type");
        }

        KnnExpr* knn_expr = (KnnExpr*)(knn_exprs[idx]);
        switch(knn_expr->distance_type_) {
            case KnnDistanceType::kL2: {
                order_by_expr->type_ = OrderType::kAsc;
                break;
            }
            case KnnDistanceType::kInnerProduct: {
                order_by_expr->type_ = OrderType::kDesc;
                break;
            }
            case KnnDistanceType::kCosine: {
                order_by_expr->type_ = OrderType::kDesc;
                break;
            }
            case KnnDistanceType::kHamming: {
                order_by_expr->type_ = OrderType::kAsc;
                break;
            }
            case KnnDistanceType::kInvalid: {
                Error<PlannerException>("Invalid knn distance type");
                break;
            }
        }
        select_statement->order_by_list->emplace_back(order_by_expr);
    }

    QueryResult result = query_context_ptr->QueryStatement(select_statement.get());
    return result;
}

} // namespace infinity
