/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#ifndef BITPACKINGALIGNED
#define BITPACKINGALIGNED

#include "common.h"

namespace FastPForLib {

const uint32_t *fastunpack_8(const uint32_t *__restrict__ in,
                             uint32_t *__restrict__ out, const uint32_t bit);
uint32_t *fastpackwithoutmask_8(const uint32_t *__restrict__ in,
                                uint32_t *__restrict__ out, const uint32_t bit);

const uint32_t *fastunpack_16(const uint32_t *__restrict__ in,
                              uint32_t *__restrict__ out, const uint32_t bit);
uint32_t *fastpackwithoutmask_16(const uint32_t *__restrict__ in,
                                 uint32_t *__restrict__ out,
                                 const uint32_t bit);

const uint32_t *fastunpack_24(const uint32_t *__restrict__ in,
                              uint32_t *__restrict__ out, const uint32_t bit);
uint32_t *fastpackwithoutmask_24(const uint32_t *__restrict__ in,
                                 uint32_t *__restrict__ out,
                                 const uint32_t bit);

const uint32_t *fastunpack_32(const uint32_t *__restrict__ in,
                              uint32_t *__restrict__ out, const uint32_t bit);

uint32_t *fastpackwithoutmask_32(const uint32_t *__restrict__ in,
                                 uint32_t *__restrict__ out,
                                 const uint32_t bit);

} // namespace FastPFor

#endif // BITPACKINGALIGNED
