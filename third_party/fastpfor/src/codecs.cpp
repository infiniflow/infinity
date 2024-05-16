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

#include <string.h>
#include <time.h>
#include <cstdlib>
#include <ctype.h>
#include <memory>
#include <iomanip>
#include "codecfactory.h"
#include "ztimer.h"
#include "synthetic.h"
#include "cpubenchmark.h"
#include "stringutil.h"
#include "entropy.h"
#include "memutil.h"
#include "deltautil.h"

using namespace std;
using namespace FastPForLib;

static struct option long_options[] = {
    {"uniformsparseclassic", no_argument, 0, 0},
    {"displayhistogram", no_argument, 0, 'H'},
    {"uniformdenseclassic", no_argument, 0, 0},
    {"clustersparseclassic", no_argument, 0, 0},
    {"clusterdenseclassic", no_argument, 0, 0},
    {"uniformsparse", no_argument, 0, 0},
    {"uniformdense", no_argument, 0, 0},
    {"clustersparse", no_argument, 0, 0},
    {"clusterdense", no_argument, 0, 0},
    {"zipfian1", no_argument, 0, 0},
    {"zipfian2", no_argument, 0, 0},
    {"vclusterdynamic", no_argument, 0, 0},
    {"crazyclusterdynamic", no_argument, 0, 0},
    {"clusterdynamicsmall", no_argument, 0, 0},
    {"uniformdynamicsmall", no_argument, 0, 0},
    {"clusterdynamic", no_argument, 0, 0},
    {"uniformdynamic", no_argument, 0, 0},
    {"clusterdynamicpredelta", no_argument, 0, 0},
    {"uniformdynamicpredelta", no_argument, 0, 0},
    {"sillyuniformdynamic", no_argument, 0, 0},
    {"codecs", required_argument, 0, 'c'},
    {"splitlongarrays", no_argument, 0, 'S'},
    {"short", no_argument, 0, 's'},
    {0, 0, 0, 0}};

void message() {
  int c = 0;
  cout << "You must select one of these options:" << endl;
  while (long_options[c].name != 0) {
    if ((strcmp(long_options[c].name, "codecs") == 0) ||
        ((strcmp(long_options[c].name, "short") == 0)))
      ++c;
    else
      cout << "--" << long_options[c++].name << endl;
  }
  cout << "the --codecs flag takes as an argument"
          " a comma-separated list of schemes chosen among those:"
       << endl;
  CODECFactory factory;
  vector<string> all = factory.allNames();
  for (auto i = all.begin(); i != all.end(); ++i) {
    cout << *i;
    if (i + 1 == all.end())
      cout << endl;
    else
      cout << ",";
  }
  cout << "(or give NONE instead of a list)" << endl;
  cout << "The --displayhistogram flag must appear before the other flags."
       << endl;
  cout << "Note that the --codecs flag must appear before the other flags."
       << endl;
  cout << "You can get a more concise output by using the --short flag."
       << endl;
}

