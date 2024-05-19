//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: tokenizer.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_TOKENIZER_H
#define MECAB_TOKENIZER_H

#include "mecab.h"
#include "freelist.h"
#include "dictionary.h"
#include "char_property.h"
#include "scoped_ptr.h"
// #include "token.h"

namespace MeCab {

class Param;

template <typename N, typename P>
class TokenizerImpl {
 private:
  std::vector<Dictionary *>              dic_;
  Dictionary                             unkdic_;
  scoped_string                          bos_feature_;
  scoped_string                          unk_feature_;
  FreeList<N>                            node_freelist_;
  FreeList<DictionaryInfo>               dictionary_info_freelist_;
  std::vector<std::pair<const Token *, size_t> > unk_tokens_;
  scoped_array<Dictionary::result_type>  daresults_;
  DictionaryInfo                        *dictionary_info_;
  CharInfo                               space_;
  CharProperty                           property_;
  size_t                                 max_grouping_size_;
  unsigned int                           id_;
  whatlog                                what_;

 public:

  inline N *getNewNode() {
    N *node = node_freelist_.alloc();
    std::memset(node, 0, sizeof(N));
    node->id = id_++;
    return node;
  }

  N *getBOSNode();
  N *getEOSNode();

  N *lookup(const char *begin, const char *end);

  bool open(const Param &);
  void close();
  void clear();

  const DictionaryInfo *dictionary_info() const;

  const char *what() { return what_.str(); }

  explicit TokenizerImpl();
  virtual ~TokenizerImpl() { this->close(); }
};

typedef TokenizerImpl<Node, Path>               Tokenizer;
typedef TokenizerImpl<LearnerNode, LearnerPath> LearnerTokenizer;
}
#endif
