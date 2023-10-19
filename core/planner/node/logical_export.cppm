//
// Created by jinhai on 23-8-29.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_export;

namespace infinity {

export class LogicalExport : public LogicalNode {
public:
    explicit LogicalExport(u64 node_id, String schema_name, String table_name, String file_path, bool header, char delimiter, CopyFileType type)
        : LogicalNode(node_id, LogicalNodeType::kExport), schema_name_(Move(schema_name)), table_name_(Move(table_name)),
          file_path_(Move(file_path)), header_(header), delimiter_(delimiter), file_type_(type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalExport"; }

    [[nodiscard]] CopyFileType FileType() const { return file_type_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &table_name() const { return table_name_; }

    [[nodiscard]] inline const String &file_path() const { return file_path_; }

    [[nodiscard]] bool header() const { return header_; }

    [[nodiscard]] char delimiter() const { return delimiter_; }

private:
    CopyFileType file_type_{CopyFileType::kCSV};
    String file_path_{};
    String table_name_{};
    String schema_name_{"default"};
    bool header_{false};
    char delimiter_{','};
};

} // namespace infinity