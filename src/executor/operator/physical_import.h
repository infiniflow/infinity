//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {


class PhysicalImport : public PhysicalOperator {
public:
    explicit
    PhysicalImport(uint64_t id,
                   TableCollectionEntry* table_collection_entry,
                   String file_path,
                   bool header,
                   char delimiter,
                   CopyFileType type)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id),
          table_collection_entry_(table_collection_entry),
          file_path_(std::move(file_path)),
          header_(header),
          delimiter_(delimiter),
          file_type_(type) {}

    ~PhysicalImport() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    void
    ImportCSV(QueryContext* query_context);

    void
    ImportJSON(QueryContext* query_context);

    inline const TableCollectionEntry*
    table_collection_entry() const {
        return table_collection_entry_;
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

    TableCollectionEntry* table_collection_entry_{};
    CopyFileType file_type_ {CopyFileType::kCSV};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};

};


}