//
// Created by xwg on 23-9-8.
//


#include "executor/physical_operator_type.h"
#include "common/types/internal_types.h"
#include "common/utility/exception.h"


namespace infinity{
String
PhysicalOperatorToString(PhysicalOperatorType type){
    switch (type) {

        case PhysicalOperatorType::kInvalid:
            return "Invalid";
        case PhysicalOperatorType::kAggregate:
            return "Aggregate";
        case PhysicalOperatorType::kUnionAll:
            return "UnionAll";
        case PhysicalOperatorType::kTableScan:
            return "TableScan";
        case PhysicalOperatorType::kFilter:
            return "Filter";
        case PhysicalOperatorType::kIndexScan:
            return "IndexScan";
        case PhysicalOperatorType::kDummyScan:
            return "DummyScan";
        case PhysicalOperatorType::kJoinHash:
            return "JoinHash";
        case PhysicalOperatorType::kJoinNestedLoop:
            return "JoinNestedLoop";
        case PhysicalOperatorType::kJoinMerge:
            return "JoinMerge";
        case PhysicalOperatorType::kJoinIndex:
            return "JoinIndex";
        case PhysicalOperatorType::kCrossProduct:
            return "CrossProduct";
        case PhysicalOperatorType::kLimit:
            return "Limit";
        case PhysicalOperatorType::kTop:
            return "Top";
        case PhysicalOperatorType::kProjection:
            return "Projection";
        case PhysicalOperatorType::kSort:
            return "Sort";
        case PhysicalOperatorType::kDelete:
            return "Delete";
        case PhysicalOperatorType::kUpdate:
            return "Update";
        case PhysicalOperatorType::kInsert:
            return "Insert";
        case PhysicalOperatorType::kImport:
            return "Import";
        case PhysicalOperatorType::kExport:
            return "Export";
        case PhysicalOperatorType::kAlter:
            return "Alter";
        case PhysicalOperatorType::kCreateTable:
            return "CreateTable";
        case PhysicalOperatorType::kCreateCollection:
            return "CreateCollection";
        case PhysicalOperatorType::kCreateSchema:
            return "CreateSchema";
        case PhysicalOperatorType::kCreateView:
            return "CreateView";
        case PhysicalOperatorType::kDropTable:
            return "DropTable";
        case PhysicalOperatorType::kDropCollection:
            return "DropCollection";
        case PhysicalOperatorType::kDropSchema:
            return "DropSchema";
        case PhysicalOperatorType::kDropView:
            return "DropView";
        case PhysicalOperatorType::kExplain:
            return "Explain";
        case PhysicalOperatorType::kPreparedPlan:
            return "PreparedPlan";
        case PhysicalOperatorType::kShow:
            return "Show";
        case PhysicalOperatorType::kFlush:
            return "Flush";
        case PhysicalOperatorType::kSink:
            return "Sink";
        case PhysicalOperatorType::kExchange:
            return "Exchange";
    }
    throw NotImplementException("Unknown physical operator type");
}
}