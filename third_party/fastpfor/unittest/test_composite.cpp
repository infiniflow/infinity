#include <vector>
#include <memory>
#include <limits>
#include <random>
#include <cmath>

#include "codecs.h"
#include "compositecodec.h"
#include "variablebyte.h"
#include "fastpfor.h"

#include "gtest/gtest.h"

namespace FastPForLib {

  class CompositeCodecTest : public ::testing::Test {
    public:
      virtual void SetUp();

      protected:
        std::unique_ptr<IntegerCODEC> codec;
        std::vector<int32_t> in32, out32;
        std::vector<uint32_t> encoded;
        std::vector<int64_t> in64, out64;

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
          std::vector<uint32_t> encoded(in64.size() * 4);
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
    };

  void CompositeCodecTest::SetUp() {
    codec.reset(new CompositeCodec<FastPFor<8>, VariableByte>());
  }

  TEST_F(CompositeCodecTest, emptyArray) {
    in32.resize(0);
    out32.resize(0);
    _verify();
    _copy64();
    _verify64();
  }

  TEST_F(CompositeCodecTest, lessThanOneBlock) {
    in32.resize(0);
    for (int32_t i = 0; i < 255; i += 2) {
      in32.push_back(i);
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_F(CompositeCodecTest, exactOneBlock) {
    in32.resize(0);
    for (int32_t i = 0; i < 256; i += 2) {
      in32.push_back(i);
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_F(CompositeCodecTest, moreThanThreeBlock) {
    in32.resize(0);
    for (int i = 0; i < 1000; i = i + 3) {
      in32.push_back(i);
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_F(CompositeCodecTest, randomeNumberMoreThanOnePage) {
    in32.resize(0);
    std::mt19937_64 e2(123456);
    std::uniform_int_distribution<int32_t> dist(
                            std::numeric_limits<int32_t>::min(),
                            std::numeric_limits<int32_t>::max());
    for (int i = 0; i < 70000; ++i) {
      in32.push_back(dist(e2));
    }
    _verify();
    _copy64();
    _verify64();
  }

  TEST_F(CompositeCodecTest, randomeNumberMoreThanOnePage64) {
    in64.resize(0);
    std::mt19937_64 e2(123456);
    std::uniform_int_distribution<int64_t> dist(
                            std::numeric_limits<int64_t>::min(),
                            std::numeric_limits<int64_t>::max());
    for (int i = 0; i < 70000; ++i) {
      in64.push_back(dist(e2));
    }
    _verify64();
  }
}
