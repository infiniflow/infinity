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

import stl;
import parser;
import index_def;

export module table_def;


namespace infinity {

export class TableDef {

public:
    static inline SharedPtr<TableDef> Make(SharedPtr<String> schema, SharedPtr<String> table_name, Vector<SharedPtr<ColumnDef>> columns) {
        return MakeShared<TableDef>(Move(schema), Move(table_name), Move(columns));
    }

public:
    explicit TableDef(SharedPtr<String> schema, SharedPtr<String> table_name, Vector<SharedPtr<ColumnDef>> columns)
            : schema_name_(Move(schema)), table_name_(Move(table_name)), columns_(Move(columns)) {
        SizeT column_count = columns_.size();
        for (SizeT idx = 0; idx < column_count; ++idx) {
            column_name2id_[columns_[idx]->name()] = idx;
        }
    }

    bool operator==(const TableDef &other) const;

    bool operator!=(const TableDef &other) const;

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<TableDef> ReadAdv(char *&ptr, i32 maxbytes);

    [[nodiscard]] inline const Vector<SharedPtr<ColumnDef>> &columns() const { return columns_; }

    [[nodiscard]] inline SizeT column_count() const { return columns_.size(); }

    [[nodiscard]] inline const SharedPtr<String> &table_name() const { return table_name_; }

    [[nodiscard]] inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline SizeT GetColIdByName(const String &name) const {
        if (column_name2id_.contains(name)) {
            return column_name2id_.at(name);
        } else {
            return -1;
        }
    }

    String ToString() const;

    void UnionWith(const SharedPtr<TableDef> &other);

private:
    Vector<SharedPtr<ColumnDef>> columns_{};
    HashMap<String, SizeT> column_name2id_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> schema_name_{};
    Vector<IndexDef> indexes_{};
};

} // namespace infinity
