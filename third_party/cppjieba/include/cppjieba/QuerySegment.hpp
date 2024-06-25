#ifndef CPPJIEBA_QUERYSEGMENT_H
#define CPPJIEBA_QUERYSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentBase.hpp"
#include "FullSegment.hpp"
#include "MixSegment.hpp"
#include "Unicode.hpp"

namespace cppjieba {
class QuerySegment: public SegmentBase {
 public:
  QuerySegment(const string& dict, const string& model, const string& userDict = "")
    : mixSeg_(dict, model, userDict),
      trie_(mixSeg_.GetDictTrie()) {
  }
  QuerySegment(const DictTrie* dictTrie, const HMMModel* model)
    : mixSeg_(dictTrie, model), trie_(dictTrie) {
  }
  ~QuerySegment() {
  }

  void Cut(const string& sentence, vector<string>& words) const {
    Cut(sentence, words, true);
  }
  void Cut(const string& sentence, vector<string>& words, bool hmm) const {
    vector<Word> tmp;
    Cut(sentence, tmp, hmm);
    GetStringsFromWords(tmp, words);
  }
  void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    vector<WordRange> wrs;
    wrs.reserve(sentence.size()/2);
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      Cut(sentence, range.begin, range.end, wrs, hmm);
    }
    words.clear();
    words.reserve(wrs.size());
    GetWordsFromWordRanges(sentence, wrs, words);
  }

  bool IsAlphaDigit(const string& str) const {
    bool ret = true;
    for(unsigned i = 0; i < str.size(); i++) {
      ret &= (std::isalpha(str[i])||std::isdigit(str[i]));
    }
    return ret;
  }

  void Cut(const string &sentence, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange> &res, bool hmm) const {
    // use mix Cut first
    vector<WordRange> mixRes;
    mixSeg_.Cut(begin, end, mixRes, hmm);

    vector<WordRange> fullRes;
    for (vector<WordRange>::const_iterator mixResItr = mixRes.begin(); mixResItr != mixRes.end(); mixResItr++) {
      if (mixResItr->Length() > 2) {
        RuneStrArray::const_iterator left = mixResItr->left;
        RuneStrArray::const_iterator right = mixResItr->right;
        uint32_t len = right->offset - left->offset + right->len;
        string str = sentence.substr(left->offset, len);
        if(IsAlphaDigit(str)) {
          res.push_back(*mixResItr);
	  continue;
	}
        for (size_t i = 0; i + 1 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 1);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      } else {
        res.push_back(*mixResItr);
      }
      if (mixResItr->Length() > 3) {
        for (size_t i = 0; i + 2 < mixResItr->Length(); i++) {
          WordRange wr(mixResItr->left + i, mixResItr->left + i + 2);
          if (trie_->Find(wr.left, wr.right + 1) != NULL) {
            res.push_back(wr);
          }
        }
      }
    }
  }

 private:
  bool IsAllAscii(const Unicode& s) const {
   for(size_t i = 0; i < s.size(); i++) {
     if (s[i] >= 0x80) {
       return false;
     }
   }
   return true;
  }
  MixSegment mixSeg_;
  const DictTrie* trie_;
}; // QuerySegment

} // namespace cppjieba

#endif
