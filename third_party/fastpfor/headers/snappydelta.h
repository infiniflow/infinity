/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */
/**
 * Goal here is to use Google Snappy to compress deltas.
 * This is purely for comparison sake, not necessarily
 * because this is a good idea a priori.
 */
#ifdef USESNAPPY

#ifndef SNAPPYDELTA_H_
#define SNAPPYDELTA_H_

#include <snappy.h>
#include "common.h"
#include "codecs.h"

namespace FastPFor {

class JustSnappy : public IntegerCODEC {
public:
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    size_t howmuchroom = nvalue * 4;
    snappy::RawCompress(reinterpret_cast<const char *>(in), length * 4,
                        reinterpret_cast<char *>(out), &howmuchroom);
    nvalue = (howmuchroom + 3) / 4;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    size_t nvalueinbytes;
    bool ok = snappy::GetUncompressedLength(reinterpret_cast<const char *>(in),
                                            length * 4, &nvalueinbytes);
    assert(nvalueinbytes % 4 == 0);
    nvalue = nvalueinbytes / 4;
    if (!ok)
      throw logic_error("got some bug retrieving length");
    snappy::RawUncompress(reinterpret_cast<const char *>(in), length * 4,
                          reinterpret_cast<char *>(out));
    return in + length;
  }
  string name() const { return "Snappy"; }
};

} // namespace FastPFor

#endif /* SNAPPYDELTA_H_ */

#endif // USESNAPPY
