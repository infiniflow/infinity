/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef CODECFACTORY_H_
#define CODECFACTORY_H_

#include "codecs.h"

namespace FastPForLib {

typedef std::map<std::string, std::shared_ptr<IntegerCODEC>> CodecMap;

/**
 * You should have at least one factory per thread.
 */
class CODECFactory {
public:
  CODECFactory();

  std::vector<std::shared_ptr<IntegerCODEC>> allSchemes();

  std::vector<std::string> allNames();

  std::shared_ptr<IntegerCODEC> &getFromName(std::string name);
private:
  CodecMap scodecmap;
};

} // namespace FastPForLib

#endif /* CODECFACTORY_H_ */
