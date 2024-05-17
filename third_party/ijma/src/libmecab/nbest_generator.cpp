//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: nbest_generator.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <queue>
#include "mecab.h"
#include "nbest_generator.h"

namespace MeCab {

bool NBestGenerator::set(Node *node) {
  freelist_.free();
  for (; node->next; node = node->next) {}   // seek to EOS;
  while (!agenda_.empty()) agenda_.pop();   // make empty
  QueueElement *eos = freelist_.alloc();
  eos->node = node;
  eos->next = 0;
  eos->fx = eos->gx = 0;
  agenda_.push(eos);
  return true;
}

Node* NBestGenerator::next() {
  while (!agenda_.empty()) {
    QueueElement *top = agenda_.top();
    agenda_.pop();
    Node *rnode = top->node;

    if (rnode->stat == MECAB_BOS_NODE) {  // BOS
      for (QueueElement *n = top; n->next; n = n->next) {
        n->node->next = n->next->node;   // change next & prev
        n->next->node->prev = n->node;
        // TODO: rewrite costs;
      }
      score = top->fx;
      return rnode;
    }

    for (Path *path = rnode->lpath; path; path = path->lnext) {
      QueueElement *n = freelist_.alloc();
      n->node = path->lnode;
      n->gx = path->cost + top->gx;
      n->fx = path->lnode->cost + path->cost + top->gx;
      n->next = top;
      agenda_.push(n);
    }
  }
  score = 0;
  return 0;
}

long NBestGenerator::nextScore(){
	return score;
}


}
