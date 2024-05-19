/** \file jma_analyzer.cpp
 * Implementation of class JMA_Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#include <algorithm> // find
#include <cassert>
#include <cstring> // strlen
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "char_table.h"
#include "jma_analyzer.h"
#include "tokenizer.h"

#define JMA_DEBUG_PRINT_COMBINE 0

using namespace std;

namespace {
/** Base N-Best Score */
const int BASE_NBEST_SCORE = 200;

/** The scale factor for the limitation count of nbest.
 * As the NBest candidates of \e JMA_Analyzer::runWithSentence() is requred to be unique on segmentation/POS,
 * while \e MeCab::Tagger::nextNode() gives results differing in segementation, POS, base form, reading, etc,
 * so that those results duplicated on segmentation/POS would be removed, and \e MeCab::Tagger::nextNode() would be called continuously until it gives
 * the unique result. To avoid calling it forever, limit the maximum count to be (N * NBEST_LIMIT_SCALE_FACTOR).
 */
const int NBEST_LIMIT_SCALE_FACTOR = 100;

/** The limit size for parse input. */
const unsigned int LIMIT_PARSE_LENGTH = 8192;

/**
 * In JMA_Analyzer::iterateNode(), used as MorphemeProcessor to append morpheme to list.
 */
class MorphemeToList {
public:
    /**
     * Constructor.
     * \param list the morpheme list
     */
    MorphemeToList(jma::MorphemeList &list) : morphList_(list) {}

    /**
     * The process method appends morpheme to list.
     * \param morp the morpheme to append
     */
    void process(const jma::Morpheme &morp) { morphList_.push_back(morp); }

private:
    /** the morpheme list */
    jma::MorphemeList &morphList_;
};

/**
 * In JMA_Analyzer::iterateSentence(), used as SentenceProcessor to append sentence to list.
 */
class SentenceToList {
public:
    /**
     * Constructor.
     * \param list the sentence list
     */
    SentenceToList(vector<jma::Sentence> &list) : sentList_(list) {}

    /**
     * The process method appends sentence to list.
     * \param str the sentence string to append
     */
    void process(const char *str) {
        assert(str);

        sentList_.push_back(jma::Sentence());
        sentList_.back().setString(str);
    }

private:
    /** the sentence list */
    vector<jma::Sentence> &sentList_;
};

/**
 * In JMA_Analyzer::iterateSentence(), used as SentenceProcessor to analyze each sentence and append the analysis result to buffer.
 */
class SentenceToAnalyzerBuffer {
public:
    /**
     * Constructor.
     * \param analyzer the sentence analyzer
     * \param buf the result buffer
     */
    SentenceToAnalyzerBuffer(jma::JMA_Analyzer &analyzer, string &buf) : analyzer_(analyzer), buffer_(buf) {}

    /**
     * The process method analyzes sentence to buffer.
     * \param str the sentence string
     */
    void process(const char *str) {
        assert(str);

        jma::Sentence sentence(str);
        analyzer_.runOneBest(sentence);

        // get result only if it exists
        if (sentence.getListSize()) {
            assert(sentence.getListSize() == 1 && "one best analyze should contain only one result");

            int index = 0;
            bool isPOS = analyzer_.isOutputPOS();
            const char *posDelim = analyzer_.getPOSDelimiter();
            const char *wordDelim = analyzer_.getWordDelimiter();

            for (int j = 0; j < sentence.getCount(index); ++j) {
                buffer_ += sentence.getLexicon(index, j);
                if (isPOS) {
                    buffer_ += posDelim;
                    buffer_ += sentence.getStrPOS(index, j);
                }
                buffer_ += wordDelim;
            }
        }
    }

private:
    /** the sentence analyzer */
    jma::JMA_Analyzer &analyzer_;

    /** the result buffer */
    string &buffer_;
};

} // namespace

