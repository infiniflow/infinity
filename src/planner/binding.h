//
// Created by JinHai on 2022/9/11.
//

#pragma once

#include "storage/meta/entry/table_collection_entry.h"
#include "storage/meta/entry/view_entry.h"

#include <unordered_map>

namespace infinity {

enum class BindingType { kInvalid, kTable, kSubquery, kCTE, kView };

class Binding {
public:
    Binding() = default;

    virtual
    ~Binding() = default;

    static SharedPtr<Binding>
    MakeBinding(BindingType binding_type,
                const String& name,
                u64 table_index,
                SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                SharedPtr<Vector<String>> column_names);

    static SharedPtr<Binding>
    MakeBinding(BindingType binding_type,
                const String& name,
                u64 table_index,
                TableCollectionEntry* table_collection_entry_ptr,
                SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                SharedPtr<Vector<String>> column_names);

    // Binding type
    BindingType binding_type_{ BindingType::kInvalid };

    // Binding table alias or name
    String table_name_{};

    u64 table_index_{};

    // Node id of the corresponding logical node
//    i64 logical_node_id_{-1};

    // ptr to the corresponding logical node.
//    SharedPtr<LogicalNode> logical_node_ptr_{nullptr};

    // if the binding is table, this is the table_ptr
    TableCollectionEntry* table_collection_entry_ptr_{nullptr};

    // Column types of the binding
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};

    // Column names of the binding
    SharedPtr<Vector<String>> column_names_{};

    // Column name -> column index mapping
    HashMap<String, i64> name2index_{};
};

//class TableBinding: public GenericBinding {
//public:
//
//};

}

