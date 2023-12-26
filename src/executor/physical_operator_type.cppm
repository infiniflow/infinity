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

import stl;

export module physical_operator_type;

namespace infinity {

export enum class PhysicalOperatorType : i8 {
    kInvalid = 0,

    kAggregate,
    kMergeAggregate,

    kParallelAggregate,
    kMergeParallelAggregate,

    kUnionAll,
    kIntersect,
    kExcept,

    kTableScan,
    kFilter,
    kIndexScan,
    kDummyScan,
    kKnnScan,
    kMergeKnn,
    kMatch,
    kFusion,

    kHash,
    kMergeHash,

    // Join
    kJoinHash,
    kJoinNestedLoop,
    kJoinMerge,
    kJoinIndex,
    kCrossProduct,

    // Result modifier
    kLimit,
    kMergeLimit,

    kTop,
    kMergeTop,

    kProjection,

    kSort,
    kMergeSort,

    // DML
    kDelete,
    kUpdate,
    kInsert,
    kImport,
    kExport,

    // DDL
    kAlter,
    kCreateTable,
    kCreateIndex,
    kCreateCollection,
    kCreateDatabase,
    kCreateView,
    kDropTable,
    kDropIndex,
    kDropCollection,
    kDropDatabase,
    kDropView,

    // misc
    kExplain,
    kPreparedPlan,
    kShow,
    kFlush,
    kOptimize,
    kCommand,

    kSink,
    kSource,
};

export String PhysicalOperatorToString(PhysicalOperatorType type);

} // namespace infinity
