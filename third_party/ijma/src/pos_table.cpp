/** \file pos_table.cpp
 * Implementation of class POSTable.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#include "pos_table.h"
#include "iconv_utils.h" // MeCab::Iconv
#include "jma_dictionary.h"
#include "knowledge.h" // Knowledge::encodeStr()

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#define JMA_DEBUG_PRINT_COMBINE 0

using namespace std;

namespace jma {
POSTable::POSTable() : strTableVec_(POS_FORMAT_NUM), tableSize_(0), ruleRoot_(0) {}

POSTable::~POSTable() { delete ruleRoot_; }

bool POSTable::loadConfig(const char *fileName, MeCab::Iconv &iconv) {
    assert(fileName);

    // remove the previous table if exists
    tableSize_ = 0;
    for (unsigned int i = 0; i < strTableVec_.size(); ++i)
        strTableVec_[i].clear();

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
    string line, fullPOS, partPOS, alphabetPOS;
    string::size_type i, j, k;
    const char *whitespaces = " \t";

    // to convert from index string to int value
    istringstream iss;
    int indexValue;

#if JMA_DEBUG_PRINT
    cout << "load POS table: " << fileName << endl;
    cout << "src charset: " << src << endl;
    cout << "dest charset: " << dest << endl;
    cout << "fullPOS\t\tindex\tAlphabet partPOS" << endl;
#endif

    // each line is assumed in the format "fullPOS index AlphabetPOS",
    // those lines not in this format would be ignored
    while (getline(from, line)) {
        line = line.substr(0, line.find('\r'));
        if (line.empty())
            continue;

        // set fullPOS
        j = line.find_first_of(whitespaces);
        if (j == string::npos)
            continue;
        fullPOS = line.substr(0, j);

        // convert encoding
        if (!iconv.convert(&fullPOS)) {
            cerr << "error to convert encoding for POS string " << fullPOS << endl;
            return false;
        }
        k = fullPOS.find('*');
        if (k == string::npos)
            partPOS = fullPOS;
        else if (k == 0)
            partPOS = "";
        else
            partPOS = fullPOS.substr(0, k - 1); // get "AAA" from "AAA,*"

        // set index
        i = line.find_first_not_of(whitespaces, j + 1);
        if (i == string::npos)
            continue;
        j = line.find_first_of(whitespaces, i);
        if (j == string::npos)
            continue;
        iss.clear();
        iss.str(line.substr(i, j - i));
        iss >> indexValue;

        // set alphabetPOS
        i = line.find_first_not_of(whitespaces, j + 1);
        if (i == string::npos)
            continue;
        j = line.find_first_of(whitespaces, i);
        if (j == string::npos)
            alphabetPOS = line.substr(i);
        else
            alphabetPOS = line.substr(i, j - i);

#if JMA_DEBUG_PRINT
        cout << fullPOS << "\t" << indexValue << "\t" << alphabetPOS << "\t" << partPOS << endl;
#endif

        if (indexValue < 0) {
            cerr << "the index code of POS (" << fullPOS << ", " << alphabetPOS << ") should not be a negative value: " << indexValue << endl;
            continue;
        }

        if (indexValue >= tableSize_) {
            tableSize_ = indexValue + 1;
            for (unsigned int i = 0; i < strTableVec_.size(); ++i)
                strTableVec_[i].resize(tableSize_);
        }

        strTableVec_[POS_FORMAT_DEFAULT][indexValue] = partPOS;
        strTableVec_[POS_FORMAT_ALPHABET][indexValue] = alphabetPOS;
        strTableVec_[POS_FORMAT_FULL_CATEGORY][indexValue] = fullPOS;

        alphaPOSMap_[alphabetPOS] = indexValue;
    }

#if JMA_DEBUG_PRINT
    cout << "total " << tableSize_ << " POS loaded" << endl << endl;
#endif

    return true;
}

const char *POSTable::getPOS(int index, POSFormat format) const {
    if (index < 0 || index >= tableSize_)
        return "";

    return strTableVec_[format][index].c_str();
}

bool POSTable::loadCombineRule(const char *fileName) {
    assert(fileName);

    // remove the previous rule Trie if exists
    delete ruleRoot_;
    ruleRoot_ = new RuleNode(0, tableSize_);

    // open file
    ifstream from(fileName);
    if (!from) {
        return false;
    }

    // read file
    string line, pos;
    vector<string> posVec;
    vector<string>::const_iterator it;
    istringstream iss;

#if JMA_DEBUG_PRINT
    cout << "load POS rule: " << fileName << endl;
    cout << "source1 source2 ... target" << endl;
#endif

    // each line is assumed in the format "source1 source2 ... target",
    // those lines not in this format would be ignored
    while (getline(from, line)) {
        line = line.substr(0, line.find('\r'));
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        iss.clear();
        posVec.clear();
        iss.str(line);
        while (iss >> pos) {
            posVec.push_back(pos);
        }

        if (posVec.size() < 2) {
            cerr << "ignore invalid rule: " << line << endl;
            cerr << "it should be \"source1 source2 ... target\"." << endl;
            continue;
        }

#if JMA_DEBUG_PRINT
        for (it = posVec.begin(); it != posVec.end(); ++it) {
            cout << *it << "\t";
        }
        cout << endl;
#endif

        RuleNode *node = ruleRoot_;
        bool isValid = true;
        for (it = posVec.begin(); it != posVec.end() - 1; ++it) {
            int posIndex = getIndexFromAlphaPOS(*it);
            if (posIndex < 0 || posIndex >= tableSize_) {
                isValid = false;
                break;
            }

            if (!node->children_[posIndex])
                node->children_[posIndex] = new RuleNode(node->level_ + 1, tableSize_);

            node = node->children_[posIndex];
        }

        int targetPOS = getIndexFromAlphaPOS(*it);
        if (!isValid || targetPOS < 0) {
            cerr << "ignore invalid rule: " << line << endl;
            cerr << "some POS string in this rule is unknown." << endl;
            continue;
        }

        // save target only if not assigned before
        if (node->target_ < 0)
            node->target_ = targetPOS;
    }

#if JMA_DEBUG_PRINT
    cout << endl;
#endif

    return true;
}

const RuleNode *POSTable::getCombineRule(int startPOS, const MeCab::Node *nextNode) const {
    assert(startPOS >= 0 && nextNode);

    const RuleNode *ruleNode = 0;
    const RuleNode *result = 0;

    // check whether "compound.def" is loaded
    if (ruleRoot_)
        ruleNode = ruleRoot_->children_[startPOS];

    const MeCab::Node *tokenNode = nextNode;
    while (ruleNode) {
        if (ruleNode->target_ >= 0)
            result = ruleNode;

        // in case of tokenNode is EOS node
        if (!tokenNode->next)
            break;

        ruleNode = ruleNode->children_[(int)tokenNode->posid];
        tokenNode = tokenNode->next;
    }

#if JMA_DEBUG_PRINT_COMBINE
    if (result)
        cout << result->level_ << " POSes are combined into " << strTableVec_[POS_FORMAT_ALPHABET][result->target_] << endl;
    else
        cout << "no rule found" << endl;
#endif

    return result;
}

int POSTable::getIndexFromAlphaPOS(const std::string &posStr) const {
    map<std::string, int>::const_iterator it = alphaPOSMap_.find(posStr);
    if (it != alphaPOSMap_.end())
        return it->second;

    return -1;
}

} // namespace jma
