/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include <vector>
#include "maropuparser.h"
#include "util.h"

using namespace std;
using namespace FastPForLib;

int main(int argc, char **argv) {
  BitWidthHistoGram histo;
  BitWidthHistoGram histolength;
  if (argc < 2) {
    cerr << "please provide a file name" << endl;
    return -1;
  }
  size_t MINLENGTH = 2;
  size_t MAXCOUNT = 200000;
  int argindex = 1;
  while (true) {
    if (strcmp(argv[argindex], "-minlength") == 0) {
      ++argindex;
      MINLENGTH = atoi(argv[argindex++]);
    } else if (strcmp(argv[argindex], "-maxcount") == 0) {
      ++argindex;
      MAXCOUNT = atoi(argv[argindex++]);
    } else
      break;
  }
  string filename = argv[argindex++];
  cout << "# parsing " << filename << endl;
  MaropuGapReader reader(filename);
  vector<uint32_t> rawdata;
  reader.open();
  uint32_t counter = 0;
  while (reader.loadIntegers(rawdata)) {
    if (rawdata.size() < MINLENGTH)
      continue;
    ++counter;
    histo.eatDGaps(rawdata);
    histolength.histo[gccbits(static_cast<uint32_t>(rawdata.size()))] += 1;
    if (counter + 1 > MAXCOUNT) {
      cout << "# bailing out early " << (counter) << endl;
      break;
    }
  }
  cout << "#bit width" << endl;
  histo.display();
  cout << "#lengths:" << endl;
  histolength.display("#");
  reader.close();
  return 0;
}
