/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef DELTAUTIL_H_
#define DELTAUTIL_H_
#include <vector>
#include <exception>
#include "common.h"
#include "codecs.h"
#include "memutil.h"
#include "entropy.h"
#include "ztimer.h"

namespace FastPForLib {

/**
 * This file is made of various convenient functions and structures.
 * It is not necessarily very reusable though.
 */
struct algostats {

  algostats(std::shared_ptr<IntegerCODEC> &a, bool simd = false)
      : algo(a), deltaspeed(), compspeed(), decompspeed(), inversedeltaspeed(),
        bitsperint(), deltatime(0), comptime(0), decomptime(0),
        inversedeltatime(0), output(), input(), SIMDDeltas(simd) {}
  std::string name() {
    // if SIMDDeltas is "true", we prepend @
    if (SIMDDeltas) {
      std::ostringstream convert;
      convert << "@" << algo->name();
      return convert.str();
    }
    return algo->name();
  }
  std::string name(size_t k) {
    std::string n = name();
    char space = ' ';
    n.resize(k, space);
    return n;
  }

  std::shared_ptr<IntegerCODEC> algo;

  std::vector<double> deltaspeed;
  std::vector<double> compspeed;
  std::vector<double> decompspeed;
  std::vector<double> inversedeltaspeed;
  std::vector<double> bitsperint;

  double deltatime;
  double comptime;
  double decomptime;
  double inversedeltatime;
  uint64_t output;
  uint64_t input;

  bool SIMDDeltas;
};

void summarize(std::vector<algostats> &v, std::string prefix = "#") {
  if (v.empty())
    return;
  std::cout << "# building summary " << std::endl;
  size_t N = v[0].bitsperint.size();
  for (size_t k = 0; k < N; ++k) {
    std::cout << "###################" << std::endl;
    if (N > 1)
      std::cout << "#test " << (k + 1) << " of " << N << std::endl;
    std::cout << "#wall clock (delta mis, comp mis, decomp mis, idelta mis, "
                 "bits per int)"
              << std::endl;
    std::cout << "#" << std::endl;
    for (auto i = v.begin(); i != v.end(); ++i) {
      double deltaspeed_k =
          (k < i->deltaspeed.size() ? i->deltaspeed[k] : -1.0);
      double compspeed_k = (k < i->compspeed.size() ? i->compspeed[k] : -1.0);
      double decompspeed_k =
          (k < i->decompspeed.size() ? i->decompspeed[k] : -1.0);
      double inversedeltaspeed_k =
          (k < i->inversedeltaspeed.size() ? i->inversedeltaspeed[k] : -1.0);
      double bitsperint_k =
          (k < i->bitsperint.size() ? i->bitsperint[k] : -1.0);
      std::cout << prefix << std::setprecision(4) << i->name(40) << " \t "
                << deltaspeed_k << " \t " << compspeed_k << " \t "
                << decompspeed_k << " \t " << inversedeltaspeed_k << " \t "
                << bitsperint_k << std::endl;
    }
    std::cout << prefix << std::endl << prefix << std::endl;
  }
  std::cout << " codec:                                  enc/mis dec/mis "
               "bits/i enc(ms) dec(ms)"
            << std::endl;
  for (algostats a : v) {
    double deltaTime = a.deltatime + a.inversedeltatime;
    double totTime = deltaTime + a.comptime + a.decomptime;
    if (totTime > 0 && a.input > 0) {
      std::cout << " " << std::setprecision(1) << std::fixed << a.name(40);
      double input = static_cast<double>(a.input);
      if (deltaTime > 0) {
        std::cout << input / a.deltatime << " \t " << input / a.comptime
                  << " \t " << input / a.decomptime << " \t "
                  << input / a.inversedeltatime << " \t "
                  << static_cast<double>(a.output) * 32.0 / input << " \t\t"
                  << "TotalTimes (ms):  "
                  << static_cast<size_t>(a.deltatime / 1000) << " \t "
                  << static_cast<size_t>(a.comptime / 1000) << " \t "
                  << static_cast<size_t>(a.decomptime / 1000) << " \t "
                  << static_cast<size_t>(a.inversedeltatime / 1000)
                  << std::endl;
      } else {
        std::cout << std::setw(7) << input / a.comptime << " " << std::setw(7)
                  << input / a.decomptime << " " << std::setw(6)
                  << a.output * 32.0 / input << " " << std::setw(7)
                  << a.comptime / 1000 << " " << std::setw(7)
                  << a.decomptime / 1000 << std::endl;
      }
    }
  }
}

/**
 * This takes every std::vector and replaces it, if
 * needed by a series of std::vectors having max
 * size MAXSIZE. If a std::vector has size less than
 * or equal to MAXSIZE, it remains unchanged.
 */
template <class T>
void splitLongArrays(std::vector<T> &datas, size_t MAXSIZE = 65536) {
  // possibly inefficient
  for (size_t i = 0; i < datas.size(); ++i) {
    if (datas[i].size() > MAXSIZE) {
      const size_t howmany = (datas[i].size() + MAXSIZE - 1) / MAXSIZE - 1;
      datas.reserve(datas.size() + howmany);
      for (size_t j = 0; j < howmany; ++j) {
        size_t begin = (j + 1) * MAXSIZE;
        size_t end = (j + 2) * MAXSIZE;
        if (end > datas[i].size())
          end = datas[i].size();
        assert(datas[i].begin() + end <= datas[i].end());
        datas.push_back(T(datas[i].begin() + begin, datas[i].begin() + end));
      }
      datas[i].resize(MAXSIZE);
    }
  }
  // for safety, we check the result:
  for (size_t i = 0; i < datas.size(); ++i) {
    assert(datas[i].size() <= MAXSIZE);
  }
}

struct processparameters {
  bool needtodelta;
  bool fulldisplay;
  bool displayhistogram;
  bool computeentropy;
  bool cumulative;
  bool separatetimes;

