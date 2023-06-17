#include <vector>
#include <memory>
#include <limits>
#include <random>
#include <cmath>

#include "gtest/gtest.h"

#include "codecs.h"
#include "variablebyte.h"

namespace FastPForLib {

  class VariableByteTest : public ::testing::Test {
    public:
      virtual void SetUp();

      protected:
        std::unique_ptr<VariableByte> codec;
        std::vector<int32_t> in32;
        std::vector<int64_t> in64;
        std::vector<uint32_t> encoded;
        std::vector<int32_t> out32;
        std::vector<int64_t> out64;

        void _verify() {
          size_t inSize = in32.size();
          encoded.resize(in32.size() * 2);
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
          encoded.resize(in64.size() * 4);
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
    };

  void VariableByteTest::SetUp() {
    codec.reset(new VariableByte());
  }

  TEST_F(VariableByteTest, emptyArray) {
    in32.resize(0);
    out32.resize(0);
    _verify();
  }

  TEST_F(VariableByteTest, emptyArray64) {
    in64.resize(0);
    out64.resize(0);
    _verify64();
  }

  TEST_F(VariableByteTest, increasingSequence) {
    in32.resize(0);
    for (int i = -20; i < 1000; ++i) {
      in32.push_back(i);
    }
    _verify();
  }

  TEST_F(VariableByteTest, maxAndMin) {
    in32.resize(0);
    in32.push_back(std::numeric_limits<int32_t>::min());
    in32.push_back(0);
    in32.push_back(std::numeric_limits<int32_t>::max());
    _verify();
  }

  TEST_F(VariableByteTest, increasingSequence64) {
    in64.resize(0);
    for (int i = -25; i < 1111; ++i) {
      in64.push_back(i);
    }
    _verify();
  }

  TEST_F(VariableByteTest, negativeIncreasingSequence64) {
    in64.resize(0);
    int64_t start = -9223372036854775800;
    for (int64_t i = 0; i < 3000; ++i) {
      in64.push_back(start + i);
    }
    _verify64();
  }

  TEST_F(VariableByteTest, positiveDecreasingSequence64) {
    in64.resize(0);
    int64_t start = 9223372036854775800;
    for (int64_t i = 0; i < 1555; ++i) {
      in64.push_back(start - i);
    }
    _verify64();
  }

  TEST_F(VariableByteTest, maxAndMin64) {
    in64.resize(0);
    in64.push_back(std::numeric_limits<int64_t>::min());
    in64.push_back(0);
    in64.push_back(std::numeric_limits<int64_t>::max());
    _verify64();
  }

  TEST_F(VariableByteTest, sequenceHasBoth32And64) {
    in64.resize(0);
    in64.push_back(-9223372036854775000);
    in64.push_back(-32767);
    in64.push_back(-300);
    in64.push_back(0);
    in64.push_back(500);
    in64.push_back(32767);
    in64.push_back(65536);
    in64.push_back(2147483647);
    in64.push_back(4294967295);
    in64.push_back(8223372036854775800);
    _verify64();
  }

  TEST_F(VariableByteTest, randomNumbers5000) {
    in64.resize(0);
    std::mt19937_64 e2(123456);
    std::uniform_int_distribution<int64_t> dist(
                            std::numeric_limits<int64_t>::min(),
                            std::numeric_limits<int64_t>::max());
    for (int i = 0; i < 5000; ++i) {
      in64.push_back(dist(e2));
    }
    _verify64();
  }
}
