//
// Created by JinHai on 2022/9/11.
//

module;

import stl;
import infinity_assert;
import infinity_exception;

module binding;

namespace infinity {

SharedPtr<Binding> Binding::MakeBinding(BindingType binding_type,
                                        const String &name,
                                        u64 table_index,
                                        SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                                        SharedPtr<Vector<String>> column_names) {
    SharedPtr<Binding> binding = MakeShared<Binding>();
    binding->binding_type_ = binding_type;
    binding->table_name_ = name;
    binding->table_index_ = table_index;

    SizeT column_count = column_names->size();
    Assert<PlannerException>(column_count == column_types->size(), "Make binding error: column size isn't valid.", __FILE_NAME__, __LINE__);
    binding->column_types_ = Move(column_types);
    binding->column_names_ = Move(column_names);

    for (SizeT idx = 0; idx < column_count; ++idx) {
        binding->name2index_[binding->column_names_->at(idx)] = static_cast<i64>(idx);
    }
    return binding;
}

SharedPtr<Binding> Binding::MakeBinding(BindingType binding_type,
                                        const String &binding_alias,
                                        u64 table_index,
                                        TableCollectionEntry *table_ptr,
                                        SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                                        SharedPtr<Vector<String>> column_names,
                                        SharedPtr<BlockIndex> block_index) {
    auto binding = MakeBinding(binding_type, binding_alias, table_index, Move(column_types), Move(column_names));
    binding->table_collection_entry_ptr_ = table_ptr;
    binding->block_index_ = Move(block_index);
    //    binding->logical_node_ptr_ = Move(logical_node_ptr);
    //    binding->logical_node_id_ = logical_node_id;
    return binding;
}

} // namespace infinity