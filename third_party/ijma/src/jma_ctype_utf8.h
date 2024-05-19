/** \file jma_ctype_utf8.h
 * Definition of class JMA_CType_UTF8.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#ifndef JMA_CTYPE_UTF8_H_
#define JMA_CTYPE_UTF8_H_

#include "jma_ctype.h"

namespace jma {

/**
 * JMA_CType_UTF8 gives the character type information in UTF-8 encoding.
 */
class JMA_CType_UTF8 : public JMA_CType {
public:
    /**
     * Create an instance of \e JMA_CType_UTF8.
     * \return the pointer to instance (invoke new here)
     */
    static JMA_CType_UTF8 *instance();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return the count of bytes.
     */
    virtual unsigned int getByteCount(const char *p) const;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     * \attention UTF-8 byte-order mark (0xEF 0xBB 0xBF) is also assumed as white-space character.
     */
    virtual bool isSpace(const char *p) const;

private:
    /**
     * Constructor.
     */
    JMA_CType_UTF8();
};

} // namespace jma

#endif // JMA_CTYPE_UTF8_H_
