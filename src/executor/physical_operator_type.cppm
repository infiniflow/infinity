//
// Created by JinHai on 2022/7/28.
//

module;

import stl;

export module physical_operator_type;

namespace infinity {

export enum class PhysicalOperatorType : i8 {
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
    kDropIndex,
    kDropCollection,
    kDropDatabase,
    kDropView,

    // misc
    kExplain,
    kPreparedPlan,
    kShow,
    kFlush,
    kCommand,

    kSink,
    kSource,
};

export String PhysicalOperatorToString(PhysicalOperatorType type);

} // namespace infinity
