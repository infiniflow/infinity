//
// Created by JinHai on 2022/7/28.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;

export module physical_export;

namespace infinity {

export class PhysicalExport : public PhysicalOperator {
public:
    explicit PhysicalExport(u64 id, String schema_name, String table_name, String file_path, bool header, char delimiter, CopyFileType type)
        : PhysicalOperator(PhysicalOperatorType::kExport, nullptr, nullptr, id), schema_name_(Move(schema_name)),
          table_name_(Move(table_name)), file_path_(Move(file_path)), header_(header), delimiter_(delimiter), file_type_(type) {}

    ~PhysicalExport() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

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