namespace jma {

/**
 * Whether the two MorphemeLists is the same
 * \param list1 the MorphemeList 1
 * \param list2 the MorphemeList 2
 * \param printPOS whether also compare POS result
 * \return true for the same, false for not the same
 */
inline bool isSameMorphemeList(const MorphemeList *list1, const MorphemeList *list2, bool printPOS) {
    // if one is zero pointer, return null
    if (!list1 || !list2) {
        return false;
    }

    if (list1->size() != list2->size())
        return false;
    // compare one by one
    size_t N = list1->size();
    if (printPOS) {
        for (size_t i = 0; i < N; ++i) {
            const Morpheme &m1 = (*list1)[i];
            const Morpheme &m2 = (*list2)[i];
            if (m1.lexicon_ != m2.lexicon_ || m1.posCode_ != m2.posCode_ || m1.posStr_ != m2.posStr_) {
                return false;
            }
        }
    } else {
        for (size_t i = 0; i < N; ++i) {
            if ((*list1)[i].lexicon_ != (*list2)[i].lexicon_)
                return false;
        }
    }
    // all the elements are the same
    return true;
}

JMA_Analyzer::JMA_Analyzer() : knowledge_(0), tagger_(0), posTable_(0), kanaTable_(0), widthTable_(0), caseTable_(0) {}

JMA_Analyzer::~JMA_Analyzer() { clear(); }

void JMA_Analyzer::clear() { delete tagger_; }

bool JMA_Analyzer::isPOSFormatAlphabet() const { return (getOption(OPTION_TYPE_POS_FORMAT_ALPHABET) != 0); }

bool JMA_Analyzer::isOutputPOS() const { return (getOption(OPTION_TYPE_POS_TAGGING) != 0); }

bool JMA_Analyzer::isCombineCompound() const { return (getOption(OPTION_TYPE_COMPOUND_MORPHOLOGY) != 0); }

bool JMA_Analyzer::isDecomposeUserNound() const { return (getOption(OPTION_TYPE_DECOMPOSE_USER_NOUN) != 0); }

POSTable::POSFormat JMA_Analyzer::getPOSFormat() const {
    POSTable::POSFormat type = POSTable::POS_FORMAT_DEFAULT;

    if (getOption(Analyzer::OPTION_TYPE_POS_FORMAT_ALPHABET) != 0) {
        type = POSTable::POS_FORMAT_ALPHABET;
    } else if (getOption(Analyzer::OPTION_TYPE_POS_FULL_CATEGORY) != 0) {
        type = POSTable::POS_FORMAT_FULL_CATEGORY;
    }

    return type;
}

int JMA_Analyzer::setKnowledge(Knowledge *pKnowledge) {
    assert(pKnowledge);

    // release the resources before allocating new ones
    clear();

    knowledge_ = dynamic_cast<JMA_Knowledge *>(pKnowledge);
    assert(knowledge_);

    tagger_ = knowledge_->createTagger();
    if (tagger_ == NULL) {
        cerr << "error: fail to create tagger, please insure that Knowledge::loadDict() returns 1 before this function is called." << endl;
        return 0;
    }
    tagger_->set_lattice_level(1);
    if (knowledge_->getCType() == NULL) {
        cerr << "error: fail to get character type, please insure that Knowledge::loadDict() returns 1 before this function is called." << endl;
        return 0;
    }

    posTable_ = &knowledge_->getPOSTable();
    kanaTable_ = &knowledge_->getKanaTable();
    widthTable_ = &knowledge_->getWidthTable();
    caseTable_ = &knowledge_->getCaseTable();
    decompMap_ = &knowledge_->getDecompMap();

    return 1;
}

int JMA_Analyzer::runWithSentence(Sentence &sentence) {
    assert(knowledge_ && knowledge_->getCType() && tagger_);

    int N = static_cast<int>(getOption(Analyzer::OPTION_TYPE_NBEST));
    assert(N > 0 && "the nbest option should be positive");

    if (N == 1) {
        runOneBest(sentence);
    } else {
        if (!runNBest(sentence, N))
            return 0;
    }

    return 1;
}

const char *JMA_Analyzer::runWithString(const char *inStr) {
    assert(knowledge_ && knowledge_->getCType() && tagger_);
    assert(inStr);

    strBuf_.clear();
    SentenceToAnalyzerBuffer processor(*this, strBuf_);
    iterateSentence(inStr, processor);

    return strBuf_.c_str();
}

int JMA_Analyzer::runWithStream(const char *inFileName, const char *outFileName) {
    assert(knowledge_ && knowledge_->getCType() && tagger_);
    assert(inFileName);
    assert(outFileName);

    ifstream in(inFileName);
    if (!in) {
        cerr << "[Error] The input file " << inFileName << " not exists!" << endl;
        return 0;
    }

    ofstream out(outFileName);
    if (!out) {
        cerr << "[Error] The output file " << outFileName << " could not be created!" << endl;
        return 0;
    }

    string line;
    while (getline(in, line))
        out << runWithString(line.c_str()) << endl;

    return 1;
}

void JMA_Analyzer::splitSentence(const char *paragraph, std::vector<Sentence> &sentences) {
    assert(knowledge_ && knowledge_->getCType());
    assert(paragraph);

    SentenceToList processor(sentences);
    iterateSentence(paragraph, processor);
}

void JMA_Analyzer::getFeatureStr(const char *featureList, int featureOffset, std::string &retVal) const {
    assert(featureList && featureOffset >= 0);

    const char delimit = ',';
    const char *str = featureList;
    const char *stre = featureList + strlen(featureList);
    const char *begin = 0;
    const char *end = 0;

    const int size = featureOffset + 1; // how many ',' to find
    int count = 0;                      // how many ',' have been found

    while (count < size && str < stre) {
        begin = str;
        end = find(str, stre, delimit);
        ++count; // also when 'end' reaches 'stre'

        if (end == stre)
            break;

        str = end + 1;
    }

    if (count == size) {
        retVal = string(begin, end - begin);
        if (retVal == "*")
            retVal = "";
    } else
        retVal = "";
}

Morpheme JMA_Analyzer::getMorpheme(const MeCab::Node *node) const {
    assert(node);

    Morpheme result;

    result.lexicon_.assign(node->surface, node->length);

    getFeatureStr(node->feature, knowledge_->getBaseFormOffset(), result.baseForm_);
    if (result.baseForm_.empty())
        result.baseForm_ = result.lexicon_;

    getFeatureStr(node->feature, knowledge_->getReadFormOffset(), result.readForm_);

    getFeatureStr(node->feature, knowledge_->getNormFormOffset(), result.normForm_);
    if (result.normForm_.empty())
        result.normForm_ = result.lexicon_;

    result.posCode_ = (int)node->posid;
    result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());

