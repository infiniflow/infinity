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

export module infinity_core:table_def;

import :index_base;

import column_def;
import global_resource_usage;

namespace infinity {

export class TableDef {

public:
    static inline std::shared_ptr<TableDef> Make(std::shared_ptr<std::string> schema,
                                                 std::shared_ptr<std::string> table_name,
                                                 std::shared_ptr<std::string> table_comment,
                                                 std::vector<std::shared_ptr<ColumnDef>> columns) {
        return std::make_shared<TableDef>(std::move(schema), std::move(table_name), std::move(table_comment), std::move(columns));
    }

public:
    explicit TableDef(std::shared_ptr<std::string> schema,
                      std::shared_ptr<std::string> table_name,
                      std::shared_ptr<std::string> table_comment,
                      std::vector<std::shared_ptr<ColumnDef>> columns)
        : schema_name_(std::move(schema)), table_name_(std::move(table_name)), table_comment_(std::move(table_comment)),
          columns_(std::move(columns)) {
        size_t column_count = columns_.size();
        for (size_t idx = 0; idx < column_count; ++idx) {
            column_name2id_[columns_[idx]->name()] = idx;
        }

#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BaseResult");
#endif
    }

    virtual ~TableDef() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BaseResult");
#endif
    }

    bool operator==(const TableDef &other) const;

    bool operator!=(const TableDef &other) const;

    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static std::shared_ptr<TableDef> ReadAdv(const char *&ptr, i32 maxbytes);

    [[nodiscard]] inline const std::vector<std::shared_ptr<ColumnDef>> &columns() const { return columns_; }

    [[nodiscard]] inline std::vector<std::shared_ptr<ColumnDef>> &columns() { return columns_; }

    [[nodiscard]] inline size_t column_count() const { return columns_.size(); }

    [[nodiscard]] inline const std::shared_ptr<std::string> &table_name() const { return table_name_; }

    [[nodiscard]] inline const std::shared_ptr<std::string> &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const std::shared_ptr<std::string> &table_comment() const { return table_comment_; }

    [[nodiscard]] inline size_t GetColIdByName(const std::string &name) const {
        if (column_name2id_.contains(name)) {
            return column_name2id_.at(name);
        } else {
            return -1;
        }
    }

    std::string ToString() const;

    void UnionWith(const std::shared_ptr<TableDef> &other);

private:
    std::shared_ptr<std::string> schema_name_{};
    std::shared_ptr<std::string> table_name_{};
    std::shared_ptr<std::string> table_comment_{};
    std::vector<std::shared_ptr<ColumnDef>> columns_{};
    std::unordered_map<std::string, size_t> column_name2id_{};
};

} // namespace infinity
