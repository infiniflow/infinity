//
// Created by jinhai on 23-6-22.
//

#pragma once

#include "parser/statement/extra/create_table_info.h"

namespace infinity {

struct TableDesc {
public:
    explicit
    TableDesc(String table_name, Vector<SharedPtr<ColumnDef>> columns)
        : columns_(std::move(columns)), table_name_(std::move(table_name)) {
        SizeT column_count = columns_.size();
        for(SizeT idx = 0; idx < column_count; ++ idx) {
            column_name2id_[columns_[idx]->name()] = idx;
        }
    }

    Vector<SharedPtr<ColumnDef>> columns_{};
    HashMap<String, SizeT> column_name2id_{};
    String table_name_{};
    String schema_name_{"default"};
};

}