    return result;
}

MeCab::Node *JMA_Analyzer::combineNode(MeCab::Node *startNode, Morpheme &result) const {
    assert(startNode && startNode->next && "it is invalid to combine NULL or EOS node");

    result = getMorpheme(startNode);

    // check option
    if (!isCombineCompound())
        return startNode;

    MeCab::Node *node = startNode;
    int startPOS = (int)node->posid;
    while (node->next) {
        const RuleNode *ruleNode = posTable_->getCombineRule(startPOS, node->next);
        if (!ruleNode)
            break;

#if JMA_DEBUG_PRINT_COMBINE
        cerr << result.lexicon_ << "/" << result.posStr_;
#endif

        // start from next node
        for (int i = 1; i < ruleNode->level_; ++i) {
            node = node->next;
            assert(node->next && "the node should not be end-of-sentence node.");

            Morpheme morp = getMorpheme(node);
#if JMA_DEBUG_PRINT_COMBINE
            cerr << "\t+\t" << morp.lexicon_ << "/" << morp.posStr_;
#endif
            result.baseForm_ = result.lexicon_ + morp.baseForm_; // combined base form = previous lexicon + last base form
            result.lexicon_ += morp.lexicon_;
            result.readForm_ += morp.readForm_;
            result.normForm_ += morp.normForm_;
        }

        result.posCode_ = ruleNode->target_;
        result.posStr_ = posTable_->getPOS(result.posCode_, getPOSFormat());
        startPOS = result.posCode_; // to match rules from this combined result

#if JMA_DEBUG_PRINT_COMBINE
        cerr << "\t=>\t" << result.lexicon_ << "/" << result.posStr_ << endl;
#endif
    }

    return node;
}

template <class MorphemeProcessor>
void JMA_Analyzer::iterateNode(const MeCab::Node *bosNode, MorphemeProcessor &processor) const {
    Morpheme morp;
    bool isDecompose = isDecomposeUserNound();
    JMA_Knowledge::DecompMap::const_iterator iter;
    for (MeCab::Node *node = bosNode->next; node->next; node = node->next) {
        node = combineNode(node, morp);

        if (isDecompose && morp.posCode_ == knowledge_->getUserNounPOSIndex() && (iter = decompMap_->find(morp.lexicon_)) != decompMap_->end()) {
            // decompose into morpheme list
            const MorphemeList &morphList = iter->second;
            for (MorphemeList::const_iterator miter = morphList.begin(); miter != morphList.end(); ++miter) {
                if (isFilter(*miter))
                    continue;

                Morpheme decomp(*miter);
                decomp.baseForm_ = decomp.normForm_ = decomp.lexicon_; // no variant for user noun
                decomp.posCode_ = morp.posCode_;                       // index of POS user noun
                decomp.posStr_ = morp.posStr_;                         // string of POS user noun
                processor.process(decomp);
            }
        } else {
            if (isFilter(morp))
                continue;

            processor.process(morp);
        }
    }
}

