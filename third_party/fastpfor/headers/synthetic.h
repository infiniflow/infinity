/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

#ifndef SYNTHETICDATA_H_
#define SYNTHETICDATA_H_

#include <vector>
#include <set>
#include "common.h"
#include "util.h"
#include "mersenne.h"
#include "memutil.h"

namespace FastPForLib {

std::vector<uint32_t, cacheallocator>
generateArray(uint32_t N, const uint32_t mask = 0xFFFFFFFFU) {
  std::vector<uint32_t, cacheallocator> ans(N);
  for (size_t k = 0; k < N; ++k)
    ans[k] = rand() & mask;
  return ans;
}

std::vector<uint32_t, cacheallocator>
generateArray32(uint32_t N, const uint32_t mask = 0xFFFFFFFFU) {
  std::vector<uint32_t, cacheallocator> ans(N);
  for (size_t k = 0; k < N; ++k)
    ans[k] = rand() & mask;
  return ans;
}

class UniformDataGenerator {
public:
  UniformDataGenerator(uint32_t seed = static_cast<uint32_t>(time(NULL)))
      : rand(seed) {}
  /**
   * fill the vector with N numbers uniformly picked from  from 0 to Max, not
   * including Max
   * if it is not possible, an exception is thrown
   */
  std::vector<uint32_t, cacheallocator> generateUniform(uint32_t N,
                                                        uint32_t Max) {
    if (Max < N)
      throw std::runtime_error(
          "can't generate enough distinct elements in small interval");
    std::vector<uint32_t, cacheallocator> ans;
    if (N == 0)
      return ans; // nothing to do
    ans.reserve(N);
    assert(Max >= 1);

    if (2 * N > Max) {
      std::set<uint32_t> s;
      while (s.size() < Max - N)
        s.insert(rand.getValue(Max - 1));
      s.insert(Max);
      ans.resize(N);
      uint32_t i = 0;
      size_t c = 0;
      for (uint32_t v : s) {
        for (; i < v; ++i)
          ans[c++] = i;
        ++i;
      }
      assert(c == ans.size());
    } else {
      std::set<uint32_t> s;
      while (s.size() < N)
        s.insert(rand.getValue(Max - 1));
      ans.assign(s.begin(), s.end());
      assert(N == ans.size());
    }
    return ans;
  }
  ZRandom rand;
};

/*
* Reference: Vo Ngoc Anh and Alistair Moffat. 2010. Index compression using
* 64-bit words. Softw. Pract. Exper.40, 2 (February 2010), 131-147.
*/
class ClusteredDataGenerator {
public:
  UniformDataGenerator unidg;
  ClusteredDataGenerator(uint32_t seed = static_cast<uint32_t>(time(NULL)))
      : unidg(seed) {}

  // Max value is excluded from range
  template <class iterator>
  void fillUniform(iterator begin, iterator end, uint32_t Min, uint32_t Max) {
    std::vector<uint32_t, cacheallocator> v =
        unidg.generateUniform(static_cast<uint32_t>(end - begin), Max - Min);
    for (size_t k = 0; k < v.size(); ++k)
      *(begin + k) = Min + v[k];
  }

  // Max value is excluded from range
  // throws exception if impossible
  template <class iterator>
  void fillClustered(iterator begin, iterator end, uint32_t Min, uint32_t Max) {
    const uint32_t N = static_cast<uint32_t>(end - begin);
    const uint32_t range = Max - Min;
    if (range < N)
      throw std::runtime_error("can't generate that many in small interval.");
    assert(range >= N);
    if ((range == N) || (N < 10)) {
      fillUniform(begin, end, Min, Max);
      return;
    }
    const uint32_t cut = N / 2 + unidg.rand.getValue(range - N);
    assert(cut >= N / 2);
    assert(Max - Min - cut >= N - N / 2);
    const double p = unidg.rand.getDouble();
    assert(p <= 1);
    assert(p >= 0);
    if (p <= 0.25) {
      fillUniform(begin, begin + N / 2, Min, Min + cut);
      fillClustered(begin + N / 2, end, Min + cut, Max);
    } else if (p <= 0.5) {
      fillClustered(begin, begin + N / 2, Min, Min + cut);
      fillUniform(begin + N / 2, end, Min + cut, Max);
    } else {
      fillClustered(begin, begin + N / 2, Min, Min + cut);
      fillClustered(begin + N / 2, end, Min + cut, Max);
    }
  }

  // Max value is excluded from range
  std::vector<uint32_t, cacheallocator> generateClustered(uint32_t N,
                                                          uint32_t Max) {
    std::vector<uint32_t, cacheallocator> ans(N);
    fillClustered(ans.begin(), ans.end(), 0, Max);
    return ans;
  }
};

class ZipfianGenerator {
public:
  uint32_t n;
  double zetan, theta;
  std::vector<double> proba;

  ZRandom rand;
  ZipfianGenerator(uint32_t seed = static_cast<uint32_t>(time(NULL)))
      : n(0), zetan(0), theta(0), proba(n), rand(seed) {}

  void init(int _items, double _zipfianconstant = 1.0) {
    n = _items;
    if (_items == 0)
      throw std::runtime_error("no items?");
    theta = _zipfianconstant;
    if (theta > 0) {
      zetan = 1 / zeta(n, theta);
      proba.clear();
      proba.resize(n, 0);
      proba[0] = zetan;
      for (uint32_t i = 1; i < n; ++i)
        proba[i] = proba[i - 1] + zetan / pow(i + 1, theta);
    } else {
      proba.resize(n, 1.0 / n);
    }
  }

  void seed(uint32_t s) { rand.seed(s); }

  ZipfianGenerator(int _items, double _zipfianconstant,
                   uint32_t seed = static_cast<uint32_t>(time(NULL)))
      : n(_items), zetan(0), theta(_zipfianconstant), proba(n), rand(seed) {
    init(_items, _zipfianconstant);
  }

  double zeta(int n, double theta) {
    double sum = 0;
    for (long i = 0; i < n; i++) {
      sum += 1.0 / (pow(static_cast<double>(i + 1), theta));
    }
    return sum;
  }
  int nextInt() {
    // Map z to the value
    const double u = rand.getDouble();
    return static_cast<int>(lower_bound(proba.begin(), proba.end(), u) -
                            proba.begin());
  }
};

std::vector<uint32_t, cacheallocator>
generateZipfianArray32(uint32_t N, double power,
                       const uint32_t mask = 0xFFFFFFFFU) {
  std::vector<uint32_t, cacheallocator> ans(N);
  ZipfianGenerator zipf;
  const uint32_t MAXVALUE = 1U << 22;
  zipf.init(mask > MAXVALUE - 1 ? MAXVALUE : mask + 1, power);
  for (size_t k = 0; k < N; ++k)
    ans[k] = zipf.nextInt();
  return ans;
}

} // namespace FastPFor

#endif
