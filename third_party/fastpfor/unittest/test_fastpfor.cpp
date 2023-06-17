#include <vector>
#include <memory>
#include <limits>
#include <random>
#include <cmath>
#include <string>

#include "gtest/gtest.h"

#include "codecs.h"
#include "fastpfor.h"

namespace FastPForLib {

  using ::testing::TestWithParam;
  using ::testing::Values;

  class FastPForTest : public ::testing::TestWithParam<std::string> {
    public:
      virtual void SetUp() {
        std::string name = GetParam();
        if (name == "FastPFor128") {
          codec.reset(new FastPFor<4>());
        } else if (name == "FastPFor256") {
          codec.reset(new FastPFor<8>());
        } else {
          throw new std::logic_error("Unknown codec " + name);
        }
      }

      protected:
        std::unique_ptr<IntegerCODEC> codec;
        std::vector<int32_t> in32;
        std::vector<int64_t> in64;
        std::vector<int32_t> out32;
        std::vector<int64_t> out64;

        void _verify() {
          size_t inSize = in32.size();
          std::vector<uint32_t> encoded(in32.size() * 2, ~0);
          size_t encodeSize = encoded.size();

          codec->encodeArray(
                             reinterpret_cast<uint32_t *>(in32.data()),
                             inSize,
                             encoded.data(),
                             encodeSize);

          out32.resize(inSize);
          codec->decodeArray(
                             encoded.data(),
                             encodeSize,
                             reinterpret_cast<uint32_t *>(out32.data()),
                             inSize);

          bool passed = true;
          for (size_t i = 0; i < inSize; ++i) {
            if (in32[i] != out32[i]) {
              passed = false;
            }
            EXPECT_EQ(in32[i], out32[i]);
          }
          if (!passed) {
            std::cout << "Test failed with int32 input: ";
            for (size_t i = 0; i < inSize; ++i) {
              std::cout << in32[i] << " ";
            }
            std::cout << std::endl;
          }
        }

        void _verify64() {
          size_t inSize = in64.size();
          std::vector<uint32_t> encoded(in64.size() * 4, ~0);
          size_t encodeSize = encoded.size();

          codec->encodeArray(
                             reinterpret_cast<uint64_t *>(in64.data()),
                             inSize,
                             encoded.data(),
                             encodeSize);

          out64.resize(inSize);
          codec->decodeArray(
                             encoded.data(),
                             encodeSize,
                             reinterpret_cast<uint64_t *>(out64.data()),
                             inSize);

          bool passed = true;
          for (size_t i = 0; i < inSize; ++i) {
            if (in64[i] != out64[i]) {
              passed = false;
            }
            EXPECT_EQ(in64[i], out64[i]);
          }
          if (!passed) {
            std::cout << "Test failed with int64 input: ";
            for (size_t i = 0; i < inSize; ++i) {
              std::cout << in64[i] << " ";
            }
            std::cout << std::endl;
          }
        }

        void _copy64() {
          in64.clear();
          for (size_t i = 0; i < in32.size(); ++i) {
            in64.push_back(in32[i]);
          }
        }

        void _genDataRandom(std::vector<int32_t>& v, uint32_t values) {
          v.clear();
          std::mt19937_64 e2(123456);
          std::uniform_int_distribution<int32_t> dist(
                                  std::numeric_limits<int32_t>::min(),
                                  std::numeric_limits<int32_t>::max());
          for (int i = 0; i < values; ++i) {
            v.push_back(dist(e2));
          }
        }

        void _genDataRandom64(std::vector<int64_t>& v, uint32_t values) {
          v.clear();
          std::mt19937_64 e2(123456);
          std::uniform_int_distribution<int64_t> dist(
                                  std::numeric_limits<int64_t>::min(),
                                  std::numeric_limits<int64_t>::max());
          for (int i = 0; i < values; ++i) {
            v.push_back(dist(e2));
          }
        }

        void _genDataWithFixBits(
                                 std::vector<int32_t>& v,
                                 uint32_t bits,
                                 uint32_t values) {
          v.clear();
          std::mt19937_64 e2(123456);
          std::uniform_int_distribution<uint32_t> dist(
                                  0,
                                  bits == 32 ? ~0U : (1U << bits) - 1);
          for (size_t i = 0; i < values; ++i) {
            v.push_back(static_cast<int32_t>(dist(e2) | 1U << (bits - 1)));
          }
        }

        void _genDataWithFixBits64(
                                   std::vector<int64_t>& v,
                                   uint32_t bits,
                                   uint32_t values) {
          v.clear();
          std::mt19937_64 e2(123456);
          std::uniform_int_distribution<uint64_t> dist(
                                  0,
                                  bits == 64 ? ~0UL : (1ULL << bits) - 1);
          for (size_t i = 0; i < values; ++i) {
            v.push_back(static_cast<int32_t>(dist(e2) | 1ULL << (bits - 1)));
          }
        }
    };

