//
// Created by jinhai on 23-9-29.
//

module;

import stl;

export module distance;

namespace infinity {

export template <typename ElemType>
ElemType fvec_norm_L2sqr(const ElemType *query, SizeT dimension);

export template <>
f32 fvec_norm_L2sqr(const f32 *query, SizeT dimension);

export template <typename DistType>
void fvec_norms_L2sqr(DistType *__restrict nr, const DistType *__restrict queries, SizeT dimension, i16 query_count);

export template <>
void fvec_norms_L2sqr<f32>(f32 *__restrict nr, const f32 *__restrict queries, SizeT dimension, i16 query_count);

} // namespace infinity
