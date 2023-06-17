/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include <memory>
#include <iomanip>
#include <time.h>
#include "codecfactory.h"
#include "util.h"
#include "ztimer.h"
#include "bitpacking.h"
#include "synthetic.h"
#include "cpubenchmark.h"

using namespace std;
using namespace FastPForLib;

int main() {
  vector<shared_ptr<IntegerCODEC>> myalgos = CODECFactory::allSchemes();
  for (uint32_t b = 0; b <= 28; ++b) {

    cout << "testing... b = " << b << endl;

    for (size_t length = 128; length < (1U << 12); length += 128) {

      vector<uint32_t, cacheallocator> data(length);
      data.reserve(length + 1024);
      for (size_t i = 0; i < data.size(); ++i) {
        data[i] = (static_cast<uint32_t>(i) +
                   (24 - static_cast<uint32_t>(i)) *
                       (12 - static_cast<uint32_t>(i))) %
                  (1U << b);
      }
      for (vector<shared_ptr<IntegerCODEC>>::iterator i = myalgos.begin();
           i != myalgos.end(); ++i) {
        IntegerCODEC &c = **i;
        vector<uint32_t, cacheallocator> out(2 * data.size() + 1024);
        if (c.name() !=
            "VSEncoding") // VSEncoding is fragile, output must be zero
          for (uint32_t k = 0; k < out.size(); ++k)
            out[k] = rand();
        size_t nvalue = out.size();
        // next magic is to satisfy cachepacking aligment requirements; should
        // not hurt others
        uint32_t *aligned_out = &out[0];
        assert(!needPaddingTo64bytes(aligned_out));
        c.encodeArray(&data[0], data.size(), aligned_out, nvalue);
        out.resize(nvalue);
        vector<uint32_t, cacheallocator> recover(data.size() + 1024);

        for (uint32_t k = 0; k < recover.size(); ++k)
          recover[k] = rand();
        size_t recoveredsize = recover.size();
        c.decodeArray(aligned_out, out.size(), &recover[0], recoveredsize);
        recover.resize(recoveredsize);
        if (recover != data) {
          if (recoveredsize != data.size()) {
            cerr << "lengths don't match!" << endl;
            cerr << recoveredsize << endl;
            cerr << data.size() << endl;
          }
          uint32_t howmany = 0;
          for (size_t k = 0; k < data.size(); ++k)
            if (recover[k] != data[k]) {
              cout << "recovered[" << k << "] = " << recover[k] << endl;
              cout << "data[" << k << "] = " << data[k] << endl;
              howmany++;
              if (howmany > 15)
                break;
            }
          cerr << c.name() << endl;
          cerr << length << endl;
          throw logic_error("we have a bug");
        }
      }
    }

    for (size_t length = 0; length < (1U << 8); ++length) {
      vector<uint32_t, cacheallocator> data(length);
      data.reserve(length + 1024);
      for (size_t i = 0; i < data.size(); ++i) {
        data[i] =
            (33231 - static_cast<uint32_t>(i) + static_cast<uint32_t>(i * i)) %
            (1U << b);
      }
      for (vector<shared_ptr<IntegerCODEC>>::iterator i = myalgos.begin();
           i != myalgos.end(); ++i) {
        IntegerCODEC &c = **i;
        vector<uint32_t, cacheallocator> out(2 * data.size() + 1024);
        if (c.name() !=
            "VSEncoding") // VSEncoding is fragile, output must be zero
          for (uint32_t k = 0; k < out.size(); ++k)
            out[k] = rand();
        size_t nvalue = out.size();
        uint32_t *aligned_out = &out[0];
        assert(!needPaddingTo64bytes(aligned_out));
        c.encodeArray(data.data(), data.size(), aligned_out, nvalue);
        out.resize(nvalue);
        vector<uint32_t, cacheallocator> recover(data.size() + 1024);
        for (uint32_t k = 0; k < recover.size(); ++k)
          recover[k] = rand();
        size_t recoveredsize = recover.size();
        c.decodeArray(aligned_out, out.size(), &recover[0], recoveredsize);
        recover.resize(recoveredsize);
        if (recoveredsize != data.size()) {
          cerr << "lengths don't match!" << endl;
          cerr << recoveredsize << endl;
          cerr << data.size() << endl;
          throw logic_error("we have a bug");
        }
        if (!equal(recover.begin(), recover.end(), data.begin())) {

          uint32_t howmany = 0;
          for (size_t k = 0; k < data.size(); ++k)
            if (recover[k] != data[k]) {
              cout << "recovered[" << k << "] = " << recover[k] << endl;
              cout << "data[" << k << "] = " << data[k] << endl;
              howmany++;
              if (howmany > 15)
                break;
            }
          cerr << c.name() << endl;
          cerr << length << endl;
          throw logic_error("we have a bug");
        }
      }
    }
  }
  cout << " testing large zipfian arrays..." << endl;
  for (size_t lengthb = 5; lengthb <= 25; lengthb += 5) {
    uint32_t length = 1U << lengthb;
    cout << endl << "length = " << length << endl;
    vector<uint32_t, cacheallocator> data =
        generateZipfianArray32(length, 2, (1U << 9) - 1);
    data.reserve(data.size() + 1024);
    for (vector<shared_ptr<IntegerCODEC>>::iterator i = myalgos.begin();
         i != myalgos.end(); ++i) {
      IntegerCODEC &c = **i;
      cout << c.name();
      cout.flush();
      vector<uint32_t, cacheallocator> out(2 * data.size() + 1024);
      if (c.name() !=
          "VSEncoding") // VSEncoding is fragile, output must be zero
        for (uint32_t k = 0; k < out.size(); ++k)
          out[k] = rand();
      size_t nvalue = out.size();
      cout << " encoding ...";
      cout.flush();
      uint32_t *aligned_out = &out[0];
      assert(!needPaddingTo64bytes(aligned_out));
      c.encodeArray(&data[0], data.size(), aligned_out, nvalue);
      out.resize(nvalue);
      assert(out.size() == nvalue); // paranoid
      vector<uint32_t, cacheallocator> recover(data.size() + 1024);
      assert(recover.size() == data.size() + 1024); // being paranoid

      for (uint32_t k = 0; k < recover.size(); ++k)
        recover[k] = rand();
      size_t recoveredsize = recover.size();
      cout << " decoding ...";
      cout.flush();
      c.decodeArray(aligned_out, out.size(), &recover[0], recoveredsize);
      recover.resize(recoveredsize);
      if (recoveredsize != data.size()) {
        cerr << "lengths don't match!" << endl;
        cerr << recoveredsize << endl;
        cerr << data.size() << endl;
        throw logic_error("we have a bug");
      }
      if (!equal(recover.begin(), recover.end(), data.begin())) {
        uint32_t howmany = 0;
        for (size_t k = 0; k < data.size(); ++k)
          if (recover[k] != data[k]) {
            cout << "recovered[" << k << "] = " << recover[k] << endl;
            cout << "data[" << k << "] = " << data[k] << endl;
            howmany++;
            if (howmany > 15)
              break;
          }
        cerr << c.name() << endl;
        cerr << length << endl;
        throw logic_error("we have a bug");
      }
      cout << " ok!" << endl;
    }
  }

  cout << endl << endl << "testing...ok. Your code is good." << endl;
}