int main(int argc, char **argv) {
  srand(10); // time(NULL));
  bool fulldisplay = true;
  bool displayhistogram = false;
  bool computeentropy = false;

  bool splitlongarrays = true;
  CODECFactory factory;
  vector<shared_ptr<IntegerCODEC>> tmp =
      factory.allSchemes(); // the default
  vector<algostats> myalgos;
  for (auto &i : tmp) {
    myalgos.push_back(algostats(i));
    myalgos.push_back(algostats(i, true)); // by default?
  }
  int c;
  while (1) {
    int option_index = 0;
    c = getopt_long(argc, argv, "Sfec:", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 'S':
      cout << "#\n# disabling partition of big arrays. Performance may "
              "suffer.#\n"
           << endl;
      splitlongarrays = false;
      break;
    case 'c': {
      myalgos.clear();
      string codecsstr(optarg);
      if (codecsstr.compare("NONE") != 0) {
        vector<string> codecslst = split(codecsstr, ",:;");
        for (auto i = codecslst.begin(); i != codecslst.end(); ++i) {
          cout << "# pretty name = " << *i << endl;
          if (i->at(0) == '@') { // SIMD
            myalgos.push_back(algostats(
                factory.getFromName(i->substr(1, i->size() - 1)), true));
          } else {
            myalgos.push_back(algostats(factory.getFromName(*i)));
          }
          cout << "# added '" << myalgos.back().name() << "'" << endl;
        }
      }
    } break;
    case 'e':
      computeentropy = true;
      break;
    case 'H':
      displayhistogram = true;
      break;
    case 's':
      fulldisplay = false;
      break;
    case 'f':
      fulldisplay = true;
      break;
    case 0: {
      if (optind < argc) {
        cout << "There are some trailing flags...?" << endl;
        message();
        return 0;
      }
      const char *parameter = long_options[option_index].name;
      cout << "# found " << parameter << endl;
      if (strcmp(parameter, "zipfian1") == 0) {
        const uint32_t N = 4194304 * 16;
        vector<vector<uint32_t, cacheallocator>> datas;
        cout << "# zipfian 1 data generation..." << endl;
        for (uint32_t k = 0; k < (1U << 1); ++k)
          datas.push_back(generateZipfianArray32(N, 1.0, 1U << 20));
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(false, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "zipfian2") == 0) {
        const uint32_t N = 4194304 * 16;
        vector<vector<uint32_t, cacheallocator>> datas;
        for (uint32_t k = 0; k < (1U << 1); ++k)
          cout << "# zipfian 2 data generation..." << endl;
        datas.push_back(generateZipfianArray32(N, 2.0, 1U << 20));
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(false, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformdenseclassic") == 0) {
        cout << "# dense uniform data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        UniformDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 5); ++k)
          datas.push_back(clu.generateUniform((1U << 18), 1U << 27));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformsparseclassic") == 0) {
        cout << "# sparse uniform data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        UniformDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 14);
             ++k) // by original paper should be 1U<<19
          datas.push_back(clu.generateUniform((1U << 9), 1U << 27));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clusterdenseclassic") == 0) {
        cout << "# dense cluster data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        ClusteredDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 5);
             ++k) // by original paper should be 1U<<10
          datas.push_back(clu.generateClustered((1U << 18), 1U << 27));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clustersparseclassic") == 0) {
        cout << "# sparse cluster data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        ClusteredDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 14);
             ++k) // by original paper should be 1U<<19
          datas.push_back(clu.generateClustered((1U << 9), 1U << 27));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformdense") == 0) {
        cout << "# dense uniform data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        UniformDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 3);
             ++k) // by original paper should be 1U<<10
          datas.push_back(clu.generateUniform((1U << 22), 1U << 29));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformsparse") == 0) {
        cout << "# sparse uniform data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        UniformDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 13); ++k)
          datas.push_back(clu.generateUniform((1U << 12), 1U << 29));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clusterdense") == 0) {
        cout << "# dense cluster data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        ClusteredDataGenerator clu;
        for (uint32_t k = 0; k < 1; ++k)
          datas.push_back(clu.generateClustered((1U << 23), 1U << 26));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clustersparse") == 0) {
        cout << "# sparse cluster data generation..." << endl;
        vector<vector<uint32_t, cacheallocator>> datas;
        ClusteredDataGenerator clu;
        for (uint32_t k = 0; k < (1U << 13); ++k)
          datas.push_back(clu.generateClustered((1U << 12), 1U << 26));
        cout << "# generated " << datas.size() << " arrays" << endl;
        if (splitlongarrays)
          splitLongArrays(datas);
        processparameters pp(true, fulldisplay, displayhistogram,
                             computeentropy, false);
        Delta::process(myalgos, datas, pp);
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clusterdynamic") == 0) {
        cout << "# dynamic clustered data generation..." << endl;
        ClusteredDataGenerator clu;
        for (uint32_t K = 15; K <= 25; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (uint32_t k = 0; k < (1U << (25 - K)); ++k)
            datas.push_back(clu.generateClustered((1U << K), 1U << 29));
          cout << "# generated " << datas.size() << " arrays" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(true, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformdynamic") == 0) {
        cout << "# sparse uniform data generation..." << endl;
        UniformDataGenerator clu;
        for (uint32_t K = 15; K <= 25; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (uint32_t k = 0; k < (1U << (25 - K)); ++k)
            datas.push_back(clu.generateUniform((1U << K), 1U << 29));
          cout << "# generated " << datas.size() << " arrays" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(true, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clusterdynamicsmall") == 0) {
        cout << "# dynamic clustered data generation..." << endl;
        ClusteredDataGenerator clu;
        for (uint32_t K = 15; K <= 20; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (size_t k = 0; k < static_cast<size_t>(1U << (20 - K)); ++k)
            datas.push_back(clu.generateClustered((1U << K), 1U << 29));
          cout << "# generated " << datas.size() << " arrays" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(true, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformdynamicsmall") == 0) {
        cout << "# sparse uniform data generation..." << endl;
        UniformDataGenerator clu;
        for (uint32_t K = 15; K <= 20; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (size_t k = 0; k < static_cast<size_t>(1U << (20 - K)); ++k)
            datas.push_back(clu.generateUniform((1U << K), 1U << 29));
          cout << "# generated " << datas.size() << " arrays" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(true, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "clusterdynamicpredelta") == 0) {
        cout << "# dynamic clustered data generation..." << endl;
        ClusteredDataGenerator clu;
        for (uint32_t K = 15; K <= 25; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (size_t k = 0; k < static_cast<size_t>(1U << (25 - K)); ++k)
            datas.push_back(
                diffs(clu.generateClustered((1U << K), 1U << 29), false));
          cout << "# generated " << datas.size()
               << " arrays and applied delta coding" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(false, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else if (strcmp(parameter, "uniformdynamicpredelta") == 0) {
        cout << "# sparse uniform data generation..." << endl;
        UniformDataGenerator clu;
        for (uint32_t K = 15; K <= 25; K += 5) {
          vector<vector<uint32_t, cacheallocator>> datas;
          for (size_t k = 0; k < static_cast<size_t>(1U << (25 - K)); ++k)
            datas.push_back(
                diffs(clu.generateUniform((1U << K), 1U << 29), false));
          cout << "# generated " << datas.size()
               << " arrays and applied delta coding" << endl;
          cout << "# their size is  " << (1U << K) << endl;
          const uint32_t p = 29 - K;
          if (splitlongarrays)
            splitLongArrays(datas);
          processparameters pp(false, fulldisplay, displayhistogram,
                               computeentropy, false);
          Delta::process(myalgos, datas, pp, std::to_string(p));
        }
        summarize(myalgos, "#");
        return 0;
      } else {
        cerr << "Support for " << parameter << " was not found." << endl;
      }
      break;
    }
    default:
      /* You won't actually get here. */
      break;
    }
  }
  message();
  return 0;
}
