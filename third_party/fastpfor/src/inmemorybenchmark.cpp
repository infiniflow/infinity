/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#ifndef _MSC_VER
#include <getopt.h>
#else 
#include "../msvc/getopt.h"
#endif

#include "common.h"
#include "codecfactory.h"
#include "maropuparser.h"
#include "util.h"
#include "ztimer.h"
#include "cpubenchmark.h"
#include "deltautil.h"
#include "stringutil.h"
#include <sstream>

using namespace std;
using namespace FastPForLib;

static struct option long_options[] = {
    {"codecs", required_argument, 0, 'c'},
    {"minlength", required_argument, 0, 'm'},
    {"maxlength", required_argument, 0, 'M'},
    {"nb", required_argument, 0, 'n'},
    {"splitlongarrays", no_argument, 0, 'S'},
    {"separatedeltatimes", no_argument, 0, 'D'},
    {0, 0, 0, 0}};

void message(const char *prog) {
  cerr << " usage : " << prog << " scheme  maropubinaryfile " << endl;
  cerr << "By default, it assumes that the original data is made of "
          "sorted distinct integers."
       << endl;
  cerr << "Use the --codecs flag to specify the schemes." << endl;
  cerr << " schemes include:" << endl;
  CODECFactory factory;
  vector<string> all = factory.allNames();
  for (auto i = all.begin(); i != all.end(); ++i) {
    cerr << *i << endl;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    message(argv[0]);
    return -1;
  }
  bool splitlongarrays = true;
  bool separatedeltatimes = false;
  size_t MINLENGTH = 1;
  size_t MAXLENGTH = (std::numeric_limits<uint32_t>::max)();
  size_t MAXCOUNTER = (std::numeric_limits<std::size_t>::max)();
  CODECFactory factory;
  vector<shared_ptr<IntegerCODEC>> tmp =
      factory.allSchemes(); // the default
  vector<algostats> myalgos;
  for (auto i = tmp.begin(); i != tmp.end(); ++i) {
    myalgos.push_back(algostats(*i));
    myalgos.push_back(algostats(*i, true)); // by default?
  }
  int c;
  while (1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "DSc:n:m:M:", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 'D':
      cout << "# Outputting separate delta and inverseDelta times." << endl;
      separatedeltatimes = true;
      break;
    case 'S':
      cout << "#\n# disabling partition of big arrays. Performance may "
              "suffer.\n#"
           << endl;
      splitlongarrays = false;
      break;
    case 'm':
      istringstream(optarg) >> MINLENGTH;
      cout << "# MINLENGTH = " << MINLENGTH << endl;
      break;
    case 'M':
      istringstream(optarg) >> MAXLENGTH;
      cout << "# MAXLENGTH = " << MAXLENGTH << endl;
      break;
    case 'n':
      istringstream(optarg) >> MAXCOUNTER;
      cout << "# MAXCOUNTER = " << MAXCOUNTER << endl;
      break;
    case 'c': {
      myalgos.clear();
      string codecsstr(optarg);
      if (codecsstr.compare("NONE") != 0) {
        vector<string> codecslst = split(codecsstr, ",:;");
        for (auto i = codecslst.begin(); i != codecslst.end(); ++i) {
          cout << "# pretty name = " << *i << endl;
          if (i->at(0) == '@') { // SIMD
            string namewithoutprefix = i->substr(1, i->size() - 1);
            myalgos.push_back(
                algostats(factory.getFromName(namewithoutprefix), true));
          } else {
            myalgos.push_back(algostats(factory.getFromName(*i)));
          }
          cout << "# added '" << myalgos.back().name() << "'" << endl;
        }
      }
    } break;
    default:
      cerr << "unrecognized flag" << endl;
      break;
    }
  }

  if (argc - optind < 1) {
    cerr << "  you must specify a file " << endl;
    return -1;
  }
  string filename = argv[optind];

  cout << "# parsing " << filename << endl;
  MaropuGapReader reader(filename);

  vector<uint32_t, cacheallocator> rawdata;
  reader.open();
  size_t counter = 0;
  const size_t MAXBLOCKSIZE = 104857600; // 400 MB
  while (counter < MAXCOUNTER) {
    // collecting the data up to MAXBLOCKSIZE integers
    vector<vector<uint32_t, cacheallocator>> datas;
    size_t datastotalsize = 0;
    cout << "# Filling up a block " << endl;
    while (reader.loadIntegers(rawdata)) {
      if ((rawdata.size() < MINLENGTH) || (rawdata.size() > MAXLENGTH))
        continue;
      ++counter;
      datastotalsize += rawdata.size();
      datas.push_back(rawdata);
      if (counter >= MAXCOUNTER) {
        cout << "#breaking early" << endl;
        break;
      }
      if (datastotalsize >= MAXBLOCKSIZE)
        break;
    }
    if (datastotalsize == 0)
      break;
    cout << "# read " << std::setprecision(3)
         << static_cast<double>(datastotalsize) * 4.0 / (1024.0 * 1024.0)
         << " MB, " << datas.size() << " arrays." << endl;

    cout << "# processing block" << endl;
    if (splitlongarrays)
      splitLongArrays(datas);
    processparameters pp(true, false, false, false, true, separatedeltatimes);
    Delta::process(myalgos, datas,
                   pp); // done collecting data, now allocating memory
  }
  reader.close();
  cout << "# build summary..." << endl;
  summarize(myalgos);
}
