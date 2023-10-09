//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "table.h"
#include "view.h"

#include <memory>
#include <unordered_map>

namespace infinity {

class Schema {
public:
    Schema(String name, u64 id) : name_(std::move(name)), schema_id_(id){};

    // Table related functions
    SharedPtr<BaseTable> GetTableByName(const String &name);

    void AddTable(const SharedPtr<BaseTable> &table_def, ConflictType conflict_type);

    void DeleteTable(const String &name, ConflictType conflict_type);

    // View related functions
    SharedPtr<View> GetViewByName(const String &name);

    void AddView(const SharedPtr<View> &view, ConflictType conflict_type);

    void DeleteView(const String &name, ConflictType conflict_type);

    [[nodiscard]] inline u64 schema_id() const { return schema_id_; };

    inline HashMap<String, SharedPtr<BaseTable>> &tables() { return tables_; }

    inline HashMap<String, SharedPtr<View>> &views() { return views_; }

private:
    HashMap<String, SharedPtr<BaseTable>> tables_;
    HashMap<String, SharedPtr<View>> views_;
    String name_;
    u64 id_counter_{0};
    u64 schema_id_{0};
};

} // namespace infinity