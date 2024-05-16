// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: viterbi.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <cstring>
#include <cmath>
#include <algorithm>
#include "param.h"
#include "viterbi.h"
#include "common.h"
#include "string_buffer.h"
#include "connector.h"

namespace MeCab {

namespace {
void calc_alpha(Node *n, double beta) {
  n->alpha = 0.0;
  for (Path *path = n->lpath; path; path = path->lnext)
    n->alpha = logsumexp(n->alpha,
                         -beta * path->cost + path->lnode->alpha,
                         path == n->lpath);
}

void calc_beta(Node *n, double beta) {
  n->beta = 0.0;
  for (Path *path = n->rpath; path; path = path->rnext)
    n->beta = logsumexp(n->beta,
                        -beta * path->cost + path->rnode->beta,
                        path == n->rpath);
}

bool partial_match(const char *f1, const char *f2) {
  char buf1[BUF_SIZE];
  char *c1[64];
  char buf2[BUF_SIZE];
  char *c2[64];
  std::strncpy(buf1, f1, sizeof(buf1));
  std::strncpy(buf2, f2, sizeof(buf2));

  size_t n1 = MeCab::tokenizeCSV(buf1, c1, sizeof(c1));
  size_t n2 = MeCab::tokenizeCSV(buf2, c2, sizeof(c2));
  size_t n  = _min(n1, n2);

  for (size_t i = 0; i < n; ++i) {
    if (std::strcmp(c1[i], "*") != 0 &&
        std::strcmp(c1[i], c2[i]) != 0) return false;
  }
  return true;
}
}

Viterbi::Viterbi() : level_(0), theta_(0.0), cost_factor_(0),
                     end_node_list_(0), begin_node_list_(0) {}

Viterbi::~Viterbi() { close(); }

bool Viterbi::open(const Param &param, Tokenizer *t, Connector *c) {
  tokenizer_ = t;
  connector_ = c;

  begin_node_list_.reserve(MIN_INPUT_BUFFER_SIZE);
  end_node_list_.reserve(MIN_INPUT_BUFFER_SIZE);
  path_freelist_.reset(0);

  copy_sentence_ = param.get<bool>("allocate-sentence");
  cost_factor_ = param.get<int>("cost-factor");
  CHECK_FALSE(cost_factor_ > 0) << "cost-factor is empty";

  set_theta(param.get<double>("theta"));
  set_lattice_level(param.get<int>("lattice-level"));
  set_partial(param.get<bool>("partial"));
  set_all_morphs(param.get<bool>("all-morphs"));

  return true;
}

void Viterbi::close() {}

void Viterbi::set_lattice_level(int level) {
  level_ = level;
  connect_ = &Viterbi::connectNormal;
  analyze_ = &Viterbi::viterbi;
  if (level_ >= 1) {
    if (!path_freelist_.get()) {
      path_freelist_.reset(new FreeList<Path>(PATH_FREELIST_SIZE));
    }
    connect_ = &Viterbi::connectWithAllPath;
  }
  if (level_ >= 2) {
    analyze_ = &Viterbi::forwardbackward;
  }
}

bool Viterbi::all_morphs() const {
  return (buildLattice_ == &Viterbi::buildAllLattice);
}

void Viterbi::set_all_morphs(bool all_morphs) {
  buildLattice_ = all_morphs ?
      &Viterbi::buildAllLattice :
      &Viterbi::buildBestLattice;
}

Node *Viterbi::analyze(const char *str, size_t len) {
  if (!partial_ && copy_sentence_) {
    sentence_.resize(len + 1);
    std::strncpy(&sentence_[0], str, len);
    str = &sentence_[0];
  }

  end_node_list_.resize(len + 4);
  begin_node_list_.resize(len + 4);

  std::memset(&end_node_list_[0],   0,
              sizeof(end_node_list_[0]) * (len + 4));
  std::memset(&begin_node_list_[0], 0,
              sizeof(begin_node_list_[0]) *(len + 4));

  clear();

  if (partial_ && !initConstraints(&str, &len))
    return 0;

  if (!(this->*analyze_)(str, len))
    return 0;

  return(this->*buildLattice_)();
}

void Viterbi::clear() {
  tokenizer_->clear();
  Z_ = 0.0;
  if (path_freelist_.get()) path_freelist_->free();
}

bool Viterbi::forwardbackward(const char *sentence, size_t len) {
  if (!this->viterbi(sentence, len)) return false;

  end_node_list_[0]->alpha = 0.0;
  for (int pos = 0;   pos <= static_cast<long>(len);  ++pos)
    for (Node *node = begin_node_list_[pos]; node; node = node->bnext)
      calc_alpha(node, theta_);

  begin_node_list_[len]->beta = 0.0;
  for (int pos = static_cast<long>(len); pos >= 0;    --pos)
    for (Node *node = end_node_list_[pos]; node; node = node->enext)
      calc_beta(node, theta_);

  Z_ = begin_node_list_[len]->alpha;  // alpha of EOS

  for (int pos = 0;   pos <= static_cast<long>(len);  ++pos)
    for (Node *node = begin_node_list_[pos]; node; node = node->bnext)
      node->prob = std::exp(node->alpha + node->beta - Z_);

  return true;
}

bool Viterbi::viterbi(const char *sentence, size_t len) {
  bosNode_ = tokenizer_->getBOSNode();
  bosNode_->begin_node_list = &begin_node_list_[0];
  bosNode_->end_node_list = &end_node_list_[0];
  bosNode_->sentence_length = len;

  begin_ = sentence;
  end_   = begin_ + len;
  bosNode_->surface = begin_;
  end_node_list_[0] = bosNode_;

  for (size_t pos = 0; pos < len; ++pos) {
    if (end_node_list_[pos]) {
      Node * rNode = tokenizer_->lookup(begin_ + pos, end_);
      rNode = filterNode(rNode, pos);
      begin_node_list_[pos] = rNode;
      if (!connect(pos, rNode)) return false;
    }
  }

  eosNode_ = tokenizer_->getEOSNode();
  eosNode_->surface = end_;
  begin_node_list_[len] = eosNode_;
  for (long pos = len; static_cast<long>(pos) >= 0; --pos) {
    if (end_node_list_[pos]) {
      if (!connect(pos, eosNode_)) return 0;
      break;
    }
  }

  return true;
}

Node *Viterbi::buildAllLattice() {
  if (!buildBestLattice()) return 0;

  Node *prev = bosNode_;
  const size_t len = static_cast<size_t>(end_ - begin_);

  for (long pos = 0; pos <= static_cast<long>(len); ++pos) {
    for (Node *node = begin_node_list_[pos]; node; node = node->bnext) {
      prev->next = node;
      node->prev = prev;
      prev = node;
      for (Path *path = node->lpath; path; path = path->lnext)
        path->prob = std::exp(path->lnode->alpha
                              - theta_ * path->cost
                              + path->rnode->beta - Z_);
    }
  }

  return bosNode_;
}

Node *Viterbi::buildBestLattice() {
  Node *node = eosNode_;
  for (Node *prevNode; node->prev;) {
    node->isbest = 1;
    prevNode = node->prev;
    prevNode->next = node;
    node = prevNode;
  }

  return bosNode_;
}

bool Viterbi::initConstraints(const char **sentence,
                              size_t *len) {
  constraint_buf_.resize(*len + 1);
  char *str = &constraint_buf_[0];
  std::strncpy(str, *sentence, *len);

  std::vector<char *> lines;
  const size_t lsize = tokenize(str, "\n", std::back_inserter(lines), 0xffff);
  CHECK_FALSE(0xffff != lsize) << "too long lines";

  char* column[2];
  size_t pos = 1;

  sentence_.resize(*len + 1);
  StringBuffer os(&sentence_[0], *len + 1);
  os << ' ';

  for (size_t i = 0; i < lsize; ++i) {
    const size_t size = tokenize(lines[i], "\t", column, 2);
    if (size == 1 && std::strcmp(column[0], "EOS") == 0) break;
    os << column[0] << ' ';
    const size_t len = std::strlen(column[0]);
    if (size == 2) {
      CHECK_FALSE(*column[1] != '\0') << "use \\t as separator";
      Node *c = tokenizer_->getNewNode();
      c->surface = column[0];
      c->feature = column[1];
      c->length  = len;
      c->rlength = len + 1;
      c->bnext = 0;
      c->wcost = 0;
      begin_node_list_[pos - 1] = c;
    }
    pos += len + 1;
  }

  os << '\0';

  *sentence = const_cast<const char*>(os.str());
  *len = pos - 1;  // remove the last ' '

  return true;
}

Node* Viterbi::filterNode(Node *node, size_t pos) {
  if (!partial_) return node;

  Node *c = begin_node_list_[pos];
  if (!c) return node;

  Node *prev = 0;
  Node *result = 0;

  for (Node *n = node; n; n = n->bnext) {
    if (c->length == n->length && (std::strcmp(c->feature, "*") == 0 ||
                                   partial_match(c->feature, n->feature))) {
      if (prev) {
        prev->bnext = n;
        prev = n;
      } else {
        result = n;
        prev = result;
      }
    }
  }

  if (!result) result = c;
  if (prev) prev->bnext = 0;

  return result;
}
}

#undef _VITERBI_WITH_ALL_PATH
#include "viterbisub.h"
#define _VITERBI_WITH_ALL_PATH
#include "viterbisub.h"
