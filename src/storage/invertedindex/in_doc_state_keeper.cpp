module;

module in_doc_state_keeper;

import stl;
import memory_pool;
import byte_slice;
import in_doc_pos_state;
import position_list_decoder;
import posting_decoder;
import inmem_posting_decoder;
import inmem_position_list_decoder;
import index_defines;
import posting_list_format;
import third_party;

namespace infinity {
InDocStateKeeper::InDocStateKeeper(InDocPositionState *state, MemoryPool *session_pool) : state_(state), session_pool_(session_pool) {}

InDocStateKeeper::~InDocStateKeeper() {
    for (SizeT i = 0; i < pos_decoders_.size(); ++i) {
        if (session_pool_) {
            pos_decoders_[i]->~PositionListDecoder();
            session_pool_->Deallocate((void *)pos_decoders_[i], sizeof(pos_decoders_[i]));
        } else
            delete pos_decoders_[i];
    }
}

void InDocStateKeeper::MoveToDoc(ttf_t current_ttf) {
    PositionListDecoder *decoder = pos_decoders_.back();
    decoder->SkipTo(current_ttf, state_);
}

void InDocStateKeeper::MoveToSegment(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, const PostingFormatOption &option) {
    PositionListDecoder *decoder = session_pool_
                                       ? (new ((session_pool_)->Allocate(sizeof(PositionListDecoder))) PositionListDecoder(option, session_pool_))
                                       : new PositionListDecoder(option, session_pool_);
    pos_decoders_.push_back(decoder);
    decoder->Init(pos_list, total_tf, pos_list_begin, state_);
    state_->SetPositionListDecoder(decoder);
}

void InDocStateKeeper::MoveToSegment(ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, const PostingFormatOption &option) {
    PositionListDecoder *decoder = session_pool_
                                       ? (new ((session_pool_)->Allocate(sizeof(PositionListDecoder))) PositionListDecoder(option, session_pool_))
                                       : new PositionListDecoder(option, session_pool_);
    pos_decoders_.push_back(decoder);
    decoder->Init(pos_list, total_tf, pos_list_begin, state_);
    state_->SetPositionListDecoder(decoder);
}

void InDocStateKeeper::MoveToSegment(InMemPositionListDecoder *decoder) {
    pos_decoders_.push_back(decoder);
    state_->SetPositionListDecoder(decoder);
}

} // namespace infinity