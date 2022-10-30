//
// Created by JinHai on 2022/10/30.
//

#pragma once

namespace infinity {

enum class ChunkVectorType {
    kInvalid,
    kFlat,
    kConstant,
    kDictionary,
    kRLE, // Run length encoding
    kSequence,
    kBias,
    kHeterogeneous,
};

// Basic unit of column data vector
class ColumnVector {

};

}
