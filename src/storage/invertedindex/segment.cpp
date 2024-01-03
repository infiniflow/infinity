module;

import stl;
import index_defines;
import indexer;
module segment;

namespace infinity {

SharedPtr<Indexer> Segment::GetIndexWriter() {
    // TODO
    return MakeShared<Indexer>();
}
} // namespace infinity
