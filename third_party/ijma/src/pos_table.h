/** \file pos_table.h
 * Definition of class POSTable.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 11, 2009
 */

#ifndef JMA_POS_TABLE_H
#define JMA_POS_TABLE_H

#include "knowledge.h" // Knowledge::EncodeType
#include "mecab.h"     // MeCab::Node

#include <map>
#include <string>
#include <vector>

namespace MeCab {
class Iconv;
} // namespace MeCab

namespace jma {

/**
 * RuleNode is a Trie node, the path from root to leaf means a rule to combine POS.
 * For example, the rule below:
 * src1 src2 ... srcN => target,
 * would combine src1, src2, ..., srcN (N >= 1) into target.
 *
 * The corresponding path would be:
 * root src1Node src2Node ... srcNNode,
 * the target value is saved in srcNNode.
 */
struct RuleNode {
    int level_;                        ///< the node level in Trie, root node is 0, its child is 1, etc
    int target_;                       ///< POS index code of target, -1 for no rule is defined with this ending node
    std::vector<RuleNode *> children_; ///< the pointers to each child, the index is POS index code, pointer 0 for the child does not exist

    /**
     * Constructor.
     */
    RuleNode(int level, int childSize) : level_(level), target_(-1), children_(childSize) {}

    /**
     * Destructor.
     */
    ~RuleNode() {
        for (std::vector<RuleNode *>::iterator it = children_.begin(); it != children_.end(); ++it)
            delete *it;
    }
};

/**
 * POSTable stores the table of part-of-speech tags, each tag includes a string value and its index code.
 * Note that the string value of part-of-speech tag is in Alphabet format such like "NP-S".
 */
class POSTable {
public:
    /**
     * Constructor.
     */
    POSTable();

    /**
     * Destructor.
     */
    ~POSTable();

    /**
     * POS format type.
     */
    enum POSFormat {
        POS_FORMAT_DEFAULT,       ///< default Japanese format such like "名詞,一般".
        POS_FORMAT_ALPHABET,      ///< Alphabet format such like "NC-G".
        POS_FORMAT_FULL_CATEGORY, ///< Japanese format of furl category such like "名詞,一般,*,*".
        POS_FORMAT_NUM            ///< the count of format types
    };

    /**
     * Load the configuration file "pos-id.def", which is in text format.
     * This file contains the part-of-speech configuration, which format is "JapanesePOS index AlphabetPOS".
     * \param fileName the file name
     * \param iconv to convert the input file's encoding type to run time encoding type
     * \return true for success, false for fail
     * \attention if this function is already called before, the table previously loaded would be removed.
     */
    bool loadConfig(const char *fileName, MeCab::Iconv &iconv);

    /**
     * Load the combination rule file "compound.def", which is in text format.
     * Each entry in this file would be like "NC-G    NS-G    NC-G",
     * the left two columns are POS to combine from,
     * the last column is POS as combination result.
     * \param fileName the file name
     * \return true for success, false for fail
     * \attention if this function is already called before, the rules previously loaded would be removed.
     */
    bool loadCombineRule(const char *fileName);

    /**
     * From POS index code, get POS string in specific format.
     * \param index the POS index code
     * \param format the POS string format
     * \return POS string, if non POS is available, the empty string is returned.
     */
    const char *getPOS(int index, POSFormat format = POS_FORMAT_DEFAULT) const;

    /**
     * Get the longest matched rule to combine the nodes.
     * \param startPOS the POS index code of the start node
     * \param nextNode the node next to the start node, the matching process ends before the end-of-sentence node is reached.
     * \return the leaf Trie node of the longest matched rule for combination, if no rule is found, 0 is returned.
     */
    const RuleNode *getCombineRule(int startPOS, const MeCab::Node *nextNode) const;

    /**
     * Get POS index code from POS string in alphabet format.
     * \param posStr POS string in alphabet format
     * \return POS index code, -1 for not found.
     */
    int getIndexFromAlphaPOS(const std::string &posStr) const;

private:
    /** the POS tag tables for each format type */
    std::vector<std::vector<std::string>> strTableVec_;

    /** the size of each POS tag table */
    int tableSize_;

    /** map from POS alphabet format to index code */
    std::map<std::string, int> alphaPOSMap_;

    /** root node to rule Trie */
    RuleNode *ruleRoot_;
};

} // namespace jma

#endif // JMA_POS_TABLE_H
