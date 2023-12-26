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
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;
import infinity_exception;

export module physical_export;

namespace infinity {

export class PhysicalExport : public PhysicalOperator {
public:
    explicit PhysicalExport(u64 id,
                            String schema_name,
                            String table_name,
                            String file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kExport, nullptr, nullptr, id, load_metas), file_type_(type), file_path_(Move(file_path)),
          table_name_(Move(table_name)), schema_name_(Move(schema_name)), header_(header), delimiter_(delimiter) {}

    ~PhysicalExport() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    void ExportCSV(QueryContext *query_context);

    void ExportJSON(QueryContext *query_context);

    inline CopyFileType FileType() const { return file_type_; }

    inline const String &file_path() const { return file_path_; }

    inline const String &schema_name() const { return schema_name_; }

    inline const String &table_name() const { return table_name_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    CopyFileType file_type_{CopyFileType::kCSV};
    String file_path_{};
    String table_name_{};
    String schema_name_{"default"};
    bool header_{false};
    char delimiter_{','};
};

} // namespace infinity
