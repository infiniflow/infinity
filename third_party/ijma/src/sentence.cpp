/** \file sentence.cpp
 * Implementation of class Sentence.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "sentence.h"

#include <algorithm>
#include <cassert>

namespace jma {

Morpheme::Morpheme() : posCode_(-1) {}

Morpheme::Morpheme(const std::string &lexicon,
                   int posCode,
                   const std::string &posStr,
                   const std::string &baseForm,
                   const std::string &readForm,
                   const std::string &normForm)
    : lexicon_(lexicon), posCode_(posCode), posStr_(posStr), baseForm_(baseForm), readForm_(readForm), normForm_(normForm) {}

Sentence::Sentence() {}

Sentence::Sentence(const char *pString) { raw_ = pString; }

void Sentence::setString(const char *pString) {
    raw_ = pString;
    candidates_.clear();
    scores_.clear();
}

const char *Sentence::getString(void) const { return raw_.c_str(); }

int Sentence::getListSize(void) const { return candidates_.size(); }

int Sentence::getCount(int nPos) const { return candidates_[nPos].size(); }

const char *Sentence::getLexicon(int nPos, int nIdx) const { return candidates_[nPos][nIdx].lexicon_.c_str(); }

int Sentence::getPOS(int nPos, int nIdx) const { return candidates_[nPos][nIdx].posCode_; }

const char *Sentence::getStrPOS(int nPos, int nIdx) const { return candidates_[nPos][nIdx].posStr_.c_str(); }

const char *Sentence::getBaseForm(int nPos, int nIdx) const { return candidates_[nPos][nIdx].baseForm_.c_str(); }

const char *Sentence::getReadForm(int nPos, int nIdx) const { return candidates_[nPos][nIdx].readForm_.c_str(); }

const char *Sentence::getNormForm(int nPos, int nIdx) const { return candidates_[nPos][nIdx].normForm_.c_str(); }

const MorphemeList *Sentence::getMorphemeList(int nPos) const { return &candidates_[nPos]; }

double Sentence::getScore(int nPos) const { return scores_[nPos]; }

void Sentence::setScore(int nPos, double nScore) { scores_[nPos] = nScore; }

int Sentence::getOneBestIndex(void) const {
    if (scores_.empty())
        return -1;

    assert(scores_.size() > 0 && scores_.size() == candidates_.size());

    return std::max_element(scores_.begin(), scores_.end()) - scores_.begin();
}

void Sentence::addList(MorphemeList &morphemeList, double score) {
    candidates_.push_back(MorphemeList());
    candidates_.back().swap(morphemeList);
    scores_.push_back(score);
}

} // namespace jma