  TEST_P(FastPForTest, increasingSequence) {
    in32.resize(0);
    for (int i = -100; i < 156; ++i) {
      in32.push_back(i);
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, randomNumbers) {
    _genDataRandom(in32, 65536);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, randomNumbers64) {
    _genDataRandom64(in64, 65536);
    _verify64();
  }

  TEST_P(FastPForTest, adHocNumbers64) {
    int64_t data[] = {
                -3673975021604308289,
                277811506958363848,
                -7625128575524920515,
                -3321922176697690625,
                -8484521102416600502,
                4879706116117661039,
                3108316356327171753,
                -5023690236249800232};

    in64.clear();
    for (int i = 0; i < 64; ++i) {
      for (int j = 0; j < 4; ++j) {
        in64.push_back(data[j]);
      }
    }
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_zeors) {
    in32.clear();
    for (int i = 0; i < 256; ++i) {
      in32.push_back(0);
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_zeros_with_exceptions) {
    in32.clear();
    in32.push_back(1024);
    for (int i = 0; i < 254; ++i) {
      in32.push_back(0);
    }
    in32.push_back(1033);

    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_min_max) {
    _genDataRandom(in32, 256);
    in32[0] = std::numeric_limits<int32_t>::min();
    in32[127] = std::numeric_limits<int32_t>::max();
    in32[128] = std::numeric_limits<int32_t>::min();
    in32[255] = std::numeric_limits<int32_t>::max();

    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_min_max64) {
    _genDataRandom64(in64, 256);
    in64[0] = std::numeric_limits<int64_t>::min();
    in64[127] = std::numeric_limits<int64_t>::max();
    in64[128] = std::numeric_limits<int64_t>::min();
    in64[255] = std::numeric_limits<int64_t>::max();

    _verify64();
  }

  TEST_P(FastPForTest, fastpack_1_noexcept) {
    _genDataWithFixBits(in32, 1, 1024);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_2_noexcept) {
    _genDataWithFixBits(in32, 2, 1024);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_4_noexcept) {
    _genDataWithFixBits(in32, 4, 1024);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_5_except) {
    _genDataWithFixBits(in32, 5, 256);
    in32[10] = 10002124;
    in32[77] = 20002124;
    in32[177] = 50002124;
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_8_noexcept) {
    _genDataWithFixBits(in32, 8, 512);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_16_noexcept) {
    _genDataWithFixBits(in32, 16, 256);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_22_noexcept) {
    _genDataWithFixBits(in32, 22, 256);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_32_noexcept) {
    _genDataWithFixBits(in32, 32, 768);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_40_noexcept) {
    _genDataWithFixBits64(in64, 40, 512);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_41_noexcept) {
    _genDataWithFixBits64(in64, 41, 512);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_51_noexcept) {
    _genDataWithFixBits64(in64, 51, 512);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_56_noexcept) {
    _genDataWithFixBits64(in64, 56, 512);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_63_noexcept) {
    _genDataWithFixBits64(in64, 63, 512);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_64_noexcept) {
    _genDataWithFixBits64(in64, 64, 768);
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_7_except_63) {
    _genDataWithFixBits64(in64, 7, 256);
    std::vector<int64_t> excepts;
    _genDataWithFixBits64(excepts, 63, 6);
    in64[0] = excepts[0];
    in64[10] = excepts[1];
    in64[100] = excepts[2];
    in64[133] = excepts[3];
    in64[177] = excepts[4];
    in64[213] = excepts[5];
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_0_except_32) {
    _genDataWithFixBits64(in64, 32, 256);
    for (int i = 20; i < 40; ++i) {
      in64[i] = 0;
    }
    for (int i = 155; i < 195; i += 2) {
      in64[i] = 0;
    }
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_3_except_64) {
    _genDataWithFixBits64(in64, 3, 256);
    for (int i = 20; i < 40; ++i) {
      in64[i] = ~0UL - i;
    }
    for (int i = 155; i < 195; i += 2) {
      in64[i] = ~0UL - i ;
    }
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_13_with_small_numbers) {
    _genDataWithFixBits(in32, 13, 256);
    in32[20] = 3U << 5;
    in32[60] = 2U << 4;
    in32[150] = 7U << 3;
    _verify();
    _copy64();
    _verify64();
  }

  TEST_P(FastPForTest, fastpack_35_with_excepts_and_small_numbers) {
    _genDataWithFixBits64(in64, 35, 256);
    std::vector<int64_t> excepts;
    _genDataWithFixBits64(excepts, 48, 6);
    in64[10] = 5U << 5;
    in64[133] = 7U << 4;
    in64[115] = 6U << 3;

    in64[5] = excepts[0];
    in64[21] = excepts[1];
    in64[22] = excepts[2];
    in64[137] = excepts[3];
    in64[155] = excepts[4];
    in64[221] = excepts[5];
    _verify64();
  }

  INSTANTIATE_TEST_CASE_P(
      FastPForLib,
      FastPForTest,
      Values("FastPFor128",
             "FastPFor256"));
}
