/** \file char_table.h
 * Definition of class CharTable.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#ifndef JMA_CHAR_TABLE_H
#define JMA_CHAR_TABLE_H

#include "knowledge.h" // Knowledge::EncodeType

#include <map>
#include <string>

namespace MeCab {
class Iconv;
} // namespace MeCab

namespace jma {

/**
 * CharTable stores the mapping table between characters from one type to another.
 */
class CharTable {
public:
    /**
     * Constructor.
     */
    CharTable();

    /**
     * Load the configuration file, such as "map-kana.def", which is in text format.
     * This file contains the mapping table between one type to another, which format is "LeftType RightType".
     * \param fileName the file name
     * \param iconv to convert the input file's encoding type to run time encoding type
     * \return true for success, false for fail
     * \attention if this function is already called before, the table previously loaded would be removed.
     */
    bool loadConfig(const char *fileName, MeCab::Iconv &iconv);

    /**
     * Convert character to the right type, other character not in the left type is returned as 0.
     * \param str the character string
     * \return if \e str is in the left type, its right type equivalent is returned, otherwise 0 is returned
     */
    const char *toRight(const char *str) const;

    /**
     * Convert character to the left type, other character not in the right type is returned as 0.
     * \param str the character string
     * \return if \e str is in the right type, its left type equivalent is returned, otherwise 0 is returned
     */
    const char *toLeft(const char *str) const;

private:
    /** type of mapping between characters */
    typedef std::map<std::string, std::string> CharMap;

    /** map from left to right type */
    CharMap mapToRight_;

    /** map from right to left type */
    CharMap mapToLeft_;
};

} // namespace jma

#endif // JMA_CHAR_TABLE_H
