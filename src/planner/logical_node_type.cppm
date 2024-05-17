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

export module logical_node_type;

namespace infinity {

// Logical operator types used in logical plan
export enum class LogicalNodeType : u8 {
    kInvalid = 0,
    kAggregate,
    kExcept,
    kUnion,
    kIntersect,
    kJoin,
    kCrossProduct,
    kLimit,
    kFilter,
    kProjection,
    kSort,
    kTop,

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
    kCreateSchema,
    kCreateView,
    kDropTable,
    kDropIndex,
    kDropCollection,
    kDropSchema,
    kDropView,

    // Search
    kFusion,
    kMatch,

    // Scan
    kTableScan,
    kIndexScan,
    kViewScan,
    kDummyScan,
    kKnnScan,
    kMatchTensorScan,

    // ?
    kShow,
    kExplain,
    kCommand,
    kPrepare,
    kFlush,
    kOptimize,

    // Compact
    kCompact,
    kCompactIndex,
    kCompactFinish,
};
}
