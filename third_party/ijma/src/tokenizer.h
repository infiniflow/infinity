/** \file source/include/tokenizer.h
 * \brief Tokenizer tokenizes a raw input string in specific encoding to a sequence of characters.
 * \author Vernkin
 * \version 0.1
 */

#ifndef JMA_TOKENIZER_H
#define JMA_TOKENIZER_H

#include "jma_ctype.h"

namespace jma {

class JMA_CType;

/**
 * \brief Tokenizer tokenizes a raw input string in specific encoding to a sequence of characters.
 *
 * Tokenizer tokenizes a raw input string in specific encoding to a sequence of characters.
 * Typically, the usage is like below:
 *
 * // create instance of encoding type
 * JMA_CType* ctype = JMA_CType_EUCJP::instance();
 *
 * // create instance of Tokenizer
 * Tokenizer tokenizer(*ctype);
 *
 * // set the raw input string
 * tokenizer.assign("...");
 *
 * // get each character
 * for(const char* p=tokenizer.next(); p; p=tokenizer.next())
 * {
 *      // print the character
 *      cout << p << endl;
 * }
 */
class CTypeTokenizer {
public:
    /**
     * Constructor.
     *
     * \param ctype reference to the specific character encoding
     */
    CTypeTokenizer(const JMA_CType *ctype);

    /*
     * construct with assign with initial string
     *
     * \param ctype reference to the specific character encoding
     * \param str initial string
     */
    CTypeTokenizer(const JMA_CType *ctype, const char *str);

    /**
     * Get the reference to the specific character encoding
     *
     * \return reference to the specific character encoding
     */
    const JMA_CType *getCType() const { return ctype_; }

    /**
     * Set a raw input string.
     * \param str pointer to the raw input string
     * \attention The raw input string should be encoded in \e ctype of
     * Constructor. As \e Tokenizer doesn't make a copy of \e str, the caller
     * must ensure the life time of \e str in the calling of \e next() afterward.
     */
    void assign(const char *str);

    /**
     * Get the next character in specific encoding.
     * \return pointer to the next character
     * \attention 0 is returned if there is no character left.
     */
    const char *next();

private:
    /** character encoding */
    const JMA_CType *ctype_;

    /** raw string */
    const char *raw_;

    /** size for character buffer of \e next() */
    enum { BUFFER_SIZE = 5 };

    /** character buffer for \e next() */
    char buffer_[BUFFER_SIZE];
};

} // namespace jma

#endif // JMA_TOKENIZER_H
