//
// Created by JinHai on 2022/9/11.
//

#pragma once

#include "storage/data_type.h"
#include "storage/table.h"
#include "logical_node.h"

#include <unordered_map>

namespace infinity {

enum class BindingType { kInvalid, kTable, kSubquery, kCTE, kView };

class Binding {
public:
    Binding() = default;
    virtual ~Binding() = default;

    static std::shared_ptr<Binding>
    MakeBinding(BindingType binding_type, const std::string& name,
                const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    static std::shared_ptr<Binding>
    MakeBinding(BindingType binding_type, const std::string& name, std::shared_ptr<Table> table_ptr,
                std::shared_ptr<LogicalNode> logical_node_ptr, int64_t logical_node_id,
                const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    // Binding type
    BindingType binding_type_{ BindingType::kInvalid };

    // Binding table alias or name
    std::string table_name_;

    // Node id of the corresponding logical node
    int64_t logical_node_id_{-1};

    // ptr to the corresponding logical node.
    std::shared_ptr<LogicalNode> logical_node_ptr_{nullptr};

    // if the binding is table, this is the table_ptr
    std::shared_ptr<Table> table_ptr_{nullptr};

    // Column types of the binding
    std::vector<LogicalType> column_types_;

    // Column names of the binding
    std::vector<std::string> column_names_;

    // Column name -> column index mapping
    std::unordered_map<std::string, int64_t> name2index_;
};

//class TableBinding: public GenericBinding {
//public:
//
//};

}

