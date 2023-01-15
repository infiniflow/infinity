//
// Created by jinhai on 22-12-25.
//

#pragma once

#include "column_def.h"

namespace infinity {

class TableDef {
public:
    static inline SharedPtr<TableDef>
    Make(String table_name, Vector<SharedPtr<ColumnDef>> columns, bool if_not_exists) {
        return MakeShared<TableDef>(std::move(table_name), std::move(columns), if_not_exists);
    }

public:
    explicit
    TableDef(String table_name, Vector<SharedPtr<ColumnDef>> columns, bool if_not_exists)
            : columns_(std::move(columns)), name_(std::move(table_name)), if_not_exists_(if_not_exists) {
        for(i64 i = 0; i < columns_.size(); ++ i) {
            column_name2id_[columns_[i]->name()] = i;
        }
    }

    [[nodiscard]] inline const Vector<SharedPtr<ColumnDef>>&
    columns() const {
        return columns_;
    }

    [[nodiscard]] inline size_t
    column_count() const {
        return columns_.size();
    }

    [[nodiscard]] inline const String&
    name() const {
        return name_;
    }

    [[nodiscard]] inline i64
    GetColIdByName(const String& name) const {
        if(column_name2id_.contains(name)) {
            return column_name2id_.at(name);
        } else {
            return -1;
        }
    }

    String
    ToString() const;

private:
    Vector<SharedPtr<ColumnDef>> columns_;
    HashMap<String, i64> column_name2id_;
    String name_;
    bool if_not_exists_{false};
};
}