  processparameters(bool ndelta, bool fdisplay, bool dhisto, bool compentropy,
                    bool cumul, bool separate = false)
      : needtodelta(ndelta), fulldisplay(fdisplay), displayhistogram(dhisto),
        computeentropy(compentropy), cumulative(cumul),
        separatetimes(separate) {}
};
/**
 * This class encodes and decode data using
 * an existing IntegerCODEC and delta coding.
 * It organizes the data by pages to avoid cache
 * misses.
 */
class Delta {
public:
  //  by D. Lemire
  template <class T> static void delta(T *data, const size_t size) {
    if (size == 0)
      throw std::runtime_error("delta coding impossible with no value!");
    for (size_t i = size - 1; i > 0; --i) {
      data[i] -= data[i - 1];
    }
  }

  template <class T> static void fastDelta(T *pData, const size_t TotalQty) {
    if (TotalQty < 5) {
      delta(pData, TotalQty); // no SIMD
      return;
    }

    const size_t Qty4 = TotalQty / 4;
    __m128i *pCurr = reinterpret_cast<__m128i *>(pData);
    const __m128i *pEnd = pCurr + Qty4;

    __m128i last = _mm_setzero_si128();
    while (pCurr < pEnd) {
      __m128i a0 = _mm_load_si128(pCurr);
      __m128i a1 = _mm_sub_epi32(a0, _mm_srli_si128(last, 12));
      a1 = _mm_sub_epi32(a1, _mm_slli_si128(a0, 4));
      last = a0;

      _mm_store_si128(pCurr++, a1);
    }

    if (Qty4 * 4 < TotalQty) {
      uint32_t lastVal = _mm_cvtsi128_si32(_mm_srli_si128(last, 12));
      for (size_t i = Qty4 * 4; i < TotalQty; ++i) {
        uint32_t newVal = pData[i];
        pData[i] -= lastVal;
        lastVal = newVal;
      }
    }
  }

  // by Leonid Boytsov, revised by D. Lemire
  template <class T> static void deltaSIMD(T *pData, const size_t TotalQty) {
    if (TotalQty < 5) {
      delta(pData, TotalQty); // no need for SIMD
      return;
    }
    const size_t Qty4 = TotalQty / 4;
    for (size_t i = 4 * Qty4; i < TotalQty; ++i) {
      pData[i] -= pData[i - 4];
    }
    __m128i *pCurr = reinterpret_cast<__m128i *>(pData) + Qty4 - 1;
    const __m128i *pStart = reinterpret_cast<__m128i *>(pData);
    __m128i a = _mm_load_si128(pCurr);
    while (pCurr > pStart) {
      __m128i b = _mm_load_si128(pCurr - 1);
      _mm_store_si128(pCurr--, _mm_sub_epi32(a, b));
      a = b;
    }
  }

