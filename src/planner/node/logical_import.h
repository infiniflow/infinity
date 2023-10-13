//
// Created by jinhai on 23-8-27.
//

#pragma once

#include "planner/logical_node.h"
#include "parser/statement/statement_common.h"

namespace infinity {

class TableCollectionEntry;

class LogicalImport : public LogicalNode {
public:
    explicit
    LogicalImport(u64 node_id, TableCollectionEntry *table_collection_entry, String file_path, bool header, char delimiter, CopyFileType type)
        : LogicalNode(node_id, LogicalNodeType::kImport), table_collection_entry_(table_collection_entry), file_path_(std::move(file_path)),
          header_(header), delimiter_(delimiter), file_type_(type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalImport"; }

    inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline TableCollectionEntry *table_collection_entry() { return table_collection_entry_; }

    [[nodiscard]] inline CopyFileType FileType() const { return file_type_; }

    [[nodiscard]] inline const String &file_path() const { return file_path_; }

    [[nodiscard]] bool header() const { return header_; }

    [[nodiscard]] char delimiter() const { return delimiter_; }

private:
    TableCollectionEntry *table_collection_entry_{};
    CopyFileType file_type_{CopyFileType::kCSV};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};
};

} // namespace infinity
