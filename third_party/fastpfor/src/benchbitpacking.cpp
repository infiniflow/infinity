/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#include <exception>
#include <iostream>
#include <iomanip>
#include <vector>
#include "util.h"
#include "bitpackinghelpers.h"
#include "simdbitpacking.h"
#include "rolledbitpacking.h"
#include "synthetic.h"
#include "ztimer.h"
#include "horizontalbitpacking.h"

using namespace std;
using namespace FastPForLib;

void maskfnc(vector<uint32_t, cacheallocator> &out, const uint32_t L) {
  if (L == 32)
    return;
  for (auto i = out.begin(); i != out.end(); ++i) {
    *i = *i % (1U << L);
  }
}

void fastpack(const vector<uint32_t, cacheallocator> &data,
              vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    fastpack(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void fastpackwithoutmask(const vector<uint32_t, cacheallocator> &data,
                         vector<uint32_t, cacheallocator> &out,
                         const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    fastpackwithoutmask(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void fastunpack(const vector<uint32_t, cacheallocator> &data,
                vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = out.size();
  for (size_t k = 0; k < N / 32; ++k) {
    fastunpack(&data[0] + bit * k, &out[0] + 32 * k, bit);
  }
}

void simdpack(const vector<uint32_t, cacheallocator> &data,
              vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 128; ++k) {
    SIMD_fastpack_32(&data[0] + 128 * k,
                     reinterpret_cast<__m128i *>(&out[0] + 4 * bit * k), bit);
  }
}

void simdpackwithoutmask(const vector<uint32_t, cacheallocator> &data,
                         vector<uint32_t, cacheallocator> &out,
                         const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 128; ++k) {
    SIMD_fastpackwithoutmask_32(
        &data[0] + 128 * k, reinterpret_cast<__m128i *>(&out[0] + 4 * bit * k),
        bit);
  }
}

void simdunpack(const vector<uint32_t, cacheallocator> &data,
                vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = out.size();
  for (size_t k = 0; k < N / 128; ++k) {
    SIMD_fastunpack_32(
        reinterpret_cast<const __m128i *>(&data[0] + 4 * bit * k),
        &out[0] + 128 * k, bit);
  }
}

void horizontalunpack(const vector<uint32_t, cacheallocator> &data,
                      vector<uint32_t, cacheallocator> &out,
                      const uint32_t bit) {
  const size_t N = out.size();
  for (size_t k = 0; k < N / 128; ++k) {
    simdhunpack(reinterpret_cast<const uint8_t *>(&data[0] + 4 * bit * k),
                &out[0] + 128 * k, bit);
  }
}

void pack(const vector<uint32_t, cacheallocator> &data,
          vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    pack<true>(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void packwithoutmask(const vector<uint32_t, cacheallocator> &data,
                     vector<uint32_t, cacheallocator> &out,
                     const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    pack<false>(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void pack_tight(const vector<uint32_t, cacheallocator> &data,
                vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    pack_tight<true>(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void pack_tightwithoutmask(const vector<uint32_t, cacheallocator> &data,
                           vector<uint32_t, cacheallocator> &out,
                           const uint32_t bit) {
  const size_t N = data.size();
  for (size_t k = 0; k < N / 32; ++k) {
    pack_tight<false>(&data[0] + 32 * k, &out[0] + bit * k, bit);
  }
}

void unpack_tight(const vector<uint32_t, cacheallocator> &data,
                  vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = out.size();
  for (size_t k = 0; k < N / 32; ++k) {
    unpack_tight(&data[0] + bit * k, &out[0] + 32 * k, bit);
  }
}

void unpack(const vector<uint32_t, cacheallocator> &data,
            vector<uint32_t, cacheallocator> &out, const uint32_t bit) {
  const size_t N = out.size();
  for (size_t k = 0; k < N / 32; ++k) {
    unpack(&data[0] + bit * k, &out[0] + 32 * k, bit);
  }
}

template <class container32bit>
bool equalOnFirstBits(const container32bit &data,
                      const container32bit &recovered, uint32_t bit) {
  if (bit == 32) {
    return data == recovered;
  }
  for (size_t k = 0; k < data.size(); ++k) {
    if (data[k] % (1U << bit) != recovered[k] % (1U << bit)) {
      cout << " They differ at k = " << k << " data[k]= " << data[k]
           << " recovered[k]=" << recovered[k] << endl;
      return false;
    }
  }
  return true;
}
void simplebenchmark(uint32_t N = 1U << 16, uint32_t T = 1U << 9) {
  T = T + 1; // we have a warming up pass
  vector<uint32_t, cacheallocator> data = generateArray32(N);
  vector<uint32_t, cacheallocator> compressed(N, 0);
  vector<uint32_t, cacheallocator> recovered(N, 0);
  WallClockTimer z;
  uint64_t packtime, packtimewm, unpacktime;
  uint64_t simdpacktime, simdpacktimewm, simdunpacktime;
  uint64_t horizontalunpacktime;

  cout << "#million of integers per second: higher is better" << endl;
  cout << "#bit, pack, pack without mask, unpack" << endl;
  for (uint32_t bitindex = 0; bitindex < 32; ++bitindex) {
    uint32_t bit = 32 - bitindex;
    maskfnc(data, bit);
    for (uint32_t repeat = 0; repeat < 1; ++repeat) {
      packtime = 0;
      packtimewm = 0;
      unpacktime = 0;
      simdpacktime = 0;
      simdpacktimewm = 0;
      simdunpacktime = 0;
      horizontalunpacktime = 0;

      for (uint32_t t = 0; t < T; ++t) {
        compressed.clear();
        compressed.resize(N * bit / 32, 0);
        recovered.clear();
        recovered.resize(N, 0);
        simdpack(data, compressed, bit);
        simdunpack(compressed, recovered, bit);
        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bugs!" << bit << endl;
          return;
        }

        z.reset();
        simdpack(data, compressed, bit);
        if (t > 0)
          simdpacktime += z.split();
        simdunpack(compressed, recovered, bit);
        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bugs!" << bit << endl;
          return;
        }

        z.reset();
        simdpackwithoutmask(data, compressed, bit);
        if (t > 0)
          simdpacktimewm += z.split();

        z.reset();
        simdunpack(compressed, recovered, bit);
        if (t > 0)
          simdunpacktime += z.split();

        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bugs!" << bit << endl;
          return;
        }

        z.reset();
        fastpack(data, compressed, bit);
        if (t > 0)
          packtime += z.split();
        fastunpack(compressed, recovered, bit);
        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bug1!" << endl;
          return;
        }

        z.reset();
        fastpackwithoutmask(data, compressed, bit);
        if (t > 0)
          packtimewm += z.split();

        z.reset();
        fastunpack(compressed, recovered, bit);
        if (t > 0)
          unpacktime += z.split();

        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bug1!" << endl;
          return;
        }

        z.reset();
        horizontalunpack(compressed, recovered, bit);
        if (t > 0)
          horizontalunpacktime += z.split();

        if (!equalOnFirstBits(data, recovered, bit)) {
          cout << " Bug1!" << endl;
          return;
        }
      }

      cout << std::setprecision(4) << bit << "\t\t"
           << N * (T - 1) / double(packtime) << "\t\t"
           << N * (T - 1) / double(packtimewm) << "\t\t\t"
           << N * (T - 1) / double(unpacktime) << "\t\t";

      cout << std::setprecision(4) << bit << "\t\t"
           << N * (T - 1) / double(simdpacktime) << "\t\t"
           << N * (T - 1) / double(simdpacktimewm) << "\t\t"
           << N * (T - 1) / double(simdunpacktime) << "\t\t";
      cout << N * (T - 1) / double(horizontalunpacktime) << "\t\t";

      cout << endl;
    }
  }
}

int main() {
  cout << "# cache-to-cache" << endl;
  simplebenchmark(1U << 16, 1U << 9);
  cout << endl;
  cout << "# ram-to-ram" << endl;
  simplebenchmark(1U << 25, 1U << 0);

  return 0;
}