std::string JMA_Analyzer::convertCharacters(const char *str) const {
    assert(knowledge_ && knowledge_->getCType());
    assert(str);

    string result;
    CTypeTokenizer tokenizer(knowledge_->getCType());
    tokenizer.assign(str);

    for (const char *p = tokenizer.next(); p; p = tokenizer.next()) {
        const char *q;
        if (getOption(OPTION_TYPE_CONVERT_TO_HIRAGANA) != 0) {
            if ((q = kanaTable_->toLeft(p)) != NULL)
                p = q;
        }
        if (getOption(OPTION_TYPE_CONVERT_TO_KATAKANA) != 0) {
            if ((q = kanaTable_->toRight(p)) != NULL)
                p = q;
        }
        if (getOption(OPTION_TYPE_CONVERT_TO_HALF_WIDTH) != 0) {
            if ((q = widthTable_->toLeft(p)) != NULL)
                p = q;
        }
        if (getOption(OPTION_TYPE_CONVERT_TO_FULL_WIDTH) != 0) {
            if ((q = widthTable_->toRight(p)) != NULL)
                p = q;
        }
        if (getOption(OPTION_TYPE_CONVERT_TO_LOWER_CASE) != 0) {
            if ((q = caseTable_->toLeft(p)) != NULL)
                p = q;
        }
        if (getOption(OPTION_TYPE_CONVERT_TO_UPPER_CASE) != 0) {
            if ((q = caseTable_->toRight(p)) != NULL)
                p = q;
        }

        result += p;
    }

    return result;
}

bool JMA_Analyzer::isFilter(const Morpheme &morph) const {
    if (knowledge_->getCType()->isSpace(morph.lexicon_.c_str()) || knowledge_->isStopWord(morph.lexicon_) ||
        !knowledge_->isKeywordPOS(morph.posCode_))
        return true;

    return false;
}

template <class SentenceProcessor>
void JMA_Analyzer::iterateSentence(const char *paragraph, SentenceProcessor &processor) {
    assert(paragraph);

    string sentenceStr;
    CTypeTokenizer tokenizer(knowledge_->getCType());
    tokenizer.assign(paragraph);

    for (const char *p = tokenizer.next(); p; p = tokenizer.next()) {
        sentenceStr += p;

        if (knowledge_->isSentenceSeparator(p)) {
            processor.process(sentenceStr.c_str());
            sentenceStr.clear();
        }
    }

    // in case the last character is not sentence separator
    if (!sentenceStr.empty()) {
        processor.process(sentenceStr.c_str());
        sentenceStr.clear();
    }
}

void JMA_Analyzer::splitLimitSize(const char *str, std::vector<std::string> &limitStrVec, unsigned int limitSize) const {
    string limitStr;
    CTypeTokenizer tokenizer(knowledge_->getCType());
    tokenizer.assign(str);
    for (const char *p = tokenizer.next(); p; p = tokenizer.next()) {
        if (limitStr.length() + strlen(p) >= limitSize) {
            limitStrVec.push_back(limitStr);
            limitStr.clear();
        }

        limitStr += p;
    }

    // the rest characters
    if (!limitStr.empty()) {
        limitStrVec.push_back(limitStr);
        limitStr.clear();
    }

    assert(validateSplitLimitResult(str, limitStrVec, limitSize));
}

bool JMA_Analyzer::validateSplitLimitResult(const char *str, const std::vector<std::string> &limitStrVec, unsigned int limitSize) const {
    string combineStr;
    for (unsigned int i = 0; i < limitStrVec.size(); ++i) {
        combineStr += limitStrVec[i];
        // all the splitted string size should be less than limitSize
        if (limitStrVec[i].length() >= limitSize) {
            cerr << "failed in JMA_Analyzer::validateSplitLimitResult()" << endl;
            cerr << "expect less than length: " << limitSize << endl;
            cerr << "actual length: " << limitStrVec[i].length() << endl;
            cerr << "split index: " << i << endl;
            cerr << "split string: " << limitStrVec[i] << endl << endl;
            return false;
        }
    }

    if (combineStr != str) {
        cerr << "failed in JMA_Analyzer::validateSplitLimitResult()" << endl;
        cerr << "original string: " << str << endl;
        cerr << "combination result: " << combineStr << endl;
        return false;
    }

    return true;
}

int JMA_Analyzer::getCodeFromStr(const std::string &posStr) const { return posTable_->getIndexFromAlphaPOS(posStr); }

void JMA_Analyzer::runOneBest(Sentence &sentence) const {
    vector<string> limitStrVec;
    splitLimitSize(sentence.getString(), limitStrVec, LIMIT_PARSE_LENGTH);

    MorphemeList list;
    MorphemeToList processor(list);

    for (vector<string>::const_iterator it = limitStrVec.begin(); it != limitStrVec.end(); ++it) {
        const MeCab::Node *bosNode = tagger_->parseToNode(it->c_str());
        iterateNode(bosNode, processor);
    }

    // ignore empty result
    if (!list.empty())
        sentence.addList(list, 1.0);
}

