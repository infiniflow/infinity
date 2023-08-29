//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(uint64_t id,
                            String schema_name,
                            String table_name,
                            String file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id),
          schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)),
          file_path_(std::move(file_path)),
          header_(header),
          delimiter_(delimiter),
          file_type_(type) {}

    ~PhysicalImport() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    void
    ImportCSV(SharedPtr<QueryContext>& query_context);

    void
    ImportJSON(SharedPtr<QueryContext>& query_context);

    inline CopyFileType
    FileType() const {
        return file_type_;
    }

    inline const String&
    file_path() const {
        return file_path_;
    }

    inline const String&
    schema_name() const {
        return schema_name_;
    }

    inline const String&
    table_name() const {
        return table_name_;
    }

    inline bool
    header() const {
        return header_;
    }

    inline char
    delimiter() const {
        return delimiter_;
    }

public:
    static void
    CSVRowHandler(void *);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    CopyFileType file_type_ {CopyFileType::kCSV};
    String file_path_{};
    String table_name_{};
    String schema_name_{"default"};
    bool header_{false};
    char delimiter_{','};

};


}