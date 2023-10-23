//
// Created by jinhai on 23-10-22.
//

module;

#include <cstdint>
#include "faiss/utils/ordered_key_value.h"

export module faiss;

namespace infinity {

export template<typename T1, typename T2>
using FaissCMin = faiss::CMin<T1, T2>;

export template<typename T1, typename T2>
using FaissCMax = faiss::CMax<T1, T2>;

}
