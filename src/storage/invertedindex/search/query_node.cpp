#include "query_node.h"
import query_visitor;

namespace infinity {

void TermQueryNode::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }

void And::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void AndNot::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void Or::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void Wand::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void Phrase::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void PrefixTerm::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void SuffixTerm::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }
void SubstringTerm::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }

} // namespace infinity