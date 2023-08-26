//
// Created by JinHai on 2022/7/24.
//

#include "schema.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<BaseTable>
Schema::GetTableByName(const String &name) {
    if (tables_.find(name) == tables_.end()) {
        // Table not found
        return nullptr;
    }
    return tables_[name];
}

void
Schema::AddTable(const SharedPtr<BaseTable>& table, ConflictType conflict_type) {
    const String& table_name = *table->table_name();
    if (tables_.find(table_name) == tables_.end()) {
        tables_[table_name] = table;
    } else {
        CatalogError("Table already exists: " + table_name);
    }
}

void
Schema::DeleteTable(const String &table_name, ConflictType conflict_type) {
    if (tables_.find(table_name) == tables_.end()) {
        switch (conflict_type) {
            case ConflictType::kIgnore: {
                return ;
            }
            case ConflictType::kError: {
                CatalogError("Table not found, can't be dropped: " + table_name);
            }
            default: {
                CatalogError("Invalid conflict type when delete table.");
            }
        }

    }
    tables_.erase(table_name);
}

SharedPtr<View>
Schema::GetViewByName(const String &name) {
    if (views_.find(name) == views_.end()) {
        // View not found
        return nullptr;
    }
    return views_[name];
}

void
Schema::AddView(const SharedPtr<View>& view, ConflictType conflict_type) {
    const String& view_name = view->view_name();
    if (views_.find(view_name) == views_.end()) {
        views_[view_name] = view;
    } else {
        CatalogError("View already exists: " + view_name);
    }
}

void
Schema::DeleteView(const String &view_name, ConflictType conflict_type) {
    if (views_.find(view_name) == views_.end()) {
        CatalogError("View not found, can't be dropped: " + view_name);
    }
    views_.erase(view_name);
}


}
