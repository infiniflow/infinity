/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire
 */

/**
 * This is purely for technical comparisons.
 *
 * Implementation of
 *
 * Willhalm T, Popovici N, Boshmaf Y, Plattner H, Zeier A, Schaffner J.
 * SIMD-scan: ultra fast in-memory table scan using on-chip vector processing
 * units.
 * Proceedings of the VLDB Endowment Aug 2009; 2(1):385-394.
 *
 * Optimized for a recent Intel core i7 processor by D. Lemire on Oct. 2012.
 */

#ifndef HORIZONTALBITPACKING_H_
#define HORIZONTALBITPACKING_H_

#if !defined(__SSE4_1__) && !(defined(_MSC_VER) && defined(__AVX__))

#ifndef _MSC_VER
#pragma message "No SSSE4.1 support? try adding -msse4.1 or the equivalent on your compiler"
#else
#pragma message("No SSSE4.1 support? try adding -msse4.1 or the equivalent on your compiler")
#endif
#endif
#include "common.h"

namespace FastPForLib {

void simdhunpack(const uint8_t *__restrict__ in, uint32_t *__restrict__ out,
                 uint32_t bit);
}

#endif /* HORIZONTALBITPACKING_H_ */
