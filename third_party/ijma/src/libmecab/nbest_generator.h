//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: nbest_generator.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_NBEST_GENERATOR_H
#define MECAB_NBEST_GENERATOR_H

#include <queue>
#include "mecab.h"
#include "freelist.h"

namespace MeCab {

class NBestGenerator {
 private:
  struct QueueElement {
    Node *node;
    QueueElement *next;
    long fx;  // f(x) = h(x) + g(x): cost function for A* search
    long gx;  // g(x)
  };

  class QueueElementComp {
   public:
    const bool operator()(QueueElement *q1, QueueElement *q2) {
      return (q1->fx > q2->fx);
    }
  };

  std::priority_queue<QueueElement *, std::vector<QueueElement *>,
                      QueueElementComp> agenda_;
  FreeList <QueueElement> freelist_;
  long score;
 public:
  explicit NBestGenerator(): freelist_(512) {}
  virtual ~NBestGenerator() {}
  bool  set(Node *);
  Node* next();

  /**
   * Invoked after invoking next()
   */
  long nextScore();
};
}

#endif
