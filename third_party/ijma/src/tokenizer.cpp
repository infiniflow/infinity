/** \file source/src/tokenizer.cpp
 * \brief Tokenizer tokenizes a raw input string in specific encoding to a sequence of characters.
 * \author Jun Jiang
 * \version 0.1
 */

#include "tokenizer.h"
#include "jma_ctype.h"

#include <cassert>
#include <cstring>

using namespace std;

namespace jma {

CTypeTokenizer::CTypeTokenizer(const JMA_CType *ctype) : ctype_(ctype), raw_(0) {
    // initialize character buffer with 0
    memset(buffer_, 0, sizeof(buffer_));
}

CTypeTokenizer::CTypeTokenizer(const JMA_CType *ctype, const char *str) : ctype_(ctype), raw_(str) {
    // initialize character buffer with 0
    memset(buffer_, 0, sizeof(buffer_));
}

void CTypeTokenizer::assign(const char *str) {
    assert(str);

    raw_ = str;
}

const char *CTypeTokenizer::next() {
    if (raw_ == 0 || *raw_ == 0)
        return 0;
    unsigned int bc = ctype_->getByteCount(raw_);
    assert(bc > 0 && bc < BUFFER_SIZE);

    for (unsigned int i = 0; i < bc; ++i)
        buffer_[i] = *(raw_ + i);

    // append 0 as end of string
    buffer_[bc] = 0;

    // move to next character
    raw_ += bc;
    return buffer_;
}

} // namespace jma