  // by Leonid Boytsov, modified by D. Lemire
  template <class T>
  static void inverseDeltaSIMD(T *pData, const size_t TotalQty) {
    if (TotalQty < 5) {
      inverseDelta(pData, TotalQty); // no SIMD
      return;
    }
    const size_t Qty4 = TotalQty / 4;

    __m128i *pCurr = reinterpret_cast<__m128i *>(pData);
    const __m128i *pEnd = pCurr + Qty4;
    __m128i a = _mm_load_si128(pCurr++);
    while (pCurr < pEnd) {
      __m128i b = _mm_load_si128(pCurr);
      a = _mm_add_epi32(a, b);
      _mm_store_si128(pCurr++, a);
    }

    for (size_t i = Qty4 * 4; i < TotalQty; ++i) {
      pData[i] += pData[i - 4];
    }
  }

  //  by D. Lemire
  template <class T> static void inverseDelta(T *data, const size_t size) {
    if (size == 0)
      return;
    size_t i = 1;
    for (; i < size - 1; i += 2) {
      data[i] += data[i - 1];
      data[i + 1] += data[i];
    }
    for (; i != size; ++i) {
      data[i] += data[i - 1];
    }
  }

  //  Original designed by Vasily Volkov, improved by D. Lemire
  template <class T> static void fastinverseDelta(T *data, const size_t size) {
    if (size == 0)
      return;
    const size_t UnrollQty = 4;
    const size_t sz0 =
        (size / UnrollQty) * UnrollQty; // equal to 0, if size < UnrollQty
    size_t i = 1;
    if (sz0 >= UnrollQty) {
      T a = data[0];
      for (; i < sz0 - UnrollQty; i += UnrollQty) {
        a = data[i] += a;
        a = data[i + 1] += a;
        a = data[i + 2] += a;
        a = data[i + 3] += a;
      }
    }
    for (; i != size; ++i) {
      data[i] += data[i - 1];
    }
  }

  template <class T>
  static void fastinverseDelta2(T *pData, const size_t TotalQty) {
    if (TotalQty < 5) {
      inverseDelta(pData, TotalQty); // no SIMD
      return;
    }
    const size_t Qty4 = TotalQty / 4;

    __m128i runningCount = _mm_setzero_si128();
    __m128i *pCurr = reinterpret_cast<__m128i *>(pData);
    const __m128i *pEnd = pCurr + Qty4;
    while (pCurr < pEnd) {
      __m128i a0 = _mm_load_si128(pCurr);
      __m128i a1 = _mm_add_epi32(_mm_slli_si128(a0, 8), a0);
      __m128i a2 = _mm_add_epi32(_mm_slli_si128(a1, 4), a1);
      a0 = _mm_add_epi32(a2, runningCount);
      runningCount = _mm_shuffle_epi32(a0, 0xFF);
      _mm_store_si128(pCurr++, a0);
    }

    for (size_t i = Qty4 * 4; i < TotalQty; ++i) {
      pData[i] += pData[i - 1];
    }
  }

