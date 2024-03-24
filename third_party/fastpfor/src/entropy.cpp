/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#include "common.h"
#include "maropuparser.h"
#include "util.h"
#include "entropy.h"
#include "deltautil.h"

using namespace std;
using namespace FastPForLib;

void message(const char *prog) {
  cerr << " usage : " << prog << "  maropubinaryfile " << endl;
  cerr << "By default, is assumes that the original data is made of "
          "sorted integers."
       << endl;
  cerr << "The -nodelta flag disables delta coding." << endl;
  cerr << "The -minlength ignores all arrays smaller than a threshold." << endl;
}

int main(int argc, char **argv) {
  uint32_t MAXCOUNTER = 1U << 31;
  if (argc < 2) {
    message(argv[0]);
    return -1;
  }
  enum { DELTA, NODELTA };
  int mode = DELTA;
  uint32_t MINLENGTH = 2;
  int argindex = 1;
  while (true) {
    if (strcmp(argv[argindex], "-minlength") == 0) {
      ++argindex;
      MINLENGTH = atoi(argv[argindex++]);
    } else if (strcmp(argv[argindex], "-nodelta") == 0) {
      mode = NODELTA;
      ++argindex;
    } else
      break;
  }
  cout << "# computing entropy" << endl;
  string filename = argv[argindex++];
  if (argindex < argc)
    MAXCOUNTER = atoi(argv[argindex++]);
  cout << "# parsing " << filename << endl;
  MaropuGapReader reader(filename);
  vector<uint32_t, cacheallocator> rawdata;
  reader.open();
  size_t counter = 0;
  size_t integers = 0;
  EntropyRecorder er;
  while (reader.loadIntegers(rawdata)) {
    if (rawdata.size() < MINLENGTH)
      continue;
    if (mode == DELTA)
      Delta::delta(&rawdata[0], rawdata.size());
    er.eat(&rawdata[0], rawdata.size());
    ++counter;
    integers += rawdata.size();
    if (counter >= MAXCOUNTER) {
      cout << "#breaking early" << endl;
      break;
    }
  }
  reader.close();
  cout << "# integers = " << integers << endl;
  cout << "# arrays = " << counter << endl;
  cout << "# next line is shannon entropy and data bits" << endl;
  cout << er.computeShannon() << "\t" << er.computeDataBits() << endl;
}
