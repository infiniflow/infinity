/** \file jma_ctype_utf8.cpp
 * Implementation of class JMA_CType_UTF8.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#include "jma_ctype_utf8.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace {
/**
 * Print the \e byte to \e stream in hexadecimal base.
 * \param stream the stream to output
 * \param byte the byte value
 */
void printByte(ostream &stream, unsigned char byte) { stream << hex << showbase << static_cast<unsigned int>(byte) << dec; }

/**
 * Get the number of the continuous bit 1 in the head of byte \e b.
 * \param b the byte value
 * \return the count number. For example, the return value would be 2 when \e b is 11011010 in binary form.
 */
unsigned int countBitOne(unsigned char b) {
    unsigned int result = 0;

    for (unsigned char mask = 0x80; mask > 0; mask >>= 1) {
        if ((b & mask) == 0)
            break;

        ++result;
    }

    return result;
}
} // namespace

namespace jma {

JMA_CType_UTF8 *JMA_CType_UTF8::instance() { return new JMA_CType_UTF8; }

JMA_CType_UTF8::JMA_CType_UTF8() {}

/**
 * In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16
 * accessible range) are encoded using sequences of 1 to 4 octets.
 * The table below summarizes the format of these different octet types.
 * The letter x indicates bits available for encoding bits of the
 * character number.
 *
 * Char. number range  |        UTF-8 octet sequence
 *    (hexadecimal)    |              (binary)
 * --------------------+---------------------------------------------
 * 0000 0000-0000 007F | 0xxxxxxx
 * 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
 * 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
 * 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
unsigned int JMA_CType_UTF8::getByteCount(const char *p) const {
    assert(p);

    const unsigned char *uc = (const unsigned char *)p; // byte sequence
    const unsigned char first = uc[0];                  // the first byte
    const unsigned int bitCount = countBitOne(first);   // count of the continuous bit 1 in the first byte
    unsigned int result = 0;

    switch (bitCount) {
        case 0:
            if (first == 0)
                result = 0; // NULL
            else
                result = 1; // ASCII

            break;

        case 1:
            cerr << "invalid byte: ";
            printByte(cerr, first);
            cerr << ", it could only be the second, third or fourth byte of a multi-byte sequence of UTF-8." << endl;
            result = 1; // ignore the invalid byte
            break;

        case 2:
            // check the first byte in 2-byte sequence
            if (first == 0xC0 || first == 0xC1) {
                cerr << "invalid byte: ";
                printByte(cerr, first);
                cerr << ", it is a start of 2-byte sequence, but its code point is less than 0x80." << endl;
                result = 1; // ignore the invalid byte
                break;
            }
        case 3:
        case 4:
            // check the subsequent bytes
            for (unsigned int i = 1; i < bitCount; ++i) {
                if (uc[i] == 0 || countBitOne(uc[i]) != 1) {
                    cerr << "invalid byte: ";
                    printByte(cerr, first);
                    cerr << ", it is followed by the " << i << "th byte: ";
                    printByte(cerr, uc[i]);
                    cerr << " in a multi-byte sequnce of UTF-8." << endl;
                    result = 1; // ignore the invalid byte
                    break;
                }
            }
            if (result == 1) {
                break;
            }

            // check the first byte in 4-byte sequence
            if (first >= 0xF5 && first <= 0xF7) {
                cerr << "invalid byte: ";
                printByte(cerr, first);
                cerr << ", it is a start of 4-byte sequence, but its code point is above 0x10FFFF." << endl;
                result = 1; // ignore the invalid byte
                break;
            }

            result = bitCount;
            break;

        default:
            cerr << "invalid byte: ";
            printByte(cerr, first);
            cerr << ", it is not defined in a multi-byte sequnce of UTF-8." << endl;
            result = 1; // ignore the invalid byte
            break;
    }

    return result;
}

bool JMA_CType_UTF8::isSpace(const char *p) const {
    assert(p);

    const unsigned char *uc = (const unsigned char *)p;

    if (uc[0] < 0x80)
        return isspace(uc[0]); // check by std library

    if (getByteCount(p) == 3) {
        if ((uc[0] == 0xE3 && uc[1] == 0x80 && uc[2] == 0x80)     // UTF-8 full-width space
            || (uc[0] == 0xEF && uc[1] == 0xBB && uc[2] == 0xBF)) // UTF-8 byte-order mark
            return true;
    }

    return false;
}

} // namespace jma
