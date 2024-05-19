//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: tokenizer.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include "tokenizer.h"
#include "viterbi.h"
#include "connector.h"
#include "param.h"
#include "common.h"
#include "utils.h"
#include "darts.h"
#include "scoped_ptr.h"

#define DRESULT_SIZE 512

namespace MeCab {

namespace {

void inline read_node_info(const Dictionary &dic,
                           const Token &token,
                           LearnerNode **node) {
  (*node)->lcAttr  = token.lcAttr;
  (*node)->rcAttr  = token.rcAttr;
  (*node)->posid   = token.posid;
  (*node)->wcost2  = token.wcost;
  (*node)->token   = const_cast<MeCab::Token *>(&token);
  (*node)->feature = dic.feature(token);
}

void inline read_node_info(const Dictionary &dic,
                           const Token &token,
                           Node **node) {
  (*node)->lcAttr  = token.lcAttr;
  (*node)->rcAttr  = token.rcAttr;
  (*node)->posid   = token.posid;
  (*node)->wcost   = token.wcost;
  (*node)->token   = const_cast<MeCab::Token *>(&token);
  (*node)->feature = dic.feature(token);
}
}

template class TokenizerImpl<Node, Path>;
template class TokenizerImpl<LearnerNode, LearnerPath>;

#if __GNUC__ >= 4
template TokenizerImpl<Node, Path>::TokenizerImpl();
template void TokenizerImpl<Node, Path>::clear();
template void TokenizerImpl<Node, Path>::close();
template const DictionaryInfo
*TokenizerImpl<Node, Path>::dictionary_info() const;
template Node* TokenizerImpl<Node, Path>::getBOSNode();
template Node* TokenizerImpl<Node, Path>::getEOSNode();
template Node* TokenizerImpl<Node, Path>::lookup(const char*, const char*);
template bool TokenizerImpl<Node, Path>::open(const Param &);
template TokenizerImpl<LearnerNode, LearnerPath>::TokenizerImpl();
template void TokenizerImpl<LearnerNode, LearnerPath>::clear();
template void TokenizerImpl<LearnerNode, LearnerPath>::close();
template const DictionaryInfo
*TokenizerImpl<LearnerNode, LearnerPath>::dictionary_info() const;
template LearnerNode* TokenizerImpl<LearnerNode, LearnerPath>::getEOSNode();
template LearnerNode* TokenizerImpl<LearnerNode, LearnerPath>::getBOSNode();
template LearnerNode*
TokenizerImpl<LearnerNode, LearnerPath>::lookup(const char*, const char*);
template bool TokenizerImpl<LearnerNode, LearnerPath>::open(const Param &);
#endif

template <typename N, typename P>
TokenizerImpl<N, P>::TokenizerImpl():
    node_freelist_(NODE_FREELIST_SIZE),
    dictionary_info_freelist_(4),
    daresults_(new Dictionary::result_type[DRESULT_SIZE]),
    dictionary_info_(0), max_grouping_size_(0), id_(0) {}

template <typename N, typename P>
void TokenizerImpl<N, P>::clear() {
  node_freelist_.free();
  id_ = 0;
}

template <typename N, typename P>
N *TokenizerImpl<N, P>::getBOSNode() {
  N *bosNode = getNewNode();
  std::memset(bosNode, 0, sizeof(N));
  bosNode->surface = const_cast<const char *>(BOS_KEY);  // dummy
  bosNode->feature = bos_feature_.get();
  bosNode->isbest = 1;
  bosNode->id = id_ - 1;
  bosNode->stat = MECAB_BOS_NODE;
  return bosNode;
}

template <typename N, typename P>
N *TokenizerImpl<N, P>::getEOSNode() {
  N *eosNode = getBOSNode();  // same
  eosNode->stat = MECAB_EOS_NODE;
  return eosNode;
}

template <typename N, typename P>
bool TokenizerImpl<N, P>::open(const Param &param) {
  close();

  const std::string prefix = param.template get<std::string>("dicdir");

  const char *mode = param.template get<bool>("open-mutable-dictionary") ?
      "r+" : "r";

  CHECK_CLOSE_FALSE(unkdic_.open(create_filename
                                 (prefix, UNK_DIC_FILE).c_str(), mode))
      << unkdic_.what();
  CHECK_CLOSE_FALSE(property_.open(param)) << property_.what();

  Dictionary *sysdic = new Dictionary;

  CHECK_CLOSE_FALSE(sysdic->open
                    (create_filename(prefix, SYS_DIC_FILE).c_str(), mode))
      << sysdic->what();

  CHECK_CLOSE_FALSE(sysdic->type() == 0)
      << "not a system dictionary: " << prefix;

  property_.set_charset(sysdic->charset());
  dic_.push_back(sysdic);

  const std::string userdic = param.template get<std::string>("userdic");
  if (!userdic.empty()) {
    char buf[BUF_SIZE];
    char *_dic[BUF_SIZE];
    std::strncpy(buf, userdic.c_str(), sizeof(buf));
    size_t n = tokenizeCSV(buf, _dic, sizeof(_dic));
    for (size_t i = 0; i < n; ++i) {
      Dictionary *d = new Dictionary;
      CHECK_CLOSE_FALSE(d->open(_dic[i], mode)) << d->what();
      CHECK_CLOSE_FALSE(d->type() == 1)
          << "not a user dictionary: " << _dic[i];
      CHECK_CLOSE_FALSE(sysdic->isCompatible(*d))
          << "incompatible dictionary: " << _dic[i];
      dic_.push_back(d);
    }
  }

  dictionary_info_ = 0;
  dictionary_info_freelist_.free();
  for (int i = static_cast<int>(dic_.size() - 1); i >= 0; --i) {
    DictionaryInfo *d = dictionary_info_freelist_.alloc();
    d->next = dictionary_info_;
    d->filename = dic_[i]->filename();
    d->charset = dic_[i]->charset();
    d->size = dic_[i]->size();
    d->lsize = dic_[i]->lsize();
    d->rsize = dic_[i]->rsize();
    d->type = dic_[i]->type();
    d->version = dic_[i]->version();
    dictionary_info_ = d;
  }

  unk_tokens_.clear();
  for (size_t i = 0; i < property_.size(); ++i) {
    const char *key = property_.name(i);
    Dictionary::result_type n = unkdic_.exactMatchSearch(key);
    CHECK_CLOSE_FALSE(n.value != -1) << "cannot find UNK category: " << key;
    const Token *token = unkdic_.token(n);
    size_t size  = unkdic_.token_size(n);
    unk_tokens_.push_back(std::make_pair(token, size));
  }

  space_ = property_.getCharInfo(0x20);  // ad-hoc

  bos_feature_.reset_string(param.template get<std::string>("bos-feature"));

  const std::string tmp = param.template get<std::string>("unk-feature");
  unk_feature_.reset(0);
  if (!tmp.empty()) unk_feature_.reset_string(tmp);

  CHECK_CLOSE_FALSE(*bos_feature_ != '\0')
      << "bos-feature is undefined in dicrc";

  max_grouping_size_ = param.template get<size_t>("max-grouping-size");
  if (max_grouping_size_ == 0)
    max_grouping_size_ = DEFAULT_MAX_GROUPING_SIZE;

  return true;
}

#define ADDUNKNWON do {                                                 \
    const Token  *token = unk_tokens_[cinfo.default_type].first;        \
    size_t size   = unk_tokens_[cinfo.default_type].second;             \
    for (size_t k = 0; k < size; ++k) {                                 \
      N *newNode = getNewNode();                                        \
      read_node_info(unkdic_, *(token + k), &newNode);                  \
      newNode->token = (Token *)(token + k);                            \
      newNode->id = id_ - 1;                                            \
      newNode->char_type = cinfo.default_type;                          \
      newNode->surface = begin2;                                        \
      newNode->length = begin3 - begin2;                                \
      newNode->rlength = begin3 - begin;                                \
      newNode->bnext = resultNode;                                      \
      newNode->stat = MECAB_UNK_NODE;                                   \
      if (unk_feature_.get()) newNode->feature = unk_feature_.get();    \
      resultNode = newNode; } } while (0)

