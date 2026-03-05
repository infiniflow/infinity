module infinity_core:physical_show_catalog.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowCatalog(QueryContext *query_context, ShowOperatorState *operator_state) {

    auto *new_catalog = query_context->storage()->new_catalog();
    auto meta_tree_ptr = new_catalog->MakeMetaTree();
    auto meta_json = meta_tree_ptr->ToJson();

    constexpr int json_intent = 4;
    auto meta_str = meta_json.dump(json_intent);

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    {
        Value value = Value::MakeVarchar(meta_str);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
    }
    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}