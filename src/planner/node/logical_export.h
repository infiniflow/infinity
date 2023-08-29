//
// Created by jinhai on 23-8-29.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalExport : public LogicalNode {
public:
    explicit
    LogicalExport(u64 node_id,
                  String schema_name,
                  String table_name,
                  String file_path,
                  bool header,
                  char delimiter,
                  CopyFileType type)
            : LogicalNode(node_id, LogicalNodeType::kExport),
              schema_name_(std::move(schema_name)),
              table_name_(std::move(table_name)),
              file_path_(std::move(file_path)),
              header_(header),
              delimiter_(delimiter),
              file_type_(type) {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared <Vector<String>>();
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return MakeShared <Vector<SharedPtr<DataType>>>();
    }

    String
    ToString(i64 &space) final;

    inline String
    name() final {
        return "LogicalExport";
    }

    [[nodiscard]] CopyFileType
    FileType() const {
        return file_type_;
    }

    [[nodiscard]] inline const String&
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline const String&
    table_name() const {
        return table_name_;
    }

    [[nodiscard]] inline const String&
    file_path() const {
        return file_path_;
    }

    [[nodiscard]] bool
    header() const {
        return header_;
    }

    [[nodiscard]] char
    delimiter() const {
        return delimiter_;
    }

private:

    CopyFileType file_type_ {CopyFileType::kCSV};
    String file_path_{};
    String table_name_{};
    String schema_name_{"default"};
    bool header_{false};
    char delimiter_{','};
};

}