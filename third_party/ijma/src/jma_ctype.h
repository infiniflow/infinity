/** \file jma_ctype.h
 * \brief JMA_CType gives the character type information.
 * \author Vernkin
 * \version 0.1
 * \date Jun 25, 2009
 */

#ifndef JMA_CTYPE_H
#define JMA_CTYPE_H

#include "knowledge.h" // Knowledge::EncodeType

#include <string>
using std::string;

namespace jma {

/**
 * \brief JMA_CType gives the character type information.
 *
 * CMA_CType gives the character type information.
 */
class JMA_CType {
public:
    /**
     * Create an instance of \e CMA_CType based on the character encode type.
     * \param type the character encode type
     * \return the pointer to instance
     */
    static JMA_CType *instance(Knowledge::EncodeType type);

    /**
     * Destrucor
     */
    virtual ~JMA_CType();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return the count of bytes.
     */
    virtual unsigned int getByteCount(const char *p) const = 0;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     */
    virtual bool isSpace(const char *p) const = 0;

    /**
     * Get the number of the characters in the p
     * \param p pointer to the string to be checked
     * \return the number of the characters in the p
     */
    size_t length(const char *p) const;
};

} // namespace jma

#endif // JMA_CTYPE_H
