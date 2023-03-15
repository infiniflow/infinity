//
// Created by jinhai on 23-3-14.
//

#include "explain_logical_plan.h"

namespace infinity {

void
ExplainLogicalPlan::Explain(const LogicalNode *statement,
                            SharedPtr<Vector<SharedPtr<String>>> &result,
                            i64 intent_size) {
    switch(statement->operator_type()) {
        case LogicalNodeType::kAggregate:
            break;
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion:
            break;
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin:
            break;
        case LogicalNodeType::kCrossProduct:
            break;
        case LogicalNodeType::kLimit:
            break;
        case LogicalNodeType::kFilter:
            break;
        case LogicalNodeType::kProjection:
            break;
        case LogicalNodeType::kSort:
            break;
        case LogicalNodeType::kDelete:
            break;
        case LogicalNodeType::kUpdate:
            break;
        case LogicalNodeType::kInsert:
            break;
        case LogicalNodeType::kImport:
            break;
        case LogicalNodeType::kExport:
            break;
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable: {
            Explain((LogicalCreateTable*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            Explain((LogicalCreateCollection*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            Explain((LogicalCreateSchema*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateView:
            break;
        case LogicalNodeType::kDropTable: {
            Explain((LogicalDropTable*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            Explain((LogicalDropCollection*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            Explain((LogicalDropSchema*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropView:
            break;
        case LogicalNodeType::kChunkScan:
            break;
        case LogicalNodeType::kTableScan:
            break;
        case LogicalNodeType::kViewScan:
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kExplain:
            break;
        case LogicalNodeType::kPrepare:
            break;
        default: {
            PlannerError("Unexpect logical node type");
        }
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCreateSchema* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE SCHEMA: ";
    } else {
        create_str = "CREATE SCHEMA: ";
    }

    create_str += *create_node->schema_name()  + "conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));
}

void
ExplainLogicalPlan::Explain(const LogicalCreateTable* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE TABLE: ";
    } else {
        create_str = "CREATE TABLE: ";
    }

    create_str += *create_node->schema_name() + "."
               + create_node->table_definitions()->name()
               + " conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));

    SizeT column_count = create_node->table_definitions()->column_count();
    if(column_count == 0) {
        PlannerError("No columns in the table");
    }
    const Vector<SharedPtr<ColumnDef>>& columns = create_node->table_definitions()->columns();

    intent_size += 2;
    String columns_str = String(intent_size, ' ') + "Columns: ";
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        columns_str += columns[idx]->ToString() + ", ";
    }
    columns_str += columns.back()->ToString();
    result->emplace_back(MakeShared<String>(columns_str));
}

void
ExplainLogicalPlan::Explain(const LogicalCreateCollection* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE COLLECTION: ";
    } else {
        create_str = "CREATE COLLECTION: ";
    }

    create_str += *create_node->schema_name() + "." + *create_node->collection_name()
                + " conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropSchema* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP SCHEMA: ";
    } else {
        drop_str = "DROP SCHEMA: ";
    }

    drop_str += *drop_node->schema_name()  + "conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropTable* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP TABLE: ";
    } else {
        drop_str = "DROP TABLE: ";
    }

    drop_str += *drop_node->schema_name() + "." + *drop_node->table_name()
             + " conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropCollection* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP COLLECTION: ";
    } else {
        drop_str = "DROP COLLECTION: ";
    }

    drop_str += *drop_node->schema_name() + "." + *drop_node->collection_name()
                + " conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

}