  // a convenience function
  template <class container>
  static void process(std::vector<algostats> &myalgos,
                      const std::vector<container> &datas,
                      processparameters &pp, const std::string prefix = "") {
    // pp.needtodelta = false;
    enum { verbose = false };
    if (datas.empty() || myalgos.empty())
      return;
    if (pp.needtodelta) {
      if (verbose)
        std::cout << "# delta coding requested... checking whether we have "
                     "sorted arrays...";
      for (auto x : datas) {
        if (x.size() > 0)
          for (size_t k = 1; k < x.size(); ++k) {
            if (x[k] < x[k - 1]) {
              std::cerr << "Delta coding requested, but data is not sorted!"
                        << std::endl;
              std::cerr << "Aborting!" << std::endl;
              return;
            }
          }
      }
      if (verbose)
        std::cout << " arrays are indeed sorted. Good." << std::endl;
    } else {
      if (verbose)
        std::cout
            << "# compressing the arrays themselves, no delta coding applied."
            << std::endl;
      // we check whether it could have been applied...
      bool sorted = true;
#if !defined(__clang__) && !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC diagnostic ignored                                                 \
    "-Wunsafe-loop-optimizations" // otherwise I get bogus warning
#endif
      for (auto &x : datas)
        if (sorted && (x.size() >= 1))
          for (size_t k = 1; k < x.size(); ++k) {
            if (x[k] < x[k - 1]) {
              sorted = false;
              break;
            }
          }
#if !defined(__clang__) && !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC diagnostic pop
#endif
      if (sorted) {
        std::cout << "#\n#\n# you are providing sorted arrays, but you are not "
                     "requesting delta coding. Are you sure?\n#\n#\n"
                  << std::endl;
      } else {
        if (verbose)
          std::cout << "# I verified that the arrays are not sorted so simple "
                       "delta coding is unapplicable."
                    << std::endl;
      }
    }
    if (pp.displayhistogram) {
      BitWidthHistoGram hist;
      for (auto i = datas.begin(); i != datas.end(); ++i)
        hist.eatIntegers(*i);
      hist.display("#");
    }
    if (pp.fulldisplay)
      std::cout << "#";
    if (pp.fulldisplay && pp.computeentropy)
      std::cout << " entropy  databits(entropy) ";

    if (pp.fulldisplay) {
      for (auto i = myalgos.begin(); i != myalgos.end(); ++i) {
        std::cout << (*i).name() << "\t";
      }
      std::cout << std::endl;
    }
    if (pp.fulldisplay)
      std::cout
          << "# for each scheme we give compression speed (million int./s)"
             " decompression speed and bits per integer"
          << std::endl;
    EntropyRecorder er;
    if (pp.computeentropy) {
      for (size_t k = 0; k < datas.size(); ++k)
        if (!datas[k].empty())
          er.eat(&datas[k][0], datas[k].size());
      if (pp.fulldisplay)
        std::cout << "# generated " << er.totallength << " integers"
                  << std::endl;
    }
    if (pp.fulldisplay)
      std::cout << prefix << "\t";
    if (pp.computeentropy && pp.fulldisplay)
      std::cout << std::setprecision(4) << er.computeShannon() << "\t";
    if (pp.computeentropy && pp.fulldisplay)
      std::cout << std::setprecision(4) << er.computeDataBits() << "\t";
    WallClockTimer z;
    size_t totallength = 0;
    size_t maxlength = 0;
    std::vector<container> outs(datas.size());
    for (size_t k = 0; k < datas.size(); ++k) {
      auto &data = datas[k];
      outs[k].resize(4 * data.size() + 2048 + 64);
      totallength += data.size();
      if (maxlength < data.size())
        maxlength = data.size();
    }
    std::vector<size_t> nvalues(datas.size());
    container recovereds(maxlength + 2048 + 64);
    container backupdata;
    backupdata.reserve(maxlength + 2048 + 64);
    for (auto i = myalgos.begin(); i != myalgos.end(); ++i) {
      IntegerCODEC &c = *(i->algo);
      const bool SIMDDeltas = i->SIMDDeltas;
      size_t nvalue;
      size_t totalcompressed = 0;
      double timemsdecomp = 0;
      double timemscomp = 0;
      double timemsdelta = 0;
      double timemsinversedelta = 0;
      for (size_t k = 0; k < datas.size(); ++k) {
        if (datas[k].empty())
          continue;
        uint32_t *outp = &outs[k][0];
        nvalue = outs[k].size();
        assert(!needPaddingTo128Bits(outp));
        const size_t orignvalue = nvalue;
        {
          nvalue = orignvalue;
          backupdata.assign(datas[k].begin(),
                            datas[k].end()); // making a copy to be safe

          if (pp.needtodelta) {
            z.reset();
            if (SIMDDeltas) {
              deltaSIMD(&backupdata[0], backupdata.size());
            } else {
              fastDelta(&backupdata[0], backupdata.size());
            }
            timemsdelta += static_cast<double>(z.split());
          }

          z.reset();
          c.encodeArray(backupdata.data(), backupdata.size(), outp, nvalue);
          nvalues[k] = nvalue;
          timemscomp += static_cast<double>(z.split());
        }

        totalcompressed += nvalue;
      }
      for (size_t k = 0; k < datas.size(); ++k) {
        const uint32_t *outp = outs[k].data();
        nvalue = nvalues[k];
        size_t recoveredsize = datas[k].size();
        assert(recoveredsize > 0);
        uint32_t *recov = recovereds.data();
        assert(!needPaddingTo128Bits(recov));

        z.reset();
        c.decodeArray(outp, nvalue, recov, recoveredsize);
        timemsdecomp += static_cast<double>(z.split());

        if (pp.needtodelta) {
          z.reset();
          if (SIMDDeltas) {
            inverseDeltaSIMD(recov, recoveredsize);
          } else {
            fastinverseDelta2(recov, recoveredsize);
            // fastinverseDelta(recov, recoveredsize);
            // inverseDelta(recov, recoveredsize);
          }
          timemsinversedelta += static_cast<double>(z.split());
        }

        if (recoveredsize != datas[k].size()) {
          std::cerr << " expected size of " << datas[k].size() << " got "
                    << recoveredsize << std::endl;
          throw std::logic_error("arrays don't have same size: bug.");
        }
        // if (!equal(datas[k].begin(), datas[k].end(), recov)) {
        //    throw std::logic_error("we have a bug");
        //}
        for (size_t i = 0; i < datas[k].size(); i++) {
          if (datas[k][i] != recov[i]) {
            std::cout << "difference at index " << i << ":" << std::endl;
            std::cout << "  expected: " << datas[k][i] << std::endl;
            std::cout << "    actual: " << recov[i] << std::endl;
            throw std::logic_error("we have a bug");
          }
        }
      }
      if (!pp.separatetimes) {
        timemscomp += timemsdelta;
        timemsdecomp += timemsinversedelta;
      }
      if (pp.cumulative) {
        i->comptime += timemscomp;
        i->decomptime += timemsdecomp;
        if (pp.separatetimes) {
          i->deltatime += timemsdelta;
          i->inversedeltatime += timemsinversedelta;
        }
        i->output += totalcompressed;
        i->input += totallength;
      } else {
        i->compspeed.push_back(static_cast<double>(totallength) /
                               static_cast<double>(timemscomp));
        i->decompspeed.push_back(static_cast<double>(totallength) /
                                 static_cast<double>(timemsdecomp));
        if (pp.separatetimes) {
          i->deltaspeed.push_back(static_cast<double>(totallength) /
                                  static_cast<double>(timemsdelta));
          i->inversedeltaspeed.push_back(
              static_cast<double>(totallength) /
              static_cast<double>(timemsinversedelta));
        }
        i->bitsperint.push_back(static_cast<double>(totalcompressed) * 32.0 /
                                static_cast<double>(totallength));
      }
      if (pp.fulldisplay) {
        if (pp.separatetimes) {
          std::cout << std::setprecision(4)
                    << static_cast<double>(totallength) /
                           static_cast<double>(timemsdelta)
                    << "\t";
        }
        std::cout << std::setprecision(4)
                  << static_cast<double>(totallength) /
                         static_cast<double>(timemscomp)
                  << "\t";
        std::cout << std::setprecision(4)
                  << static_cast<double>(totallength) /
                         static_cast<double>(timemsdecomp)
                  << "\t";
        if (pp.separatetimes) {
          std::cout << std::setprecision(4)
                    << static_cast<double>(totallength) /
                           static_cast<double>(timemsinversedelta)
                    << "\t";
        }
        std::cout << std::setprecision(4)
                  << static_cast<double>(totalcompressed) * 32.0 /
                         static_cast<double>(totallength)
                  << "\t";
        std::cout << "\t";
      }
    }
    if (pp.fulldisplay)
      std::cout << std::endl;
  }
};

} // namespace FastPFor

#endif /* DELTAUTIL_H_ */
