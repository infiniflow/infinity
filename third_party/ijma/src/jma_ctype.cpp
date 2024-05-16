/** \file jma_ctype.cpp
 * \brief JMA_CType gives the character type information.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#include "jma_ctype.h"
#include "jma_ctype_eucjp.h"
#include "jma_ctype_sjis.h"
#include "jma_ctype_utf8.h"

#include <cassert>

using namespace std;

namespace jma {

JMA_CType *JMA_CType::instance(Knowledge::EncodeType type) {
    switch (type) {
        case Knowledge::ENCODE_TYPE_EUCJP:
            return JMA_CType_EUCJP::instance();

        case Knowledge::ENCODE_TYPE_SJIS:
            return JMA_CType_SJIS::instance();

        case Knowledge::ENCODE_TYPE_UTF8:
            return JMA_CType_UTF8::instance();

        default:
            assert(false && "Unknown character encode type");
            return 0;
    }
}

JMA_CType::~JMA_CType() {}

size_t JMA_CType::length(const char *p) const {
    size_t ret = 0;
    while (p) {
        unsigned int len = getByteCount(p);
        // len can be 0
        if (!len)
            return ret;
        p += len;
        ++ret;
    }
    return ret;
}

} // namespace jma
