//
// Created by JinHai on 2022/9/11.
//

#pragma once

#include "storage/data_type.h"
#include "logical_node.h"

#include <unordered_map>

namespace infinity {

enum class BindingType { Generic, Table };

class Binding {
public:
    Binding() = default;
    virtual ~Binding() = default;

    // Binding type
    BindingType binding_type_{ BindingType::Generic };

    // Binding table alias or name
    std::string table_name_;

    // Node id of the corresponding logical node
    int64_t logical_node_id_;

    // Weak ptr to the corresponding logical node. In general we only need either the weak ptr or the node id.
    std::weak_ptr<LogicalNode> logical_node_ptr_;

    // Binding table index or some place call it tag
    int64_t table_index_;

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

