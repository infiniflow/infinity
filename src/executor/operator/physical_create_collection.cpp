//
// Created by jinhai on 23-3-16.
//

#include "physical_create_collection.h"

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
    NotImplementError("Physical create collection execution")
}

}