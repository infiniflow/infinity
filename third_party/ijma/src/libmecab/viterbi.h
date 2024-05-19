//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: viterbi.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_VITERBI_H
#define MECAB_VITERBI_H

#include <vector>
#include "mecab.h"
#include "freelist.h"
#include "tokenizer.h"

namespace MeCab {

class Param;
class Connector;

class Viterbi {
 private:
  Tokenizer*           tokenizer_;
  Connector*           connector_;
  Node*                eosNode_;
  Node*                bosNode_;
  const char*          begin_;
  const char*          end_;
  int                  level_;
  double               theta_;
  int                  cost_factor_;
  double               Z_;
  bool                 copy_sentence_;
  bool                 partial_;
  scoped_ptr<FreeList<Path> > path_freelist_;
  std::vector<Node *>  end_node_list_;
  std::vector<Node *>  begin_node_list_;
  std::vector<char>    sentence_;
  std::vector<char>    constraint_buf_;

  bool (Viterbi::*analyze_)(const char *, size_t);
  bool (Viterbi::*connect_)(size_t, Node *);
  Node *(Viterbi::*buildLattice_)();

  whatlog what_;

  Node *lookup(size_t pos);

  bool connectNormal(size_t pos, Node *node);
  bool connectWithAllPath(size_t pos, Node *node);

  bool forwardbackward(const char *str, size_t len);
  bool viterbi(const char *str, size_t len);

  Node *filterNode(Node *node, size_t len);
  bool initConstraints(const char **sen, size_t *len);

  Node *buildBestLattice();
  Node *buildAllLattice();

  inline bool connect(size_t p, Node* n) {
    return(this->*connect_)(p, n);
  }

 public:
  void clear();
  bool open(const Param &param, Tokenizer *t, Connector *c);
  void close();

  double Z() const { return Z_; }

  Node *analyze(const char *str, size_t len);

  const char *what() { return what_.str(); }

  bool partial() const { return partial_; }
  void set_partial(bool partial) { partial_ = partial; }

  float theta() const { return theta_ * cost_factor_; }
  void set_theta(float theta) { theta_ = theta / cost_factor_; }

  int  lattice_level() const { return level_; }
  void  set_lattice_level(int level);

  bool  all_morphs() const;
  void  set_all_morphs(bool all_morphs);

  explicit Viterbi();
  virtual ~Viterbi();
};
}
#endif
