#pragma once

#include "faiss/Index.h"

namespace infinity {
struct FaissIndexPtr {
    faiss::Index *index_;
    faiss::Index *quantizer_;

    FaissIndexPtr(faiss::Index *index, faiss::Index *quantizer) : index_(index), quantizer_(quantizer) {}
};

} // namespace infinity