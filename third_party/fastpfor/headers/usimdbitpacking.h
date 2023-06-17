/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire
 */
#ifndef USIMDBITPACKING_H_
#define USIMDBITPACKING_H_

#include "common.h"

namespace FastPForLib {

void usimdpack(const uint32_t *__restrict__ in, __m128i *__restrict__ out,
               uint32_t bit);
void usimdpackwithoutmask(const uint32_t *__restrict__ in,
                          __m128i *__restrict__ out, uint32_t bit);
void usimdunpack(const __m128i *__restrict__ in, uint32_t *__restrict__ out,
                 uint32_t bit);

} // namespace FastPFor

#endif /* SIMDBITPACKING_H_ */
