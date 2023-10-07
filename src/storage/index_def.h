#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "parser/statement/extra/create_index_info.h"
namespace infinity {

class IndexDef {
public:
    static inline SharedPtr<IndexDef>
    Make(SharedPtr<String> schema_name,
         SharedPtr<String> table_name,
         SharedPtr<Vector<String>> column_names,
         SharedPtr<String> index_name,
         IndexMethod method_type) {
        return MakeShared<IndexDef>(std::move(schema_name),
                                    std::move(table_name),
                                    std::move(column_names),
                                    std::move(index_name),
                                    method_type);
    }

    explicit IndexDef(SharedPtr<String> schema_name,
                      SharedPtr<String> table_name,
                      SharedPtr<Vector<String>> column_names,
                      SharedPtr<String> index_name,
                      IndexMethod method_type)
        : schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)),
          column_names_(std::move(column_names)),
          index_name_(std::move(index_name)),
          method_type_(method_type) {
    }

    String
    ToString() const;

public:
    [[nodiscard]] inline const SharedPtr<String>&
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline const SharedPtr<String>&
    table_name() const {
        return table_name_;
    }

    [[nodiscard]] inline const SharedPtr<Vector<String>>&
    column_names() const {
        return column_names_;
    }

    [[nodiscard]] inline const SharedPtr<String>&
    index_name() const {
        return index_name_;
    }

    [[nodiscard]] inline IndexMethod
    method_type() const {
        return method_type_;
    }

private:
    SharedPtr<String> schema_name_;
    SharedPtr<String> table_name_;
    SharedPtr<Vector<String>> column_names_;
    SharedPtr<String> index_name_;
    IndexMethod method_type_;
};
}// namespace infinity