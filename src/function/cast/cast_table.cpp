//
// Created by jinhai on 23-1-5.
//

#include "cast_table.h"

namespace infinity {

CastTable::CastTable() {
    for (i8 i = LogicalType::kBoolean; i < LogicalType::kInvalid; ++i) {
        for (i8 j = LogicalType::kBoolean; j < LogicalType::kInvalid; ++j) {
            matrix_[i][j] = -1;
        }
    }

    // From boolean to other type
    matrix_[kBoolean][kBoolean] = 0;
    matrix_[kBoolean][kVarchar] = 1;

    // From tinyint to other type
    matrix_[kTinyInt][kTinyInt] = 0;
    matrix_[kTinyInt][kSmallInt] = 1;
    matrix_[kTinyInt][kInteger] = 2;
    matrix_[kTinyInt][kBigInt] = 3;
    matrix_[kTinyInt][kHugeInt] = 4;
    matrix_[kTinyInt][kDecimal] = 8;
    matrix_[kTinyInt][kFloat] = 9;
    matrix_[kTinyInt][kDouble] = 10;
    matrix_[kTinyInt][kVarchar] = 12;

    // From smallint to other type
    matrix_[kSmallInt][kSmallInt] = 0;
    matrix_[kSmallInt][kInteger] = 1;
    matrix_[kSmallInt][kBigInt] = 2;
    matrix_[kSmallInt][kHugeInt] = 3;
    matrix_[kSmallInt][kDecimal] = 7;
    matrix_[kSmallInt][kFloat] = 8;
    matrix_[kSmallInt][kDouble] = 9;
    matrix_[kSmallInt][kVarchar] = 11;

    // From integer to other type
    matrix_[kInteger][kInteger] = 0;
    matrix_[kInteger][kBigInt] = 1;
    matrix_[kInteger][kHugeInt] = 2;
    matrix_[kInteger][kDecimal] = 6;
    matrix_[kInteger][kFloat] = 7;
    matrix_[kInteger][kDouble] = 8;
    matrix_[kInteger][kVarchar] = 10;

    // From big int to other type
    matrix_[kBigInt][kBigInt] = 0;
    matrix_[kBigInt][kHugeInt] = 1;
    matrix_[kBigInt][kDecimal] = 5;
    matrix_[kBigInt][kFloat] = 6;
    matrix_[kBigInt][kDouble] = 7;
    matrix_[kBigInt][kVarchar] = 9;

    // From huge int to other type
    matrix_[kHugeInt][kHugeInt] = 0;
    matrix_[kHugeInt][kDecimal] = 4;
    matrix_[kHugeInt][kFloat] = 5;
    matrix_[kHugeInt][kDouble] = 6;
    matrix_[kHugeInt][kVarchar] = 8;

    // From decimal128 to other type
    matrix_[kDecimal][kDecimal] = 0;
    matrix_[kDecimal][kFloat] = 1;
    matrix_[kDecimal][kDouble] = 2;
    matrix_[kDecimal][kVarchar] = 4;

    // From float to other type
    matrix_[kFloat][kFloat] = 0;
    matrix_[kFloat][kDouble] = 1;
    matrix_[kFloat][kVarchar] = 3;

    // From double to other type
    matrix_[kDouble][kDouble] = 0;
    matrix_[kDouble][kVarchar] = 2;

    // From date to other type
    matrix_[kDate][kDate] = 0;
    matrix_[kDate][kDateTime] = 1;
    matrix_[kDate][kTimestamp] = 2;
    matrix_[kDate][kVarchar] = 5;

    // From time to other type
    matrix_[kTime][kTime] = 0;
    matrix_[kTime][kVarchar] = 2;

    // From datetime to other type
    matrix_[kDateTime][kDateTime] = 0;
    matrix_[kDateTime][kTimestamp] = 1;
    matrix_[kDateTime][kTime] = 2;
    matrix_[kDateTime][kDate] = 2;
    matrix_[kDateTime][kVarchar] = 3;

    // From timestamp to other type
    matrix_[kTimestamp][kTimestamp] = 0;
    matrix_[kTimestamp][kDateTime] = 1;
    matrix_[kTimestamp][kTime] = 2;
    matrix_[kTimestamp][kDate] = 2;
    matrix_[kTimestamp][kVarchar] = 3;

    // From interval to other type
    matrix_[kInterval][kInterval] = 0;
    matrix_[kInterval][kVarchar] = 1;

    // From array to other type
    // From tuple to other type

    // From point to other type
    matrix_[kPoint][kPoint] = 0;
    matrix_[kPoint][kVarchar] = 1;

    // From line to other type
    matrix_[kLine][kLine] = 0;
    matrix_[kLine][kVarchar] = 1;

    // From line segment to other type
    matrix_[kLineSeg][kLineSeg] = 0;
    matrix_[kLineSeg][kVarchar] = 1;

    // From box to other type
    matrix_[kBox][kBox] = 0;
    matrix_[kBox][kVarchar] = 1;

    // From path to other type
    matrix_[kPath][kPath] = 0;
    matrix_[kPath][kVarchar] = 1;

    // From polygon to other type
    matrix_[kPolygon][kPolygon] = 0;
    matrix_[kPolygon][kVarchar] = 1;

    // From circle to other type
    matrix_[kCircle][kCircle] = 0;
    matrix_[kCircle][kVarchar] = 1;

    // From bitmap to other type
    matrix_[kBitmap][kBitmap] = 0;
    matrix_[kBitmap][kVarchar] = 1;

    // From uuid to other type
    matrix_[kUuid][kUuid] = 0;
    matrix_[kUuid][kVarchar] = 1;

    // From cast blob to other type
    matrix_[kBlob][kBlob] = 0;

    // From embedding to other type
    matrix_[kEmbedding][kEmbedding] = 0;
    matrix_[kEmbedding][kVarchar] = 1;

    // From row_id to other type
    matrix_[kRowID][kRowID] = 0;
    matrix_[kRowID][kVarchar] = 1;

    // From embedding to other type
    matrix_[kMixed][kMixed] = 0;
    matrix_[kMixed][kBigInt] = 1;
    matrix_[kMixed][kFloat] = 2;
    matrix_[kMixed][kVarchar] = 4;

    // From null to other type
    matrix_[kNull][kNull] = 0;
    matrix_[kNull][kBoolean] = 0;
    matrix_[kNull][kTinyInt] = 0;
    matrix_[kNull][kSmallInt] = 0;
    matrix_[kNull][kInteger] = 0;
    matrix_[kNull][kBigInt] = 0;
    matrix_[kNull][kHugeInt] = 0;
    matrix_[kNull][kFloat] = 0;
    matrix_[kNull][kDouble] = 0;
    matrix_[kNull][kDecimal] = 0;
    matrix_[kNull][kVarchar] = 0;
    matrix_[kNull][kDate] = 0;
    matrix_[kNull][kTime] = 0;
    matrix_[kNull][kDateTime] = 0;
    matrix_[kNull][kTimestamp] = 0;
    matrix_[kNull][kInterval] = 0;
    matrix_[kNull][kArray] = 0;
    matrix_[kNull][kTuple] = 0;
    matrix_[kNull][kPoint] = 0;
    matrix_[kNull][kLine] = 0;
    matrix_[kNull][kLineSeg] = 0;
    matrix_[kNull][kBox] = 0;
    matrix_[kNull][kPath] = 0;
    matrix_[kNull][kPolygon] = 0;
    matrix_[kNull][kCircle] = 0;
    matrix_[kNull][kBitmap] = 0;
    matrix_[kNull][kUuid] = 0;
    matrix_[kNull][kBlob] = 0;
    matrix_[kNull][kEmbedding] = 0;
    matrix_[kNull][kRowID] = 0;

    // From varchar to other type
    matrix_[kVarchar][kNull] = 2;
    matrix_[kVarchar][kBoolean] = 2;
    matrix_[kVarchar][kTinyInt] = 2;
    matrix_[kVarchar][kSmallInt] = 2;
    matrix_[kVarchar][kInteger] = 2;
    matrix_[kVarchar][kBigInt] = 2;
    matrix_[kVarchar][kHugeInt] = 2;
    matrix_[kVarchar][kFloat] = 2;
    matrix_[kVarchar][kDouble] = 2;
    matrix_[kVarchar][kDecimal] = 2;
    matrix_[kVarchar][kVarchar] = 0;
    matrix_[kVarchar][kDate] = 2;
    matrix_[kVarchar][kTime] = 2;
    matrix_[kVarchar][kDateTime] = 2;
    matrix_[kVarchar][kTimestamp] = 2;
    matrix_[kVarchar][kInterval] = 2;
    matrix_[kVarchar][kArray] = 2;
    matrix_[kVarchar][kTuple] = 2;
    matrix_[kVarchar][kPoint] = 2;
    matrix_[kVarchar][kLine] = 2;
    matrix_[kVarchar][kLineSeg] = 2;
    matrix_[kVarchar][kBox] = 2;
    matrix_[kVarchar][kPath] = 2;
    matrix_[kVarchar][kPolygon] = 2;
    matrix_[kVarchar][kCircle] = 2;
    matrix_[kVarchar][kBitmap] = 2;
    matrix_[kVarchar][kUuid] = 2;
    matrix_[kVarchar][kBlob] = 2;
    matrix_[kVarchar][kEmbedding] = 2;
    matrix_[kVarchar][kRowID] = 2;
}

i64 CastTable::GetCastCost(LogicalType from, LogicalType to) const { return matrix_[from][to]; }

} // namespace infinity