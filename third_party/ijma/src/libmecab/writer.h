//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: writer.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <string>
#include "mecab.h"
#include "utils.h"
#include "scoped_ptr.h"
#include "string_buffer.h"

namespace MeCab {

class Param;

class Writer {
 private:
  scoped_string  node_format_;
  scoped_string  bos_format_;
  scoped_string  eos_format_;
  scoped_string  unk_format_;
  scoped_string  eon_format_;
  whatlog what_;

  bool writeLattice(StringBuffer *s, const char *sent, const Node *node);
  bool writeWakati(StringBuffer *s, const char *sent, const Node *node);
  bool writeNone(StringBuffer *s, const char *sent, const Node *node);
  bool writeUser(StringBuffer *s, const char *sent, const Node *node);
  bool writeDump(StringBuffer *s, const char *sent, const Node *node);
  bool writeEM(StringBuffer *s, const char *sent, const Node *node);

  bool (Writer::*_write)(StringBuffer *os, const char *sent, const Node *node);

 public:
  explicit Writer() {}
  virtual ~Writer() {}
  bool open(const Param &);
  void close();
  bool writeNode(StringBuffer *os, const char *fmt,
                 const char *sent, const Node* node);
  bool writeNode(StringBuffer *os,
                 const char *sent, const Node *node);
  bool write(StringBuffer *os, const char *sent, const Node *node);
  const char *what() { return what_.str(); }
};
}

