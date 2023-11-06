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