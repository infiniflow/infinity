/** \file knowledge.cpp
 * Implementation of class Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "knowledge.h"

#include <cassert>
#include <string>

using namespace std;

namespace {
/** the string of each encoding type */
const char *ENCODE_TYPE_STR[jma::Knowledge::ENCODE_TYPE_NUM] = {"EUC-JP", "SHIFT-JIS", "UTF-8"};

/**
 * Get a string in lower alphabets.
 * \param s the original string
 * \return the converted string with lower alphabets. For example, "euc-jp" is returned when "EUC-JP" is input.
 */
string toLower(const char *s) {
    string str(s);
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if ((c >= 'A') && (c <= 'Z')) {
            c += 'a' - 'A';
            str[i] = c;
        }
    }
    return str;
}
} // namespace

namespace jma {

Knowledge::Knowledge() : encodeType_(ENCODE_TYPE_NUM) {}

Knowledge::~Knowledge() {}

Knowledge::EncodeType Knowledge::getEncodeType() const { return encodeType_; }

Knowledge::EncodeType Knowledge::decodeEncodeType(const char *encodeStr) {
    assert(encodeStr);

    string lower = toLower(encodeStr);
    if (lower == "euc-jp" || lower == "eucjp") {
        return Knowledge::ENCODE_TYPE_EUCJP;
    } else if (lower == "shift-jis" || lower == "sjis") {
        return Knowledge::ENCODE_TYPE_SJIS;
    } else if (lower == "utf-8" || lower == "utf8") {
        return Knowledge::ENCODE_TYPE_UTF8;
    }

    // unknown character encoding type
    return Knowledge::ENCODE_TYPE_NUM;
}

const char *Knowledge::encodeStr(EncodeType encodeType) {
    if (encodeType < ENCODE_TYPE_NUM)
        return ENCODE_TYPE_STR[encodeType];

    // unknown encoding type
    return 0;
}

void Knowledge::setSystemDict(const char *dirPath) {
    assert(dirPath);

    systemDictPath_ = dirPath;
}

void Knowledge::addUserDict(const char *fileName, EncodeType type) {
    assert(fileName);

    userDictNames_.push_back(UserDictFileType(fileName, type));
}

} // namespace jma
