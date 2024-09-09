// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"
import base_test;
import croaring;

class CRoaringTest : public infinity::BaseTest {};

TEST_F(CRoaringTest, test_size) {
    {
        std::cout << "=== add range" << std::endl;
        roaring::Roaring r;
        r.addRange(0, 8192);
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
    }
    {
        std::cout << "\n=== add one-by-one" << std::endl;
        roaring::Roaring r;
        for (uint32_t i = 0; i < 8192; ++i) {
            r.add(i);
        }
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
        std::cout << "call runOptimize" << std::endl;
        r.runOptimize();
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
    }
    {
        std::cout << "\n=== add one-by-one" << std::endl;
        roaring::Roaring r;
        for (uint32_t i = 0; i < 8192; ++i) {
            r.add(i);
        }
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
        std::cout << "add range" << std::endl;
        r.addRange(0, 8192);
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
        std::cout << "call runOptimize" << std::endl;
        r.runOptimize();
        std::cout << "cardinality = " << r.cardinality() << std::endl;
        std::cout << "size = " << r.getSizeInBytes() << std::endl;
    }
}

TEST_F(CRoaringTest, test1) {
    roaring::Roaring r1;
    for (uint32_t i = 100; i < 1000; i++) {
        r1.add(i);
    }
    std::cout << "cardinality = " << r1.cardinality() << std::endl;

    roaring::Roaring64Map r2;
    for (uint64_t i = 18000000000000000100ull; i < 18000000000000001000ull; i++) {
        r2.add(i);
    }
    std::cout << "cardinality = " << r2.cardinality() << std::endl;
}

using namespace roaring;

TEST_F(CRoaringTest, test2) {
    Roaring r1;
    for (uint32_t i = 100; i < 1000; i++) {
        r1.add(i);
    }

    // check whether a value is contained
    assert(r1.contains(500));

    // compute how many bits there are:
    uint32_t cardinality = r1.cardinality();
    std::cout << "Cardinality = " << cardinality << std::endl;

    // if your bitmaps have long runs, you can compress them by calling
    // run_optimize
    uint32_t size = r1.getSizeInBytes();
    r1.runOptimize();

    // you can enable "copy-on-write" for fast and shallow copies
    r1.setCopyOnWrite(true);

    uint32_t compact_size = r1.getSizeInBytes();
    std::cout << "size before run optimize " << size << " bytes, and after " << compact_size << " bytes." << std::endl;

    // create a new bitmap with varargs
    Roaring r2 = Roaring::bitmapOf(5, 1, 2, 3, 5, 6);

    r2.printf();
    printf("\n");

    // create a new bitmap with initializer list
    Roaring r2i = Roaring::bitmapOfList({1, 2, 3, 5, 6});

    assert(r2i == r2);

    // we can also create a bitmap from a pointer to 32-bit integers
    const uint32_t values[] = {2, 3, 4};
    Roaring r3(3, values);

    // we can also go in reverse and go from arrays to bitmaps
    uint64_t card1 = r1.cardinality();
    uint32_t *arr1 = new uint32_t[card1];
    r1.toUint32Array(arr1);
    Roaring r1f(card1, arr1);
    delete[] arr1;

    // bitmaps shall be equal
    assert(r1 == r1f);

    // we can copy and compare bitmaps
    Roaring z(r3);
    assert(r3 == z);

    // we can compute union two-by-two
    Roaring r1_2_3 = r1 | r2;
    r1_2_3 |= r3;

    // we can compute a big union
    const Roaring *allmybitmaps[] = {&r1, &r2, &r3};
    Roaring bigunion = Roaring::fastunion(3, allmybitmaps);
    assert(r1_2_3 == bigunion);

    // we can compute intersection two-by-two
    Roaring i1_2 = r1 & r2;

    // we can write a bitmap to a pointer and recover it later
    uint32_t expectedsize = r1.getSizeInBytes();
    char *serializedbytes = new char[expectedsize];
    r1.write(serializedbytes);
    // readSafe will not overflow, but the resulting bitmap
    // is only valid and usable if the input follows the
    // Roaring specification: https://github.com/RoaringBitmap/RoaringFormatSpec/
    Roaring t = Roaring::readSafe(serializedbytes, expectedsize);
    assert(r1 == t);
    delete[] serializedbytes;

    // we can iterate over all values using custom functions
    uint32_t counter = 0;
    r1.iterate(
        [](uint32_t value, void *param) {
            *(uint32_t *)param += value;
            return true;
        },
        &counter);

    // we can also iterate the C++ way
    counter = 0;
    for (Roaring::const_iterator i = t.begin(); i != t.end(); i++) {
        ++counter;
    }
    // counter == t.cardinality()

    // we can move iterators to skip values
    const uint32_t manyvalues[] = {2, 3, 4, 7, 8};
    Roaring rogue(5, manyvalues);
    Roaring::const_iterator j = rogue.begin();
    j.equalorlarger(4); // *j == 4
}
