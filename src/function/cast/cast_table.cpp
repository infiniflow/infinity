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

module cast_table;

namespace infinity {

CastTable::CastTable() {
    for (auto &arr : matrix_) {
        arr.fill(-1);
    }

    // From boolean to other type
    matrix_[to_underlying_val(LogicalType::kBoolean)][to_underlying_val(LogicalType::kBoolean)] = 0;
    matrix_[to_underlying_val(LogicalType::kBoolean)][to_underlying_val(LogicalType::kVarchar)] = 100;

    // From tinyint to other type
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kTinyInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kSmallInt)] = 1;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kInteger)] = 2;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kBigInt)] = 3;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kHugeInt)] = 4;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kDecimal)] = 8;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kFloat)] = 9;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kDouble)] = 10;
    matrix_[to_underlying_val(LogicalType::kTinyInt)][to_underlying_val(LogicalType::kVarchar)] = 112;

    // From smallint to other type
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kSmallInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kInteger)] = 1;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kBigInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kHugeInt)] = 3;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kDecimal)] = 7;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kFloat)] = 8;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kDouble)] = 9;
    matrix_[to_underlying_val(LogicalType::kSmallInt)][to_underlying_val(LogicalType::kVarchar)] = 111;

    // From integer to other type
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kInteger)] = 0;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kBigInt)] = 1;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kHugeInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kDecimal)] = 6;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kFloat)] = 7;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kDouble)] = 8;
    matrix_[to_underlying_val(LogicalType::kInteger)][to_underlying_val(LogicalType::kVarchar)] = 110;

    // From big int to other type
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kBigInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kHugeInt)] = 1;
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kDecimal)] = 5;
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kFloat)] = 6;
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kDouble)] = 7;
    matrix_[to_underlying_val(LogicalType::kBigInt)][to_underlying_val(LogicalType::kVarchar)] = 109;

    // From huge int to other type
    matrix_[to_underlying_val(LogicalType::kHugeInt)][to_underlying_val(LogicalType::kHugeInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kHugeInt)][to_underlying_val(LogicalType::kDecimal)] = 4;
    matrix_[to_underlying_val(LogicalType::kHugeInt)][to_underlying_val(LogicalType::kFloat)] = 5;
    matrix_[to_underlying_val(LogicalType::kHugeInt)][to_underlying_val(LogicalType::kDouble)] = 6;
    matrix_[to_underlying_val(LogicalType::kHugeInt)][to_underlying_val(LogicalType::kVarchar)] = 108;

    // From decimal128 to other type
    matrix_[to_underlying_val(LogicalType::kDecimal)][to_underlying_val(LogicalType::kDecimal)] = 0;
    matrix_[to_underlying_val(LogicalType::kDecimal)][to_underlying_val(LogicalType::kFloat)] = 1;
    matrix_[to_underlying_val(LogicalType::kDecimal)][to_underlying_val(LogicalType::kDouble)] = 2;
    matrix_[to_underlying_val(LogicalType::kDecimal)][to_underlying_val(LogicalType::kVarchar)] = 104;

    // From float16 to other type
    matrix_[to_underlying_val(LogicalType::kFloat16)][to_underlying_val(LogicalType::kFloat16)] = 0;
    matrix_[to_underlying_val(LogicalType::kFloat16)][to_underlying_val(LogicalType::kFloat)] = 1;
    matrix_[to_underlying_val(LogicalType::kFloat16)][to_underlying_val(LogicalType::kDouble)] = 2;
    matrix_[to_underlying_val(LogicalType::kFloat16)][to_underlying_val(LogicalType::kVarchar)] = 103;

    // From bfloat16 to other type
    matrix_[to_underlying_val(LogicalType::kBFloat16)][to_underlying_val(LogicalType::kBFloat16)] = 0;
    matrix_[to_underlying_val(LogicalType::kBFloat16)][to_underlying_val(LogicalType::kFloat)] = 1;
    matrix_[to_underlying_val(LogicalType::kBFloat16)][to_underlying_val(LogicalType::kDouble)] = 2;
    matrix_[to_underlying_val(LogicalType::kBFloat16)][to_underlying_val(LogicalType::kVarchar)] = 103;

    // From float to other type
    matrix_[to_underlying_val(LogicalType::kFloat)][to_underlying_val(LogicalType::kFloat)] = 0;
    matrix_[to_underlying_val(LogicalType::kFloat)][to_underlying_val(LogicalType::kDouble)] = 1;
    matrix_[to_underlying_val(LogicalType::kFloat)][to_underlying_val(LogicalType::kVarchar)] = 103;

    // From double to other type
    matrix_[to_underlying_val(LogicalType::kDouble)][to_underlying_val(LogicalType::kDouble)] = 0;
    matrix_[to_underlying_val(LogicalType::kDouble)][to_underlying_val(LogicalType::kVarchar)] = 102;

    // From date to other type
    matrix_[to_underlying_val(LogicalType::kDate)][to_underlying_val(LogicalType::kDate)] = 0;
    matrix_[to_underlying_val(LogicalType::kDate)][to_underlying_val(LogicalType::kDateTime)] = 1;
    matrix_[to_underlying_val(LogicalType::kDate)][to_underlying_val(LogicalType::kTimestamp)] = 2;
    matrix_[to_underlying_val(LogicalType::kDate)][to_underlying_val(LogicalType::kVarchar)] = 105;

    // From time to other type
    matrix_[to_underlying_val(LogicalType::kTime)][to_underlying_val(LogicalType::kTime)] = 0;
    matrix_[to_underlying_val(LogicalType::kTime)][to_underlying_val(LogicalType::kVarchar)] = 102;

    // From datetime to other type
    matrix_[to_underlying_val(LogicalType::kDateTime)][to_underlying_val(LogicalType::kDateTime)] = 0;
    matrix_[to_underlying_val(LogicalType::kDateTime)][to_underlying_val(LogicalType::kTimestamp)] = 1;
    matrix_[to_underlying_val(LogicalType::kDateTime)][to_underlying_val(LogicalType::kTime)] = 2;
    matrix_[to_underlying_val(LogicalType::kDateTime)][to_underlying_val(LogicalType::kDate)] = 2;
    matrix_[to_underlying_val(LogicalType::kDateTime)][to_underlying_val(LogicalType::kVarchar)] = 103;

    // From timestamp to other type
    matrix_[to_underlying_val(LogicalType::kTimestamp)][to_underlying_val(LogicalType::kTimestamp)] = 0;
    matrix_[to_underlying_val(LogicalType::kTimestamp)][to_underlying_val(LogicalType::kDateTime)] = 1;
    matrix_[to_underlying_val(LogicalType::kTimestamp)][to_underlying_val(LogicalType::kTime)] = 2;
    matrix_[to_underlying_val(LogicalType::kTimestamp)][to_underlying_val(LogicalType::kDate)] = 2;
    matrix_[to_underlying_val(LogicalType::kTimestamp)][to_underlying_val(LogicalType::kVarchar)] = 103;

    // From interval to other type
    matrix_[to_underlying_val(LogicalType::kInterval)][to_underlying_val(LogicalType::kInterval)] = 0;
    matrix_[to_underlying_val(LogicalType::kInterval)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From array to other type
    // From tuple to other type

    // From point to other type
    matrix_[to_underlying_val(LogicalType::kPoint)][to_underlying_val(LogicalType::kPoint)] = 0;
    matrix_[to_underlying_val(LogicalType::kPoint)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From line to other type
    matrix_[to_underlying_val(LogicalType::kLine)][to_underlying_val(LogicalType::kLine)] = 0;
    matrix_[to_underlying_val(LogicalType::kLine)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From line segment to other type
    matrix_[to_underlying_val(LogicalType::kLineSeg)][to_underlying_val(LogicalType::kLineSeg)] = 0;
    matrix_[to_underlying_val(LogicalType::kLineSeg)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From box to other type
    matrix_[to_underlying_val(LogicalType::kBox)][to_underlying_val(LogicalType::kBox)] = 0;
    matrix_[to_underlying_val(LogicalType::kBox)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From path to other type
    //    matrix_[kPath][kPath] = 0;
    //    matrix_[kPath][kVarchar] = 1;

    // From polygon to other type
    //    matrix_[kPolygon][kPolygon] = 0;
    //    matrix_[kPolygon][kVarchar] = 1;

    // From circle to other type
    matrix_[to_underlying_val(LogicalType::kCircle)][to_underlying_val(LogicalType::kCircle)] = 0;
    matrix_[to_underlying_val(LogicalType::kCircle)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From bitmap to other type
    //    matrix_[kBitmap][kBitmap] = 0;
    //    matrix_[kBitmap][kVarchar] = 1;

    // From uuid to other type
    matrix_[to_underlying_val(LogicalType::kUuid)][to_underlying_val(LogicalType::kUuid)] = 0;
    matrix_[to_underlying_val(LogicalType::kUuid)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From cast blob to other type
    //    matrix_[kBlob][kBlob] = 0;

    // From embedding to other type
    matrix_[to_underlying_val(LogicalType::kEmbedding)][to_underlying_val(LogicalType::kEmbedding)] = 0;
    matrix_[to_underlying_val(LogicalType::kEmbedding)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From row_id to other type
    matrix_[to_underlying_val(LogicalType::kRowID)][to_underlying_val(LogicalType::kRowID)] = 0;
    matrix_[to_underlying_val(LogicalType::kRowID)][to_underlying_val(LogicalType::kVarchar)] = 101;

    // From embedding to other type
    matrix_[to_underlying_val(LogicalType::kMixed)][to_underlying_val(LogicalType::kMixed)] = 0;
    matrix_[to_underlying_val(LogicalType::kMixed)][to_underlying_val(LogicalType::kBigInt)] = 1;
    matrix_[to_underlying_val(LogicalType::kMixed)][to_underlying_val(LogicalType::kFloat)] = 2;
    matrix_[to_underlying_val(LogicalType::kMixed)][to_underlying_val(LogicalType::kVarchar)] = 104;

    // From null to other type
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kNull)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kBoolean)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kTinyInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kSmallInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kInteger)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kBigInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kHugeInt)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kFloat)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kDouble)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kDecimal)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kVarchar)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kDate)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kTime)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kDateTime)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kTimestamp)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kInterval)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kArray)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kTuple)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kPoint)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kLine)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kLineSeg)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kBox)] = 0;
    //    matrix_[kNull][kPath] = 0;
    //    matrix_[kNull][kPolygon] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kCircle)] = 0;
    //    matrix_[kNull][kBitmap] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kUuid)] = 0;
    //    matrix_[kNull][kBlob] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kEmbedding)] = 0;
    matrix_[to_underlying_val(LogicalType::kNull)][to_underlying_val(LogicalType::kRowID)] = 0;

    // From varchar to other type
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kNull)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kBoolean)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kTinyInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kSmallInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kInteger)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kBigInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kHugeInt)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kFloat)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kDouble)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kDecimal)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kVarchar)] = 0;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kDate)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kTime)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kDateTime)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kTimestamp)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kInterval)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kArray)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kTuple)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kPoint)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kLine)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kLineSeg)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kBox)] = 2;
    //    matrix_[kVarchar][kPath] = 2;
    //    matrix_[kVarchar][kPolygon] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kCircle)] = 2;
    //    matrix_[kVarchar][kBitmap] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kUuid)] = 2;
    //    matrix_[kVarchar][kBlob] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kEmbedding)] = 2;
    matrix_[to_underlying_val(LogicalType::kVarchar)][to_underlying_val(LogicalType::kRowID)] = 2;
}

i64 CastTable::GetCastCost(LogicalType from, LogicalType to) const { return matrix_[to_underlying_val(from)][to_underlying_val(to)]; }

} // namespace infinity