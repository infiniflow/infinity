//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <cstdint>

// Logical operator types used in logical plan
enum class LogicalOperatorType: uint8_t {
    kInvalid = 0,
    kAggregate,
    kExcept,
    kUnion,
    kIntersect,
    kJoin,
    kLimit,
    kPredicate,
    kProjection,
    kSort,

    // DML
    kDelete,
    kUpdate,
    kInsert,
    kImport,
    kExport,

    // DDL
    kAlter = 3,
    kCreateTable = 4,
    kCreateView = 5,
    kDropTable = 6,
    kDropView = 7,

    // ?
    kAlias,
    kMock,
    kValidate,
    kStoredTable,
    kStaticTable,
    kRoot,
    kDummyTable,
    kPrepare,
};
