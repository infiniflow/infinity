//
// Created by JinHai on 2022/12/15.
//

#pragma once

namespace infinity {

struct ColumnVectorTryCastData {
    ColumnVectorTryCastData(ColumnVector &result, string *error_message_p, bool strict_p)
            : result(result), error_message(error_message_p), strict(strict_p) {
    }

    Vector &result_ref_;
    string *error_message;
    bool strict;
    bool all_converted = true;
};


struct ColumnVectorCast {
template <class SourceType, class TargetType, class OP>
inline static bool
TryCastColumnVector(ColumnVector &source, ColumnVector &result, size_t count, CastParameters &parameters) {
    return TemplatedTryCastLoop<SourceType, TargetType, VectorTryCastOperator<OP>>(source, result, count, parameters);
}

};

}