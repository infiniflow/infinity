//
// Created by jinhai on 22-12-25.
//

#pragma once

#include "parser/statement/extra/create_table_info.h"

namespace infinity {

class TableDef {
public:
    static inline SharedPtr<TableDef>
    Make(String table_name, Vector<SharedPtr<ColumnDef>> columns) {
        return MakeShared<TableDef>(std::move(table_name), std::move(columns));
    }

public:
    explicit
    TableDef(String table_name, Vector<SharedPtr<ColumnDef>> columns)
            : columns_(std::move(columns)),
            table_name_(std::move(table_name)) {
        SizeT column_count = columns_.size();
        for(SizeT idx = 0; idx < column_count; ++ idx) {
            column_name2id_[columns_[idx]->name()] = idx;
        }
    }

    [[nodiscard]] inline const Vector<SharedPtr<ColumnDef>>&
    columns() const {
        return columns_;
    }

    [[nodiscard]] inline SizeT
    column_count() const {
        return columns_.size();
    }

    [[nodiscard]] inline const String&
    table_name() const {
        return table_name_;
    }

    [[nodiscard]] inline const String&
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline SizeT
    GetColIdByName(const String& name) const {
        if(column_name2id_.contains(name)) {
            return column_name2id_.at(name);
        } else {
            return -1;
        }
    }

    String
    ToString() const;

    void
    UnionWith(const SharedPtr<TableDef>& other);

private:
    Vector<SharedPtr<ColumnDef>> columns_{};
    HashMap<String, SizeT> column_name2id_{};
    String table_name_{};
    String schema_name_{"Default"};
};
}

