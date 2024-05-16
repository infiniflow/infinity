// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: connector.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_CONNECTOR_H
#define MECAB_CONNECTOR_H

#include "mecab.h"
#include "common.h"

namespace MeCab {
class Param;
template <class T> class Mmap;

class Connector {
 private:
  Mmap<short>    *cmmap_;
  short          *matrix_;
  unsigned short  lsize_;
  unsigned short  rsize_;
  whatlog         what_;

 public:

  bool open(const Param &param);
  void close();
  void clear() {}

  const char *what() { return what_.str(); }

  size_t left_size()  const { return static_cast<size_t>(lsize_); }
  size_t right_size() const { return static_cast<size_t>(rsize_); }

  void set_left_size(size_t lsize)  { lsize_ = lsize; }
  void set_right_size(size_t rsize) { rsize_ = rsize; }

  inline int cost(const Node *lNode, const Node *rNode) const {
    return matrix_[ lNode->rcAttr + lsize_ * rNode->lcAttr ] + rNode->wcost;
  }

  // access to raw matrix
  short *mutable_matrix() { return &matrix_[0]; }
  const short *matrix() const { return &matrix_[0]; }

  bool openText(const char *filename);
  bool open(const char *filename, const char *mode = "r");

  bool is_valid(size_t lid, size_t rid) const {
    return (lid >= 0 && lid < rsize_ && rid >= 0 && rid < lsize_);
  }

  static bool compile(const char *, const char *);

  explicit Connector():
      cmmap_(0), matrix_(0), lsize_(0), rsize_(0) {}

  virtual ~Connector() { this->close(); }
};
}
#endif