bool JMA_Analyzer::runNBest(Sentence &sentence, int nbest) const {
    assert(nbest > 1);

    vector<string> limitStrVec;
    splitLimitSize(sentence.getString(), limitStrVec, LIMIT_PARSE_LENGTH);

    bool isPOS = isOutputPOS();
    const int maxCount = nbest * NBEST_LIMIT_SCALE_FACTOR;

    // nbest result for whole input
    vector<MorphemeList> totalNBestVec;
    vector<double> totalScoreVec;

    for (vector<string>::const_iterator it = limitStrVec.begin(); it != limitStrVec.end(); ++it) {
        if (!tagger_->parseNBestInit(it->c_str())) {
            cerr << "error: fail to init nbest analyze on string: " << *it << endl;
            return false;
        }

        // nbest result for each buffer
        vector<MorphemeList> limitNBestVec;
        vector<double> limitScoreVec;
        long base = 0;
        // j to count steps to avoid iterating forever
        for (int j = 0; limitNBestVec.size() < static_cast<unsigned int>(nbest) && j < maxCount; ++j) {
            const MeCab::Node *bosNode = tagger_->nextNode();
            if (!bosNode)
                break;

            MorphemeList list;
            MorphemeToList processor(list);
            iterateNode(bosNode, processor);

            // ignore empty result
            if (list.empty())
                continue;

            bool isDupl = false;
            // check the current result with previous results
            for (vector<MorphemeList>::const_reverse_iterator rit = limitNBestVec.rbegin(); rit < limitNBestVec.rend(); ++rit) {
                if (isSameMorphemeList(&*rit, &list, isPOS)) {
                    isDupl = true;
                    break;
                }
            }
            // ignore the duplicate results
            if (isDupl)
                continue;

            long score = tagger_->nextScore();
            if (limitNBestVec.empty()) {
                if (score > 0)
                    base = score > BASE_NBEST_SCORE ? score - BASE_NBEST_SCORE : 0;
                else
                    base = score - BASE_NBEST_SCORE;
            }

            double dScore = 1.0 / (score - base);
            limitNBestVec.push_back(MorphemeList());
            limitNBestVec.back().swap(list);
            limitScoreVec.push_back(dScore);
        }
        assert(limitNBestVec.size() == limitScoreVec.size() && "the size of nbest and score limit results should be equal");

        // ignore empty nbest limit results
        if (limitNBestVec.empty())
            continue;

        // concatenate buffer result to whole result
        if (totalNBestVec.empty()) {
            totalNBestVec.swap(limitNBestVec);
            totalScoreVec.swap(limitScoreVec);
        } else {
            const unsigned int totalSize = totalNBestVec.size();
            const unsigned int limitSize = limitNBestVec.size();

            // append 1st candidate until total size and limit size are equal
            if (totalSize < limitSize) {
                totalNBestVec.insert(totalNBestVec.end(), limitSize - totalSize, totalNBestVec.front());
                totalScoreVec.insert(totalScoreVec.end(), limitSize - totalSize, totalScoreVec.front());
            } else if (limitSize < totalSize) {
                limitNBestVec.insert(limitNBestVec.end(), totalSize - limitSize, limitNBestVec.front());
                limitScoreVec.insert(limitScoreVec.end(), totalSize - limitSize, limitScoreVec.front());
            }

            // concatenate limit to total
            const unsigned int newTotalSize = totalNBestVec.size();
            assert(limitNBestVec.size() == newTotalSize && totalScoreVec.size() == newTotalSize && limitScoreVec.size() == newTotalSize &&
                   "the size of total and limit nbest results should be equal");
            for (unsigned int i = 0; i < newTotalSize; ++i) {
                totalNBestVec[i].insert(totalNBestVec[i].end(), limitNBestVec[i].begin(), limitNBestVec[i].end());
                totalScoreVec[i] += limitScoreVec[i];
            }
        }
    }
    assert(totalNBestVec.size() == totalScoreVec.size() && "the size of nbest and score total results should be equal");

    double totalScore = 0;
    for (unsigned int i = 0; i < totalScoreVec.size(); ++i)
        totalScore += totalScoreVec[i];
    // normalize denominator
    if (totalScore == 0)
        totalScore = 1;

    for (unsigned int i = 0; i < totalNBestVec.size(); ++i)
        sentence.addList(totalNBestVec[i], totalScoreVec[i] / totalScore);

    return true;
}

} // namespace jma
