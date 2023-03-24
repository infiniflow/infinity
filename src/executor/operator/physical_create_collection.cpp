//
// Created by jinhai on 23-3-16.
//

#include "physical_create_collection.h"
#include "main/infinity.h"

namespace infinity {

PhysicalCreateCollection::PhysicalCreateCollection(SharedPtr<String> schema_name,
                                                   SharedPtr<String> collection_name,
                                                   ConflictType conflict_type,
                                                   u64 table_index,
                                                   u64 id):
                                                   PhysicalOperator(PhysicalOperatorType::kCreateCollection,
                                                                    nullptr,
                                                                    nullptr,
                                                                    id),
                                                   schema_name_(std::move(schema_name)),
                                                   collection_name_(std::move(collection_name)),
                                                   conflict_type_(conflict_type),
                                                   table_index_(table_index)
                                                   {}

void
PhysicalCreateCollection::Init() {

}

void
PhysicalCreateCollection::Execute(SharedPtr<QueryContext>& query_context) {
    Infinity::instance().catalog()->CreateCollection(*schema_name_, *collection_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}