/** \file char_table.cpp
 * Implementation of class CharTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 12, 2010
 */

#include "char_table.h"
#include "iconv_utils.h" // MeCab::Iconv
#include "jma_dictionary.h"
#include "knowledge.h" // Knowledge::encodeStr()

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility> // make_pair

using namespace std;

namespace jma {
CharTable::CharTable() {}

bool CharTable::loadConfig(const char *fileName, MeCab::Iconv &iconv) {
    assert(fileName);

    // remove the previous tables if exist
    mapToLeft_.clear();
    mapToRight_.clear();

    // open file
    const DictUnit *dict = JMA_Dictionary::instance()->getDict(fileName);
    if (!dict) {
        cerr << "cannot find configuration file: " << fileName << endl;
        return false;
    }

    istringstream from(string(dict->text_, dict->length_));
    if (!from.good()) {
        cerr << "cannot read configuration file: " << fileName << endl;
        return false;
    }

    // read file
    string line, left, right;
    istringstream iss;

#if JMA_DEBUG_PRINT
    cout << "load char mapping table: " << fileName << endl;
    cout << "src charset: " << src << endl;
    cout << "dest charset: " << dest << endl;
    cout << "Left\tRight" << endl;
#endif

    // each line is assumed in the format "LeftType RightType",
    // those lines not in this format would be ignored
    while (getline(from, line)) {
        line = line.substr(0, line.find('\r'));
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        iss.clear();
        iss.str(line);
        iss >> left >> right;

        if (!iss) {
            cerr << "ignore the invalid char mapping line: " << line << endl;
            continue;
        }

        // convert encoding
        if (!iconv.convert(&left)) {
            cerr << "error to convert encoding for character " << left << endl;
            return false;
        }
        if (!iconv.convert(&right)) {
            cerr << "error to convert encoding for character " << right << endl;
            return false;
        }

#if JMA_DEBUG_PRINT
        cout << left << "\t" << right << endl;
#endif

        // only insert when not exists
        mapToLeft_.insert(make_pair(right, left));
        mapToRight_.insert(make_pair(left, right));
    }

#if JMA_DEBUG_PRINT
    cout << "total " << mapToLeft_.size() << " <=> " << mapToRight_.size() << " char mapping are loaded" << endl << endl;
#endif

    return true;
}

const char *CharTable::toLeft(const char *str) const {
    CharMap::const_iterator it = mapToLeft_.find(str);
    if (it == mapToLeft_.end())
        return 0;

    return it->second.c_str();
}

const char *CharTable::toRight(const char *str) const {
    CharMap::const_iterator it = mapToRight_.find(str);
    if (it == mapToRight_.end())
        return 0;

    return it->second.c_str();
}

} // namespace jma
