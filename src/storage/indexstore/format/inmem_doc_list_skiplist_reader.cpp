#include "inmem_doc_list_skiplist_reader.h"

namespace infinity {

void
InMemDocListSkipListReader::Load(BufferedByteSlice* posting_buffer) {
    InitMember();
    BufferedByteSlice* skiplist_buffer = new BufferedByteSlice(session_pool_, session_pool_);
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

}
