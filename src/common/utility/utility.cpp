//
// Created by jinhai on 23-2-18.
//

module;

import stl;

module utility;

namespace infinity {

SizeT NextPowerOfTwo(SizeT input) {
    --input;
    input |= input >> 1;
    input |= input >> 2;
    input |= input >> 4;
    input |= input >> 8;
    input |= input >> 16;
    input |= input >> 32;
    return ++input;
}

} // namespace infinity