template <typename N, typename P>
N *TokenizerImpl<N, P>::lookup(const char *begin, const char *end) {
  CharInfo cinfo;
  N *resultNode = 0;
  size_t mblen = 0;
  size_t clen = 0;

  end = static_cast<size_t>(end - begin) >= 65535 ? begin + 65535 : end;
  const char *begin2 = property_.seekToOtherType(begin, end, space_,
                                                 &cinfo, &mblen, &clen);

  for (std::vector<Dictionary *>::const_iterator it = dic_.begin();
       it != dic_.end(); ++it) {
    size_t n = (*it)->commonPrefixSearch(begin2,
                                         static_cast<size_t>(end - begin2),
                                         daresults_.get(), DRESULT_SIZE);

    for (size_t i = 0; i < n; ++i) {
      size_t size  = (*it)->token_size(daresults_[i]);
      const Token *token = (*it)->token(daresults_[i]);
      for (size_t j = 0; j < size; ++j) {
        N *newNode = getNewNode();
        read_node_info(**it, *(token + j), &newNode);
        newNode->token = (Token *)(token + j);
        newNode->length = daresults_[i].length;
        newNode->rlength = begin2 - begin + newNode->length;
        newNode->surface = begin2;
        newNode->stat = MECAB_NOR_NODE;
        newNode->char_type = cinfo.default_type;
        newNode->bnext = resultNode;
        resultNode = newNode;
      }
    }
  }

  if (resultNode && !cinfo.invoke)  return resultNode;

  const char *begin3 = begin2 + mblen;
  const char *group_begin3 = 0;

  if (begin3 > end) {
    ADDUNKNWON;
    return resultNode;
  }

  if (cinfo.group) {
    const char *tmp = begin3;
    CharInfo fail;
    begin3 = property_.seekToOtherType(begin3, end, cinfo,
                                       &fail, &mblen, &clen);
    if (clen <= max_grouping_size_) ADDUNKNWON;
    group_begin3 = begin3;
    begin3 = tmp;
  }

  for (size_t i = 1; i <= cinfo.length; ++i) {
    if (begin3 > end) break;
    if (begin3 == group_begin3) continue;
    clen = i;
    ADDUNKNWON;
    if (!cinfo.isKindOf(property_.getCharInfo(begin3, end, &mblen))) break;
    begin3 += mblen;
  }

  if (!resultNode) { ADDUNKNWON; }

  return resultNode;
}

#undef ADDUNKNWON

template <typename N, typename P>
const DictionaryInfo *TokenizerImpl<N, P>::dictionary_info() const {
  return const_cast<const DictionaryInfo *>(dictionary_info_);
}

template <typename N, typename P>
void TokenizerImpl<N, P>::close() {
  for (std::vector<Dictionary *>::iterator it = dic_.begin();
       it != dic_.end(); ++it)
    delete *it;
  dic_.clear();
  unk_tokens_.clear();
  property_.close();
}
}
