//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"

namespace infinity {

enum class PhysicalOperatorType : i8 {
    kInvalid = 0,

    kAggregate,

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
    kDropCollection,
    kDropDatabase,
    kDropView,

    // misc
    kExplain,
    kPreparedPlan,
    kShow,
    kFlush,

    kSink,
    kSource,
};

String PhysicalOperatorToString(PhysicalOperatorType type);

} // namespace infinity