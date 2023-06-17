/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
/**
 * The ideas is to partition a database of arrays
 * according to their length. We partition by the
 * integer logarithm of the length so that arrays
 * having length from 2^L to 2^L-1 will be stored
 * together.
 */

#include <sstream>
#include <vector>
#include "maropuparser.h"
#include "util.h"

using namespace std;
using namespace FastPForLib;

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "please provide an input file name" << endl;
    return -1;
  }
  int argindex = 1;

  string filename = argv[argindex++];
  cout << "# parsing " << filename << endl;
  MaropuGapReader reader(filename);
  vector<uint32_t> rawdata;
  reader.open();
  map<uint32_t, FILE *> output;
  map<uint32_t, size_t> counter;
  map<uint32_t, string> name;
  while (reader.loadIntegers(rawdata)) {
    uint32_t lengthinbits = gccbits(static_cast<uint32_t>(rawdata.size()));
    if (output.find(lengthinbits) == output.end()) {
      ostringstream o;
      o << filename << "." << lengthinbits;
      cout << "creating output file " << o.str() << endl;
      FILE *fd = ::fopen(o.str().c_str(), "w+b");
      if (fd == NULL) {
        cerr << strerror(errno) << endl;
        cerr << "can't open " << o.str().c_str() << endl;
        break;
      }
      setvbuf(fd, NULL, _IOFBF, 1024 * 4); // large buffer
      output[lengthinbits] = fd;
      counter[lengthinbits] = 0;
      name[lengthinbits] = o.str();
    }
    uint32_t thislength = static_cast<uint32_t>(rawdata.size());
    if (fwrite(&thislength, sizeof(thislength), 1, output[lengthinbits]) != 1) {
      cerr << "problem writing" << endl;
      break;
    }
    if (fwrite(&rawdata[0], sizeof(uint32_t), thislength,
               output[lengthinbits]) != thislength) {
      cerr << "problem writing" << endl;
      break;
    }
    counter[lengthinbits]++;
  }
  auto i = output.begin();
  auto j = counter.begin();
  auto k = name.begin();
  for (; i != output.end(); ++i, ++j, ++k) {
    cout << "file " << k->second << " contains " << j->second << " arrays"
         << endl;
    ::fclose(i->second);
  }
  reader.close();
  return 0;
}
