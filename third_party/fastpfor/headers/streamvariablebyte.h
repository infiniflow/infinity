/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef _STREAMVARIABLEBYTE_
#define _STREAMVARIABLEBYTE_

#include "common.h"
#include "codecs.h"

namespace FastPForLib {

/**
 * StreamVByte is an integer CODEC invented by Nathan Kurz.
 */

extern "C" {
uint64_t svb_encode(uint8_t *out, const uint32_t *in, uint32_t count, int delta,
                    int type);
uint8_t *svb_decode_avx_simple(uint32_t *out, uint8_t *keyPtr, uint8_t *dataPtr,
                               uint64_t count);
uint8_t *svb_decode_avx_d1_simple(uint32_t *out, uint8_t *keyPtr,
                                  uint8_t *dataPtr, uint64_t count);
uint8_t *svb_decode_scalar_d1_init(uint32_t *outPtr, const uint8_t *keyPtr,
                                   uint8_t *dataPtr, uint32_t count,
                                   uint32_t prev);
uint32_t svb_select_avx_d1_init(uint8_t *keyPtr, uint8_t *dataPtr,
                                uint64_t count, uint32_t prev, int slot);
int svb_find_avx_d1_init(uint8_t *keyPtr, uint8_t *dataPtr, uint64_t count,
                         uint32_t prev, uint32_t key, uint32_t *presult);
uint8_t *svb_insert_scalar_d1_init(uint8_t *keyPtr, uint8_t *dataPtr,
                                   size_t dataSize, uint32_t count,
                                   uint32_t prev, uint32_t new_key,
                                   uint32_t *position);
uint8_t *svb_append_scalar_d1(uint8_t *keyPtr, uint8_t *dataPtr,
                              size_t sizebytes, size_t count, uint32_t delta);
}

/**
 * Regular StreamVByte (no differential coding)
 */
class StreamVByte : public IntegerCODEC {
public:
  void encodeArray(const uint32_t *in, const size_t count, uint32_t *out,
                   size_t &nvalue) {
    uint64_t bytesWritten = svb_encode(
        (uint8_t *)out, in, static_cast<uint32_t>(std::min<size_t>(
                                count, std::numeric_limits<uint32_t>::max())),
        0, 1);
    nvalue = static_cast<size_t>(bytesWritten + 3) / 4;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t /* count */,
                              uint32_t *out, size_t &nvalue) {
    uint32_t count = *(uint32_t *)in; // first 4 bytes is number of ints
    nvalue = count;
    if (count == 0)
      return 0;

    uint8_t *keyPtr = (uint8_t *)in + 4; // full list of keys is next
    uint32_t keyLen = ((count + 3) / 4); // 2-bits per key (rounded up)
    uint8_t *dataPtr = keyPtr + keyLen;  // data starts at end of keys
    nvalue = count;
    return reinterpret_cast<const uint32_t *>(
        (reinterpret_cast<uintptr_t>(
             svb_decode_avx_simple(out, keyPtr, dataPtr, count)) +
         3) &
        ~3);
  }

  std::string name() const { return "streamvbyte"; }
};
}
#endif
