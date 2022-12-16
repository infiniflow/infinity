//
// Created by JinHai on 2022/12/15.
//

#pragma once
#include "common/types/internal_types.h"

namespace infinity {



struct ColumnVectorTryCastData {
    ColumnVectorTryCastData(ColumnVector &result, String& error_message_p, bool strict_p)
            : result_ref_(result), error_message(error_message_p), strict(strict_p) {
    }

    ColumnVector &result_ref_;
    String& error_message;
    bool strict;
    bool all_converted = true;
};


template<typename Operator>
struct ValueTryCast {
    template<typename SourceValueType, typename TargetValueType>
    inline static TargetValueType
    Operation(SourceValueType input, size_t idx, void* state_ptr) {
        TargetValueType result;
        if(Operator::template Operation<SourceValueType, TargetValueType>(input, result)) {
            return result;
        }

        auto* data_ptr = (ColumnVectorTryCastData*)(state_ptr);
        return
    }
};

struct ColumnVectorCast {
template <class SourceType, class TargetType, class Operator>
inline static bool
TryCastColumnVector(ColumnVector &source, ColumnVector &result, size_t count, CastParameters &parameters) {
    return TemplatedTryCastLoop<SourceType, TargetType, ValueTryCast<Operator>>(source, result, count, parameters);
}

};

}