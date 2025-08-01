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

export module infinity_core:physical_operator_type;

import :stl;

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
    kUnnest,
    kUnnestAggregate,
    kIndexScan,
    kDummyScan,
    kKnnScan,
    kMergeKnn,
    kMatchTensorScan,
    kMergeMatchTensor,
    kMatchSparseScan,
    kMergeMatchSparse,
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
    kCreateCollection,
    kCreateDatabase,
    kCreateView,
    kDropTable,
    kDropIndex,
    kDropCollection,
    kDropDatabase,
    kDropView,

    kCreateIndexPrepare,

    // misc
    kExplain,
    kPreparedPlan,
    kShow,
    kFlush,
    kOptimize,
    kCommand,

    // Compact
    kCompact,

    kReadCache,

    kSink,
    kSource,

    kCheck,
};

export String PhysicalOperatorToString(PhysicalOperatorType type);

} // namespace infinity
