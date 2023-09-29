//
// Created by jinhai on 23-9-29.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

template<typename ElemType>
ElemType
fvec_norm_L2sqr(const ElemType* query, SizeT dimension);

template<>
f32
fvec_norm_L2sqr(const f32* query, SizeT dimension);

}
