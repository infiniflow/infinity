#include "physical_create_index.h"
#include "parser/statement/extra/create_table_info.h"

namespace infinity {
void
PhysicalCreateIndex::Init() {}

void
PhysicalCreateIndex::Execute(QueryContext* query_context) {
    auto txn = query_context->GetTxn();
    // TODO shenyushi 1
    txn->CreateIndex(*schema_name_, conflict_type_);

    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};
    output_ = Table::MakeResultTable(column_defs);
}

void
PhysicalCreateIndex::Execute(QueryContext* query_context,
                             InputState* input_state,
                             OutputState* output_state) {
    auto txn = query_context->GetTxn();
    // TODO shenyushi 1
    auto result = txn->CreateIndex(*schema_name_, conflict_type_);

    auto create_table_output_state = (CreateTableOutputState*)output_state;
    if(result.err_ != nullptr) {
        create_table_output_state->error_message_ = std::move(result.err_);
    }
}
// SharedPtr<Vector<String>>
// PhysicalCreateIndex::GetOutputNames() const {}

// SharedPtr<Vector<SharedPtr<DataType>>>
// PhysicalCreateIndex::GetOutputTypes() const {}

PhysicalCreateIndex::PhysicalCreateIndex(SharedPtr<String> schema_name,
                                         SharedPtr<IndexDef> index_definition,
                                         ConflictType conflict_type,

                                         SharedPtr<Vector<String>> output_names,
                                         SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                         u64 id)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndex, nullptr, nullptr, id),
      schema_name_(std::move(schema_name)),
      index_definition_(std::move(index_definition)),
      conflict_type_(conflict_type),
      output_names_(std::move(output_names)),
      output_types_(std::move(output_types)) {
}

}// namespace infinity