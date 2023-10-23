//
// Created by xwg on 23-9-8.
//

module;

import stl;
import infinity_assert;
import infinity_exception;

module physical_operator_type;

namespace infinity {
String PhysicalOperatorToString(PhysicalOperatorType type) {
    switch (type) {

        case PhysicalOperatorType::kInvalid:
            return "Invalid";
        case PhysicalOperatorType::kAggregate:
            return "Aggregate";
        case PhysicalOperatorType::kParallelAggregate:
            return "ParallelAggregate";
        case PhysicalOperatorType::kMergeParallelAggregate:
            return "MergeParallelAggregate";
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
        case PhysicalOperatorType::kHash:
            return "Hash";
        case PhysicalOperatorType::kMergeHash:
            return "MergeHash";
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
        case PhysicalOperatorType::kMergeLimit:
            return "MergeLimit";
        case PhysicalOperatorType::kTop:
            return "Top";
        case PhysicalOperatorType::kMergeTop:
            return "MergeTop";
        case PhysicalOperatorType::kProjection:
            return "Projection";
        case PhysicalOperatorType::kSort:
            return "Sort";
        case PhysicalOperatorType::kMergeSort:
            return "MergeSort";
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
        case PhysicalOperatorType::kCreateIndex:
            return "CreateIndex";
        case PhysicalOperatorType::kCreateCollection:
            return "CreateCollection";
        case PhysicalOperatorType::kCreateDatabase:
            return "CreateSchema";
        case PhysicalOperatorType::kCreateView:
            return "CreateView";
        case PhysicalOperatorType::kDropTable:
            return "DropTable";
        case PhysicalOperatorType::kDropCollection:
            return "DropCollection";
        case PhysicalOperatorType::kDropDatabase:
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
        case PhysicalOperatorType::kSource:
            return "Source";
        case PhysicalOperatorType::kIntersect:
            return "Intersect";
        case PhysicalOperatorType::kExcept:
            return "Except";
        case PhysicalOperatorType::kKnnScan:
            return "KnnScan";
        case PhysicalOperatorType::kMergeKnn:
            return "MergeKnn";
    }

    Error<NotImplementException>("Unknown physical operator type", __FILE_NAME__, __LINE__);
}
} // namespace infinity