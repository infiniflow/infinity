//
// Created by JinHai on 2022/7/24.
//

#include "schema.h"
#include "common/utility/asserter.h"

namespace infinity {

std::shared_ptr<TableDefinition>
Schema::get_table_by_name(const std::string &name) {
    if (tables_.find(name) == tables_.end()) {
        ResponseError("Table not found: " + name);
    }
    return tables_[name];
}

void
Schema::add_table(const std::shared_ptr<TableDefinition>& table_def) {
    const std::string& table_name = table_def->name();
    if (tables_.find(table_name) == tables_.end()) {
        table_def->set_table_id(table_id_counter_++);
        tables_[table_name] = table_def;
    } else {
        ResponseError("Table already exists: " + table_name);
    }
}

void
Schema::delete_table(const std::string &table_name) {
    if (tables_.find(table_name) == tables_.end()) {
        ResponseError("Table not found, can't be dropped: " + table_name);
    }
    tables_.erase(table_name);
}

}
