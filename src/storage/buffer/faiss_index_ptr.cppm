module;

import third_party;

export module faiss_index_ptr;

namespace infinity {
export struct FaissIndexPtr {
    FaissIndex *index_;
    FaissIndex *quantizer_;

    FaissIndexPtr(FaissIndex *index, FaissIndex *quantizer) : index_(index), quantizer_(quantizer) {}
};

} // namespace infinity