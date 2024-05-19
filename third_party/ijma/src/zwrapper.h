/** \file zwrapper.h
 * Definition of class ZWrapper, a wrapper class of zlib.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 10, 2010
 */

#ifndef ZLIB_ZWRAPPER_H
#define ZLIB_ZWRAPPER_H

#include "zlib.h"

#include <ostream>

namespace zlib {

/**
 * ZWrapper wraps the compress/uncompress functions for zlib.
 */
class ZWrapper {
public:
    /**
     * Constructor.
     */
    ZWrapper();

    /**
     * Destructor.
     */
    ~ZWrapper();

    /**
     * Initializes the internal stream state for compression.
     * \return true for success, false for failure.
     */
    bool defalteInit();

    /**
     * Compress the source buffer and write into the stream.
     * \param source the source buffer
     * \param sourceLen the length of source buffer
     * \param os the output stream
     * \param isFlush whether force to flush compressed data into stream, it is generally set to true when the source buffer reaches end-of-file.
     * \return true for success, false for failure
     */
    bool deflateToStream(const char *source, unsigned int sourceLen, std::ostream &os, bool isFlush);

    /**
     * Free the internal stream state.
     * \return true for success, false for failure.
     */
    bool deflateEnd();

    /**
     * Decompresses the source buffer into the destination buffer.
     * \param source the source buffer
     * \param sourceLen the length of source buffer
     * \param dest the destination buffer
     * \param destLen the length of destination buffer, upon exit, it would be the actual size of the uncompressed buffer
     * \return true for success, false for failure
     */
    bool uncompress(const char *source, unsigned int sourceLen, char *dest, unsigned int &destLen);

private:
    /** stream for zlib */
    z_stream zstrm_;

    /** deflate status */
    bool isDeflate_;

    enum {
        OUT_BUFFER_SIZE = 0x4000 ///< size of output buffer
    };

    /** out buffer for zlib */
    char outBuffer_[OUT_BUFFER_SIZE];
};

} // namespace zlib

#endif // ZLIB_ZWRAPPER_H
