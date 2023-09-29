//
// Created by jinhai on 23-9-29.
//

#include "distance.h"
#include "faiss/impl/platform_macros.h"

namespace infinity {

FAISS_PRAGMA_IMPRECISE_FUNCTION_BEGIN
template<typename ElemType>
ElemType
fvec_norm_L2sqr(const ElemType* query, SizeT dimension) {
    ElemType res = 0;
    FAISS_PRAGMA_IMPRECISE_LOOP
    for(size_t i = 0; i != dimension; ++i) {
        res += query[i] * query[i];
    }

    return res;
}
FAISS_PRAGMA_IMPRECISE_FUNCTION_END

FAISS_PRAGMA_IMPRECISE_FUNCTION_BEGIN
template<>
f32
fvec_norm_L2sqr(const f32* query, SizeT dimension) {
    f32 res = 0;
    FAISS_PRAGMA_IMPRECISE_LOOP
    for(size_t i = 0; i != dimension; ++i) {
        res += query[i] * query[i];
    }

    return res;
}
FAISS_PRAGMA_IMPRECISE_FUNCTION_END
}
