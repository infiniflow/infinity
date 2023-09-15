//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "common/types/internal_types.h"

#include <cstdint>


namespace infinity {

enum class PhysicalOperatorType: uint8_t {
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


String
PhysicalOperatorToString(PhysicalOperatorType type);


} // namespace infinity