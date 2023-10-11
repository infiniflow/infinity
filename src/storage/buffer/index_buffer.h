#pragma once

#include "faiss/Index.h"
namespace infinity {

class IndexBuffer {
public:
private:
    faiss::Index *index_{};
};

} // namespace infinity