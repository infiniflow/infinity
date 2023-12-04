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

module;

#include <limits>

export module faiss;

namespace infinity {


export constexpr int faiss_distance_compute_blas_threshold = 20;
export constexpr int faiss_distance_compute_blas_query_bs = 4096;
export constexpr int faiss_distance_compute_blas_database_bs = 1024;
export constexpr int faiss_distance_compute_min_k_reservoir = 100;


export template <typename T_, typename TI_>
struct CompareMax;


// traits of minheaps = heaps where the minimum value is stored on top
// useful to find the *max* values of an array
export template <typename T_, typename TI_>
struct CompareMin {
    typedef T_ T;
    typedef TI_ TI;
    typedef CompareMax<T_, TI_> Crev; // reference to reverse comparison
    static bool cmp(T a, T b) {
        return a < b;
    }

    // Similar to cmp(), but also breaks ties
    // by comparing the second pair of arguments.
    static bool cmp2(T a1, T b1, TI a2, TI b2) {
        return (a1 < b1) || ((a1 == b1) && (a2 < b2));
    }

    static T neutral() {
        return std::numeric_limits<T>::lowest();
    }

    static const bool is_max = false;

    // static T nextafter(T x) {
    //     return std::nextafterf(x, -HUGE_VALF);
    // }
};

template <typename T_, typename TI_>
struct CompareMax {
    typedef T_ T;
    typedef TI_ TI;
    typedef CompareMin<T_, TI_> Crev;

    static bool cmp(T a, T b) {
        return a > b;
    }

    // Similar to cmp(), but also breaks ties
    // by comparing the second pair of arguments.
    static bool cmp2(T a1, T b1, TI a2, TI b2) {
        return (a1 > b1) || ((a1 == b1) && (a2 > b2));
    }

    static T neutral() {
        return std::numeric_limits<T>::max();
    }

    static const bool is_max = true;

    // static T nextafter(T x) {
    //     return std::nextafterf(x, HUGE_VALF);
    // }
};


}
