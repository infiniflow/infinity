//
// Created by JinHai on 2022/7/23.
//

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

}
