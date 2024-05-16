/**
 * \file jma_ctype_sjis.h
 * \brief encoding for the SHIFT-JIS
 *
 * \date Jun 25, 2009
 * \author vernkin
 */

#ifndef JMA_CTYPE_SJIS_H_
#define JMA_CTYPE_SJIS_H_

#include "jma_ctype.h"

namespace jma {

class JMA_CType_SJIS : public JMA_CType {

public:
    /**
     * Create an instance of JMA_CType_SJIS
     * \return the pointer to instance (invoke new here)
     */
    static JMA_CType_SJIS *instance();

    virtual ~JMA_CType_SJIS();

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
     */
    virtual bool isSpace(const char *p) const;

private:
    JMA_CType_SJIS();
};

} // namespace jma

#endif /* JMA_CTYPE_SJIS_H_ */
