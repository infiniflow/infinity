//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <cstdint>

enum class PhysicalOperatorType: uint8_t {
    kInvalid = 0,
    kAggregate,
    kUnionAll,
    kTableScan,
    kFilter,
    kIndexScan,
    kDummyScan,

    // Join
    kJoinHash,
    kJoinNestedLoop,
    kJoinMerge,
    kJoinIndex,
    kCrossProduct,

    // Result modifier
    kLimit,
    kTop,
    kProjection,
    kSort,

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
    kCreateSchema,
    kCreateView,
    kDropTable,
    kDropCollection,
    kDropSchema,
    kDropView,

    // misc
    kExplain,
    kPreparedPlan,
    kShow,
    kFlush,

    kSink,
    kExchange,
};
