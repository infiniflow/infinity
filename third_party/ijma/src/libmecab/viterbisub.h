//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: viterbisub.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include "viterbi.h"

// for MSVC
#undef max

namespace MeCab {

#ifdef _VITERBI_WITH_ALL_PATH
bool Viterbi::connectWithAllPath(size_t pos, Node *rNode) {
#else
  bool Viterbi::connectNormal(size_t pos, Node *rNode) {
#endif
    for (;rNode; rNode = rNode->bnext) {
       long bestCost = 2147483647;

      Node* bestNode = 0;

      for (Node *lNode = end_node_list_[pos]; lNode; lNode = lNode->enext) {
#ifdef _VITERBI_WITH_ALL_PATH
         int  lcost = connector_->cost(lNode, rNode);  // local cost
         long cost  = lNode->cost + lcost;
#else
         long cost  = lNode->cost + connector_->cost(lNode, rNode);
#endif

        if (cost < bestCost) {
          bestNode  = lNode;
          bestCost  = cost;
        }

#ifdef _VITERBI_WITH_ALL_PATH
        Path *path   = path_freelist_->alloc();
        path->cost   = lcost;
        path->rnode  = rNode;
        path->lnode  = lNode;
        path->lnext  = rNode->lpath;
        rNode->lpath = path;
        path->rnext  = lNode->rpath;
        lNode->rpath = path;
#endif
      }

      // overflow check 2003/03/09
      CHECK_FALSE(bestNode) << "too long sentence.";

      rNode->prev = bestNode;
      rNode->next = 0;
      rNode->cost = bestCost;
      size_t x    = rNode->rlength + pos;
      rNode->enext = end_node_list_[x];
      end_node_list_[x] = rNode;
    }

    return true;
  }
}
