/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#ifndef BITPACKINGUNALIGNED_H_
#define BITPACKINGUNALIGNED_H_
#include "common.h"

namespace FastPForLib {

typedef uint8_t byte;

const byte *fastunalignedunpack_8(const byte *__restrict__ in,
                                  uint32_t *__restrict__ out,
                                  const uint32_t bit);
byte *fastunalignedpackwithoutmask_8(const uint32_t *__restrict__ in,
                                     byte *__restrict__ out,
                                     const uint32_t bit);
const byte *fastunalignedbyteunpack_8(const byte *__restrict__ in,
                                      byte *__restrict__ out,
                                      const uint32_t bit);
byte *fastunalignedbytepackwithoutmask_8(const byte *__restrict__ in,
                                         byte *__restrict__ out,
                                         const uint32_t bit);
const byte *fastunalignedunpack_16(const byte *__restrict__ in,
                                   uint32_t *__restrict__ out,
                                   const uint32_t bit);
byte *fastunalignedpackwithoutmask_16(const uint32_t *__restrict__ in,
                                      byte *__restrict__ out,
                                      const uint32_t bit);
const byte *fastunalignedbyteunpack_16(const byte *__restrict__ in,
                                       byte *__restrict__ out,
                                       const uint32_t bit);
byte *fastunalignedbytepackwithoutmask_16(const byte *__restrict__ in,
                                          byte *__restrict__ out,
                                          const uint32_t bit);

} // namespace FastPForLib

#endif /* BITPACKINGUNALIGNED_H_ */
