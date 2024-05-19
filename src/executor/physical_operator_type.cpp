// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

module physical_operator_type;

import stl;
import status;
import infinity_exception;
import logger;

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
        case PhysicalOperatorType::kCreateCollection:
            return "CreateCollection";
        case PhysicalOperatorType::kCreateDatabase:
            return "CreateSchema";
        case PhysicalOperatorType::kCreateView:
            return "CreateView";
        case PhysicalOperatorType::kDropTable:
            return "DropTable";
        case PhysicalOperatorType::kDropIndex:
            return "DropIndex";
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
        case PhysicalOperatorType::kOptimize:
            return "Optimize";
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
        case PhysicalOperatorType::kCommand:
            return "Command";
        case PhysicalOperatorType::kCompact:
            return "Compact";
        case PhysicalOperatorType::kCompactIndexPrepare:
            return "CompactIndexPrepare";
        case PhysicalOperatorType::kCompactIndexDo:
            return "CompactIndexDo";
        case PhysicalOperatorType::kCompactFinish:
            return "CompactFinish";
        case PhysicalOperatorType::kMatch:
            return "Match";
        case PhysicalOperatorType::kMatchTensorScan:
            return "MatchTensorScan";
        case PhysicalOperatorType::kMergeMatchTensor:
            return "MergeMatchTensor";
        case PhysicalOperatorType::kFusion:
            return "Fusion";
        case PhysicalOperatorType::kMergeAggregate:
            return "MergeAggregate";
        case PhysicalOperatorType::kCreateIndexPrepare:
            return "CreateIndexPrepare";
        case PhysicalOperatorType::kCreateIndexDo:
            return "CreateIndexDo";
        case PhysicalOperatorType::kCreateIndexFinish:
            return "CreateIndexFinish";
    }

    Status status = Status::NotSupport("Unknown physical operator type");
    LOG_ERROR(status.message());
    RecoverableError(status);
}
} // namespace infinity
