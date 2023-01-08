//
// Created by jinhai on 23-1-5.
//

#include "cast_table.h"

namespace infinity {

CastTable::CastTable() {
    for(i8 i = LogicalType::kBoolean; i < LogicalType::kInvalid; ++ i) {
        for(i8 j = LogicalType::kBoolean; j < LogicalType::kInvalid; ++ j) {
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
    matrix_[kTinyInt][kDecimal16] = 5;
    matrix_[kTinyInt][kDecimal32] = 6;
    matrix_[kTinyInt][kDecimal64] = 7;
    matrix_[kTinyInt][kDecimal128] = 8;
    matrix_[kTinyInt][kFloat] = 9;
    matrix_[kTinyInt][kDouble] = 10;
    matrix_[kTinyInt][kChar] = 11;
    matrix_[kTinyInt][kVarchar] = 12;

    // From smallint to other type
    matrix_[kSmallInt][kSmallInt] = 0;
    matrix_[kSmallInt][kInteger] = 1;
    matrix_[kSmallInt][kBigInt] = 2;
    matrix_[kSmallInt][kHugeInt] = 3;
    matrix_[kSmallInt][kDecimal16] = 4;
    matrix_[kSmallInt][kDecimal32] = 5;
    matrix_[kSmallInt][kDecimal64] = 6;
    matrix_[kSmallInt][kDecimal128] = 7;
    matrix_[kSmallInt][kFloat] = 8;
    matrix_[kSmallInt][kDouble] = 9;
    matrix_[kSmallInt][kChar] = 10;
    matrix_[kSmallInt][kVarchar] = 11;

    // From integer to other type
    matrix_[kInteger][kInteger] = 0;
    matrix_[kInteger][kBigInt] = 1;
    matrix_[kInteger][kHugeInt] = 2;
    matrix_[kInteger][kDecimal16] = 3;
    matrix_[kInteger][kDecimal32] = 4;
    matrix_[kInteger][kDecimal64] = 5;
    matrix_[kInteger][kDecimal128] = 6;
    matrix_[kInteger][kFloat] = 7;
    matrix_[kInteger][kDouble] = 8;
    matrix_[kInteger][kChar] = 9;
    matrix_[kInteger][kVarchar] = 10;

    // From big int to other type
    matrix_[kBigInt][kBigInt] = 0;
    matrix_[kBigInt][kHugeInt] = 1;
    matrix_[kBigInt][kDecimal16] = 2;
    matrix_[kBigInt][kDecimal32] = 3;
    matrix_[kBigInt][kDecimal64] = 4;
    matrix_[kBigInt][kDecimal128] = 5;
    matrix_[kBigInt][kFloat] = 6;
    matrix_[kBigInt][kDouble] = 7;
    matrix_[kBigInt][kChar] = 8;
    matrix_[kBigInt][kVarchar] = 9;

    // From huge int to other type
    matrix_[kHugeInt][kHugeInt] = 0;
    matrix_[kHugeInt][kDecimal16] = 1;
    matrix_[kHugeInt][kDecimal32] = 2;
    matrix_[kHugeInt][kDecimal64] = 3;
    matrix_[kHugeInt][kDecimal128] = 4;
    matrix_[kHugeInt][kFloat] = 5;
    matrix_[kHugeInt][kDouble] = 6;
    matrix_[kHugeInt][kChar] = 7;
    matrix_[kHugeInt][kVarchar] = 8;

    // From decimal16 to other type
    matrix_[kDecimal16][kDecimal16] = 0;
    matrix_[kDecimal16][kDecimal32] = 1;
    matrix_[kDecimal16][kDecimal64] = 2;
    matrix_[kDecimal16][kDecimal128] = 3;
    matrix_[kDecimal16][kFloat] = 4;
    matrix_[kDecimal16][kDouble] = 5;
    matrix_[kDecimal16][kChar] = 6;
    matrix_[kDecimal16][kVarchar] = 7;

    // From decimal32 to other type
    matrix_[kDecimal32][kDecimal32] = 0;
    matrix_[kDecimal32][kDecimal64] = 1;
    matrix_[kDecimal32][kDecimal128] = 2;
    matrix_[kDecimal32][kFloat] = 3;
    matrix_[kDecimal32][kDouble] = 4;
    matrix_[kDecimal32][kChar] = 5;
    matrix_[kDecimal32][kVarchar] = 6;

    // From decimal64 to other type
    matrix_[kDecimal64][kDecimal64] = 0;
    matrix_[kDecimal64][kDecimal128] = 1;
    matrix_[kDecimal64][kFloat] = 2;
    matrix_[kDecimal64][kDouble] = 3;
    matrix_[kDecimal64][kChar] = 4;
    matrix_[kDecimal64][kVarchar] = 5;

    // From decimal128 to other type
    matrix_[kDecimal128][kDecimal128] = 0;
    matrix_[kDecimal128][kFloat] = 1;
    matrix_[kDecimal128][kDouble] = 2;
    matrix_[kDecimal128][kChar] = 3;
    matrix_[kDecimal128][kVarchar] = 4;

    // From float to other type
    matrix_[kFloat][kFloat] = 0;
    matrix_[kFloat][kDouble] = 1;
    matrix_[kFloat][kChar] = 2;
    matrix_[kFloat][kVarchar] = 3;

    // From double to other type
    matrix_[kDouble][kDouble] = 0;
    matrix_[kDouble][kChar] = 1;
    matrix_[kDouble][kVarchar] = 2;

    // From date to other type
    matrix_[kDate][kDate] = 0;
    matrix_[kDate][kDateTime] = 1;
    matrix_[kDate][kTimestamp] = 2;
    matrix_[kDate][kTimestampTZ] = 3;
    matrix_[kDate][kChar] = 4;
    matrix_[kDate][kVarchar] = 5;

    // From time to other type
    matrix_[kTime][kTime] = 0;
    matrix_[kTime][kChar] = 1;
    matrix_[kTime][kVarchar] = 2;

    // From datetime to other type
    matrix_[kDateTime][kDateTime] = 0;
    matrix_[kDateTime][kTimestamp] = 1;
    matrix_[kDateTime][kTimestampTZ] = 1;
    matrix_[kDateTime][kTime] = 2;
    matrix_[kDateTime][kDate] = 2;
    matrix_[kDateTime][kChar] = 3;
    matrix_[kDateTime][kVarchar] = 3;

    // From timestamp to other type
    matrix_[kTimestamp][kTimestamp] = 0;
    matrix_[kTimestamp][kTimestampTZ] = 1;
    matrix_[kTimestamp][kDateTime] = 1;
    matrix_[kTimestamp][kTime] = 2;
    matrix_[kTimestamp][kDate] = 2;
    matrix_[kTimestamp][kChar] = 3;
    matrix_[kTimestamp][kVarchar] = 3;

    // From timestamp tz to other type
    matrix_[kTimestampTZ][kTimestampTZ] = 0;
    matrix_[kTimestampTZ][kTimestamp] = 1;
    matrix_[kTimestampTZ][kDateTime] = 1;
    matrix_[kTimestampTZ][kTime] = 2;
    matrix_[kTimestampTZ][kDate] = 2;
    matrix_[kTimestampTZ][kChar] = 3;
    matrix_[kTimestampTZ][kVarchar] = 3;

    // From interval to other type
    matrix_[kInterval][kInterval] = 0;
    matrix_[kInterval][kChar] = 1;
    matrix_[kInterval][kVarchar] = 1;

    // From array to other type
    // From tuple to other type

    // From point to other type
    matrix_[kPoint][kPoint] = 0;
    matrix_[kPoint][kChar] = 1;
    matrix_[kPoint][kVarchar] = 1;

    // From line to other type
    matrix_[kLine][kLine] = 0;
    matrix_[kLine][kChar] = 1;
    matrix_[kLine][kVarchar] = 1;

    // From line segment to other type
    matrix_[kLineSeg][kLineSeg] = 0;
    matrix_[kLineSeg][kChar] = 1;
    matrix_[kLineSeg][kVarchar] = 1;

    // From box to other type
    matrix_[kBox][kBox] = 0;
    matrix_[kBox][kChar] = 1;
    matrix_[kBox][kVarchar] = 1;

    // From path to other type
    matrix_[kPath][kPath] = 0;
    matrix_[kPath][kChar] = 1;
    matrix_[kPath][kVarchar] = 1;

    // From polygon to other type
    matrix_[kPolygon][kPolygon] = 0;
    matrix_[kPolygon][kChar] = 1;
    matrix_[kPolygon][kVarchar] = 1;

    // From circle to other type
    matrix_[kCircle][kCircle] = 0;
    matrix_[kCircle][kChar] = 1;
    matrix_[kCircle][kVarchar] = 1;

    // From bitmap to other type
    matrix_[kBitmap][kBitmap] = 0;
    matrix_[kBitmap][kChar] = 1;
    matrix_[kBitmap][kVarchar] = 1;

    // From uuid to other type
    matrix_[kUuid][kUuid] = 0;
    matrix_[kUuid][kChar] = 1;
    matrix_[kUuid][kVarchar] = 1;

    // From cast blob to other type
    matrix_[kBlob][kBlob] = 0;

    // From embedding to other type
    matrix_[kEmbedding][kEmbedding] = 0;
    matrix_[kEmbedding][kChar] = 1;
    matrix_[kEmbedding][kVarchar] = 1;

    // From embedding to other type
    matrix_[kMixed][kMixed] = 0;
    matrix_[kMixed][kBigInt] = 1;
    matrix_[kMixed][kFloat] = 2;
    matrix_[kMixed][kChar] = 3;
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
    matrix_[kNull][kDecimal16] = 0;
    matrix_[kNull][kDecimal32] = 0;
    matrix_[kNull][kDecimal64] = 0;
    matrix_[kNull][kDecimal128] = 0;
    matrix_[kNull][kVarchar] = 0;
    matrix_[kNull][kChar] = 0;
    matrix_[kNull][kDate] = 0;
    matrix_[kNull][kTime] = 0;
    matrix_[kNull][kDateTime] = 0;
    matrix_[kNull][kTimestamp] = 0;
    matrix_[kNull][kTimestampTZ] = 0;
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

    // From char to other type
    matrix_[kChar][kNull] = 0;
    matrix_[kChar][kBoolean] = 0;
    matrix_[kChar][kTinyInt] = 0;
    matrix_[kChar][kSmallInt] = 0;
    matrix_[kChar][kInteger] = 0;
    matrix_[kChar][kBigInt] = 0;
    matrix_[kChar][kHugeInt] = 0;
    matrix_[kChar][kFloat] = 0;
    matrix_[kChar][kDouble] = 0;
    matrix_[kChar][kDecimal16] = 0;
    matrix_[kChar][kDecimal32] = 0;
    matrix_[kChar][kDecimal64] = 0;
    matrix_[kChar][kDecimal128] = 0;
    matrix_[kChar][kVarchar] = 0;
    matrix_[kChar][kChar] = 0;
    matrix_[kChar][kDate] = 0;
    matrix_[kChar][kTime] = 0;
    matrix_[kChar][kDateTime] = 0;
    matrix_[kChar][kTimestamp] = 0;
    matrix_[kChar][kTimestampTZ] = 0;
    matrix_[kChar][kInterval] = 0;
    matrix_[kChar][kArray] = 0;
    matrix_[kChar][kTuple] = 0;
    matrix_[kChar][kPoint] = 0;
    matrix_[kChar][kLine] = 0;
    matrix_[kChar][kLineSeg] = 0;
    matrix_[kChar][kBox] = 0;
    matrix_[kChar][kPath] = 0;
    matrix_[kChar][kPolygon] = 0;
    matrix_[kChar][kCircle] = 0;
    matrix_[kChar][kBitmap] = 0;
    matrix_[kChar][kUuid] = 0;
    matrix_[kChar][kBlob] = 0;
    matrix_[kChar][kEmbedding] = 0;

    // From varchar to other type
    matrix_[kVarchar][kNull] = 0;
    matrix_[kVarchar][kBoolean] = 0;
    matrix_[kVarchar][kTinyInt] = 0;
    matrix_[kVarchar][kSmallInt] = 0;
    matrix_[kVarchar][kInteger] = 0;
    matrix_[kVarchar][kBigInt] = 0;
    matrix_[kVarchar][kHugeInt] = 0;
    matrix_[kVarchar][kFloat] = 0;
    matrix_[kVarchar][kDouble] = 0;
    matrix_[kVarchar][kDecimal16] = 0;
    matrix_[kVarchar][kDecimal32] = 0;
    matrix_[kVarchar][kDecimal64] = 0;
    matrix_[kVarchar][kDecimal128] = 0;
    matrix_[kVarchar][kVarchar] = 0;
    matrix_[kVarchar][kChar] = 0;
    matrix_[kVarchar][kDate] = 0;
    matrix_[kVarchar][kTime] = 0;
    matrix_[kVarchar][kDateTime] = 0;
    matrix_[kVarchar][kTimestamp] = 0;
    matrix_[kVarchar][kTimestampTZ] = 0;
    matrix_[kVarchar][kInterval] = 0;
    matrix_[kVarchar][kArray] = 0;
    matrix_[kVarchar][kTuple] = 0;
    matrix_[kVarchar][kPoint] = 0;
    matrix_[kVarchar][kLine] = 0;
    matrix_[kVarchar][kLineSeg] = 0;
    matrix_[kVarchar][kBox] = 0;
    matrix_[kVarchar][kPath] = 0;
    matrix_[kVarchar][kPolygon] = 0;
    matrix_[kVarchar][kCircle] = 0;
    matrix_[kVarchar][kBitmap] = 0;
    matrix_[kVarchar][kUuid] = 0;
    matrix_[kVarchar][kBlob] = 0;
    matrix_[kVarchar][kEmbedding] = 0;
}

i64
CastTable::GetCastCost(LogicalType from, LogicalType to) const {
    return matrix_[from][to];
}

}