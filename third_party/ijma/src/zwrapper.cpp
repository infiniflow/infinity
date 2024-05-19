/** \file zwrapper.cpp
 * Implementation of class ZWrapper.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "zwrapper.h"

#include <cassert>

using namespace std;

namespace zlib {

ZWrapper::ZWrapper() : isDeflate_(false) {}

ZWrapper::~ZWrapper() {
    if (isDeflate_)
        deflateEnd();
}

bool ZWrapper::defalteInit() {
    zstrm_.zalloc = Z_NULL;
    zstrm_.zfree = Z_NULL;
    zstrm_.opaque = Z_NULL;

    if (::deflateInit(&zstrm_, Z_DEFAULT_COMPRESSION) == Z_OK) {
        isDeflate_ = true;
        return true;
    }

    return false;
}

bool ZWrapper::deflateToStream(const char *source, unsigned int sourceLen, std::ostream &os, bool isFlush) {
    if (!isDeflate_)
        return false;

    int ret;
    unsigned int have;
    int flush = isFlush ? Z_FINISH : Z_NO_FLUSH;
    zstrm_.next_in = (Bytef *)source;
    zstrm_.avail_in = sourceLen;

    // run deflate() on input until output buffer not full,
    // finish compression if all of source has been read in
    do {
        zstrm_.next_out = (Bytef *)outBuffer_;
        zstrm_.avail_out = OUT_BUFFER_SIZE;

        ret = ::deflate(&zstrm_, flush); // no bad return value
        assert(ret != Z_STREAM_ERROR);   // state not clobbered

        have = OUT_BUFFER_SIZE - zstrm_.avail_out;
        if (!os.write(outBuffer_, have)) {
            return false;
        }
    } while (zstrm_.avail_out == 0);
    assert(zstrm_.avail_in == 0); // all input will be used

    return true;
}

bool ZWrapper::deflateEnd() {
    if (isDeflate_) {
        isDeflate_ = false;
        return ::deflateEnd(&zstrm_) == Z_OK;
    }

    return false;
}

bool ZWrapper::uncompress(const char *source, unsigned int sourceLen, char *dest, unsigned int &destLen) {
    uLongf tempLen = destLen;

    if (::uncompress((Bytef *)dest, &tempLen, (const Bytef *)source, sourceLen) == Z_OK) {
        destLen = tempLen;
        return true;
    }

    return false;
}

} // namespace zlib
