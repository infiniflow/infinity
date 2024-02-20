#include "query_node.h"

namespace infinity {

void TermQueryNode::Accept(QueryVisitor &visitor) { visitor.Visit(*this); }

} // namespace infinity