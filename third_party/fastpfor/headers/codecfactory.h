/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef CODECFACTORY_H_
#define CODECFACTORY_H_

#include "common.h"
#include "codecs.h"
#include "vsencoding.h"
#include "util.h"
#include "simple16.h"
#include "simple9.h"
#include "simple9_rle.h"
#include "simple8b.h"
#include "simple8b_rle.h"
#include "newpfor.h"
#include "simdnewpfor.h"
#include "optpfor.h"
#include "simdoptpfor.h"
#include "fastpfor.h"
#include "simdfastpfor.h"
#include "variablebyte.h"
#include "compositecodec.h"
#include "blockpacking.h"
#include "pfor.h"
#include "simdpfor.h"
#include "pfor2008.h"
#include "VarIntG8IU.h"
#include "simdbinarypacking.h"
#include "snappydelta.h"
#include "varintgb.h"
#include "simdvariablebyte.h"
#include "streamvariablebyte.h"
#include "simdgroupsimple.h"

namespace FastPForLib {

typedef std::map<std::string, std::shared_ptr<IntegerCODEC>> CodecMap;

/**
 * This class is a convenience class to generate codecs quickly.
 * It cannot be used safely in a multithreaded context where
 * each thread should have a different IntegerCODEC.
 */
class CODECFactory {
public:
  static CodecMap scodecmap;

  // hacked for convenience
  static std::vector<std::shared_ptr<IntegerCODEC>> allSchemes() {
    std::vector<std::shared_ptr<IntegerCODEC>> ans;
    for (auto i = scodecmap.begin(); i != scodecmap.end(); ++i) {
      ans.push_back(i->second);
    }
    return ans;
  }

  static std::vector<std::string> allNames() {
    std::vector<std::string> ans;
    for (auto i = scodecmap.begin(); i != scodecmap.end(); ++i) {
      ans.push_back(i->first);
    }
    return ans;
  }

  static std::shared_ptr<IntegerCODEC> &getFromName(std::string name) {
    if (scodecmap.find(name) == scodecmap.end()) {
      std::cerr << "name " << name << " does not refer to a CODEC."
                << std::endl;
      std::cerr << "possible choices:" << std::endl;
      for (auto i = scodecmap.begin(); i != scodecmap.end(); ++i) {
        std::cerr << static_cast<std::string>(i->first)
                  << std::endl; // useless cast, but just to be clear
      }
      std::cerr << "for now, I'm going to just return 'copy'" << std::endl;
      return scodecmap["copy"];
    }
    return scodecmap[name];
  }
};

// C++11 allows better than this, but neither Microsoft nor Intel support C++11
// fully.
static inline CodecMap initializefactory() {
  CodecMap map;
  map["fastbinarypacking8"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<FastBinaryPacking<8>, VariableByte>);
  map["fastbinarypacking16"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<FastBinaryPacking<16>, VariableByte>);
  map["fastbinarypacking32"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<FastBinaryPacking<32>, VariableByte>);
  map["BP32"] =
      std::shared_ptr<IntegerCODEC>(new CompositeCodec<BP32, VariableByte>);
  map["vsencoding"] =
      std::shared_ptr<IntegerCODEC>(new vsencoding::VSEncodingBlocks(1U << 16));
  map["fastpfor128"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<FastPFor<4>, VariableByte>());
  map["fastpfor256"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<FastPFor<8>, VariableByte>());
  map["simdfastpfor128"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDFastPFor<4>, VariableByte>());
  map["simdfastpfor256"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDFastPFor<8>, VariableByte>());
  map["simplepfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SimplePFor<>, VariableByte>());
  map["simdsimplepfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDSimplePFor<>, VariableByte>());
  map["pfor"] =
      std::shared_ptr<IntegerCODEC>(new CompositeCodec<PFor, VariableByte>());
  map["simdpfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDPFor, VariableByte>());
  map["pfor2008"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<PFor2008, VariableByte>());
  map["simdnewpfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDNewPFor<4, Simple16<false>>, VariableByte>());
  map["newpfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<NewPFor<4, Simple16<false>>, VariableByte>());
  map["optpfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<OPTPFor<4, Simple16<false>>, VariableByte>());
  map["simdoptpfor"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDOPTPFor<4, Simple16<false>>, VariableByte>());
  map["varint"] = std::shared_ptr<IntegerCODEC>(new VariableByte());
  map["vbyte"] = std::shared_ptr<IntegerCODEC>(new VByte());
  map["maskedvbyte"] = std::shared_ptr<IntegerCODEC>(new MaskedVByte());
  map["streamvbyte"] = std::shared_ptr<IntegerCODEC>(new StreamVByte());
  map["varintgb"] = std::shared_ptr<IntegerCODEC>(new VarIntGB<>());
  map["simple16"] = std::shared_ptr<IntegerCODEC>(new Simple16<true>());
  map["simple9"] = std::shared_ptr<IntegerCODEC>(new Simple9<true>());
  map["simple9_rle"] = std::shared_ptr<IntegerCODEC>(new Simple9_RLE<true>());
  map["simple8b"] = std::shared_ptr<IntegerCODEC>(new Simple8b<true>());
  map["simple8b_rle"] = std::shared_ptr<IntegerCODEC>(new Simple8b_RLE<true>());
#ifdef VARINTG8IU_H__
  map["varintg8iu"] = std::shared_ptr<IntegerCODEC>(new VarIntG8IU());
#endif
#ifdef USESNAPPY
  map["snappy"] = std::shared_ptr<IntegerCODEC>(new JustSnappy());
#endif
  map["simdbinarypacking"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDBinaryPacking, VariableByte>());
  map["simdgroupsimple"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDGroupSimple<false, false>, VariableByte>());
  map["simdgroupsimple_ringbuf"] = std::shared_ptr<IntegerCODEC>(
      new CompositeCodec<SIMDGroupSimple<true, true>, VariableByte>());
  map["copy"] = std::shared_ptr<IntegerCODEC>(new JustCopy());
  return map;
}

CodecMap CODECFactory::scodecmap = initializefactory();

} // namespace FastPFor

#endif /* CODECFACTORY_H_ */
