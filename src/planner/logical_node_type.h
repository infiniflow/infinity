//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <cstdint>

// Logical operator types used in logical plan
enum class LogicalNodeType : uint8_t {
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
    kDropCollection,
    kDropSchema,
    kDropView,

    // Scan
    kTableScan,
    kViewScan,
    kDummyScan,
    kKnnScan,

    // ?
    kShow,
    kExplain,
    kPrepare,
    kFlush,
};
