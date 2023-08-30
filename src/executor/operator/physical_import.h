//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(uint64_t id,
                            SharedPtr<Table> table_ptr,
                            String file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id),
          table_ptr_(std::move(table_ptr)),
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

    inline const SharedPtr<Table>&
    table_ptr() const {
        return table_ptr_;
    }

    inline CopyFileType
    FileType() const {
        return file_type_;
    }

    inline const String&
    file_path() const {
        return file_path_;
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
    CSVHeaderHandler(void *);

    static void
    CSVRowHandler(void *);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    SharedPtr<Table> table_ptr_{};
    CopyFileType file_type_ {CopyFileType::kCSV};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};

};


}