//
// Created by JinHai on 2022/7/24.
//

#include "schema.h"
#include "common/utility/asserter.h"

namespace infinity {

std::shared_ptr<Table>
Schema::GetTableByName(const std::string &name) {
    if (tables_.find(name) == tables_.end()) {
//        ResponseError("Table not found: " + name);
        return nullptr;
    }
    return tables_[name];
}

void
Schema::AddTable(const std::shared_ptr<Table>& table_def) {
    const std::string& table_name = table_def->table_def()->name();
    if (tables_.find(table_name) == tables_.end()) {
        table_def->table_def()->set_table_id(id_counter_++);
        tables_[table_name] = table_def;
    } else {
        ResponseError("Table already exists: " + table_name);
    }
}

void
Schema::DeleteTable(const std::string &table_name) {
    if (tables_.find(table_name) == tables_.end()) {
        ResponseError("Table not found, can't be dropped: " + table_name);
    }
    tables_.erase(table_name);
}

std::shared_ptr<View>
Schema::GetViewByName(const std::string &name) {
    if (views_.find(name) == views_.end()) {
//        ResponseError("Table not found: " + name);
        return nullptr;
    }
    return views_[name];
}

void
Schema::AddView(const std::shared_ptr<View>& view) {
    const std::string& view_name = view->name();
    if (views_.find(view_name) == views_.end()) {
        views_[view_name] = view;
    } else {
        ResponseError("View already exists: " + view_name);
    }
}

void
Schema::DeleteView(const std::string &view_name) {
    if (views_.find(view_name) == views_.end()) {
        ResponseError("View not found, can't be dropped: " + view_name);
    }
    views_.erase(view_name);
}


}
