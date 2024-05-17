/**
 * \file jma_ctype_sjis.cpp
 * \brief encoding for the SHIFT-JIS
 *
 * \author vernkin
 */

#include "jma_ctype_sjis.h"

#include <cassert>

namespace jma {

JMA_CType_SJIS *JMA_CType_SJIS::instance() { return new JMA_CType_SJIS; }

JMA_CType_SJIS::JMA_CType_SJIS() {
    // TODO Auto-generated constructor stub
}

JMA_CType_SJIS::~JMA_CType_SJIS() {
    // TODO Auto-generated destructor stub
}

unsigned int JMA_CType_SJIS::getByteCount(const char *p) const {
    assert(p);
    const unsigned char *uc = (const unsigned char *)p;

    if (uc[0] == 0)
        return 0;

    if (uc[0] < 0x80)
        return 1; // encoding in ASCII

    assert(uc[1]);
    return 2; // encoding in SHIFT-JIS
}

bool JMA_CType_SJIS::isSpace(const char *p) const {
    assert(p);

    const unsigned char *uc = (const unsigned char *)p;

    if (uc[0] < 0x80)
        return isspace(uc[0]); // check by std library

    // full-width space in SHIFT-JIS
    if (uc[0] == 0x81 && uc[1] == 0x40)
        return true;

    return false;
}

} // namespace jma
