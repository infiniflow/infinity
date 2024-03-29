module;

#include <cassert>

module multi_posting_decoder;

import stl;
import byte_slice;
import byte_slice_reader;
import memory_pool;
import index_decoder;
import in_doc_pos_iterator;
import in_doc_pos_state;
import in_doc_state_keeper;
import segment_posting;
import index_defines;
import posting_writer;
import term_meta;
import posting_list_format;
import inmem_posting_decoder;
import inmem_position_list_decoder;
import skiplist_reader;
import internal_types;

namespace infinity {

MultiPostingDecoder::MultiPostingDecoder(const PostingFormatOption &format_option, InDocPositionState *state, MemoryPool *pool)
    : format_option_(format_option), session_pool_(pool), in_doc_state_keeper_(state, pool) {
    need_decode_tf_ = format_option_.HasTfList();
    need_decode_doc_payload_ = format_option_.HasDocPayload();
}

MultiPostingDecoder::~MultiPostingDecoder() {
    if (index_decoder_) {
        if (session_pool_) {
            index_decoder_->~IndexDecoder();
            session_pool_->Deallocate((void *)index_decoder_, sizeof(index_decoder_));
        } else
            delete index_decoder_;
    }
}

void MultiPostingDecoder::Init(SharedPtr<Vector<SegmentPosting>> &seg_postings) {
    seg_postings_ = seg_postings;
    segment_count_ = (u32)seg_postings_->size();
    MoveToSegment(0UL);
}

bool MultiPostingDecoder::DecodeDocBuffer(RowID start_row_id, docid_t *doc_buffer, RowID &first_doc_id, RowID &last_doc_id, ttf_t &current_ttf) {
    while (true) {
        if (DecodeDocBufferInOneSegment(start_row_id, doc_buffer, first_doc_id, last_doc_id, current_ttf)) {
            return true;
        }
        if (!MoveToSegment(start_row_id)) {
            return false;
        }
    }
    return false;
}

bool MultiPostingDecoder::DecodeCurrentTFBuffer(tf_t *tf_buffer) {
    if (need_decode_tf_) {
        index_decoder_->DecodeCurrentTFBuffer(tf_buffer);
        need_decode_tf_ = false;
        return true;
    }
    return false;
}

void MultiPostingDecoder::DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer) {
    if (need_decode_doc_payload_) {
        index_decoder_->DecodeCurrentDocPayloadBuffer(doc_payload_buffer);
        need_decode_doc_payload_ = false;
    }
}

bool MultiPostingDecoder::DecodeDocBufferInOneSegment(RowID start_row_id,
                                                      docid_t *doc_buffer,
                                                      RowID &first_row_id,
                                                      RowID &last_row_id,
                                                      ttf_t &current_ttf) {
    RowID next_seg_base_row_id = GetSegmentBaseRowId(segment_cursor_);
    if (next_seg_base_row_id != INVALID_ROWID && start_row_id >= next_seg_base_row_id) {
        // start docid not in current segment
        return false;
    }

    docid_t cur_seg_doc_id = (start_row_id >= base_row_id_) ? docid_t(start_row_id - base_row_id_) : 0;

    docid_t first_doc_id, last_doc_id;
    if (!index_decoder_->DecodeDocBuffer(cur_seg_doc_id, doc_buffer, first_doc_id, last_doc_id, current_ttf)) {
        return false;
    }
    need_decode_tf_ = format_option_.HasTfList();
    need_decode_doc_payload_ = format_option_.HasDocPayload();

    first_row_id = base_row_id_ + first_doc_id;
    last_row_id = base_row_id_ + last_doc_id;
    return true;
}

IndexDecoder *MultiPostingDecoder::CreateIndexDecoder(u32 doc_list_begin_pos) {
    return session_pool_ ? (new ((session_pool_)->Allocate(sizeof(SkipIndexDecoder<SkipListReaderByteSlice>)))
                                SkipIndexDecoder<SkipListReaderByteSlice>(session_pool_,
                                                                          &doc_list_reader_,
                                                                          doc_list_begin_pos,
                                                                          format_option_.GetDocListFormatOption()))
                         : new SkipIndexDecoder<SkipListReaderByteSlice>(session_pool_,
                                                                         &doc_list_reader_,
                                                                         doc_list_begin_pos,
                                                                         format_option_.GetDocListFormatOption());
}

bool MultiPostingDecoder::MoveToSegment(RowID start_row_id) {
    u32 locate_seg_cursor = LocateSegment(segment_cursor_, start_row_id);
    if (locate_seg_cursor >= segment_count_) {
        return false;
    }
    segment_cursor_ = locate_seg_cursor;
    SegmentPosting &cur_segment_posting = (*seg_postings_)[segment_cursor_];
    base_row_id_ = cur_segment_posting.GetBaseRowId();
    const SharedPtr<PostingWriter> &posting_writer = cur_segment_posting.GetInMemPostingWriter();
    if (posting_writer) {
        InMemPostingDecoder *posting_decoder = posting_writer->CreateInMemPostingDecoder(session_pool_);
        if (index_decoder_) {
            if (session_pool_) {
                index_decoder_->~IndexDecoder();
                session_pool_->Deallocate((void *)index_decoder_, sizeof(index_decoder_));
            } else
                delete index_decoder_;
            index_decoder_ = nullptr;
        }
        index_decoder_ = posting_decoder->GetInMemDocListDecoder();
        if (format_option_.HasPositionList()) {
            InMemPositionListDecoder *pos_decoder = posting_decoder->GetInMemPositionListDecoder();
            in_doc_state_keeper_.MoveToSegment(pos_decoder);
            if (in_doc_pos_iterator_) {
                if (session_pool_) {
                    in_doc_pos_iterator_->~InDocPositionIterator();
                    session_pool_->Deallocate((void *)in_doc_pos_iterator_, sizeof(in_doc_pos_iterator_));
                } else
                    delete in_doc_pos_iterator_;
                in_doc_pos_iterator_ = nullptr;
            }
            in_doc_pos_iterator_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(InDocPositionIterator)))
                                                        InDocPositionIterator(format_option_.GetPosListFormatOption()))
                                                 : new InDocPositionIterator(format_option_.GetPosListFormatOption());
        }
        if (posting_decoder) {
            if (session_pool_) {
                posting_decoder->~InMemPostingDecoder();
                session_pool_->Deallocate((void *)posting_decoder, sizeof(posting_decoder));
            } else
                delete posting_decoder;
        }
        ++segment_cursor_;
        return true;
    }
    ByteSliceReader doc_list_reader;
    ByteSliceList *posting_list = cur_segment_posting.GetSliceListPtr().get();
    doc_list_reader.Open(posting_list);
    doc_list_reader_.Open(posting_list);
    const TermMeta &term_meta = cur_segment_posting.GetTermMeta();
    u32 doc_skiplist_size = doc_list_reader.ReadVUInt32();
    u32 doc_list_size = doc_list_reader.ReadVUInt32();

    u32 doc_list_begin_pos = doc_list_reader.Tell() + doc_skiplist_size;
    if (index_decoder_) {
        if (session_pool_) {
            index_decoder_->~IndexDecoder();
            session_pool_->Deallocate((void *)index_decoder_, sizeof(index_decoder_));
        } else
            delete index_decoder_;
        index_decoder_ = nullptr;
    }
    index_decoder_ = CreateIndexDecoder(doc_list_begin_pos);
    u32 doc_skiplist_start = doc_list_reader.Tell();
    u32 doc_skip_list_end = doc_skiplist_start + doc_skiplist_size;
    index_decoder_->InitSkipList(doc_skiplist_start, doc_skip_list_end, posting_list, term_meta.GetDocFreq());
    if (format_option_.HasPositionList()) {
        u32 pos_list_begin = doc_list_reader.Tell() + doc_skiplist_size + doc_list_size;
        in_doc_state_keeper_.MoveToSegment(posting_list, term_meta.GetTotalTermFreq(), pos_list_begin, format_option_);
        if (in_doc_pos_iterator_) {
            if (session_pool_) {
                in_doc_pos_iterator_->~InDocPositionIterator();
                session_pool_->Deallocate((void *)in_doc_pos_iterator_, sizeof(in_doc_pos_iterator_));
            } else
                delete in_doc_pos_iterator_;
            in_doc_pos_iterator_ = nullptr;
        }
        in_doc_pos_iterator_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(InDocPositionIterator)))
                                                    InDocPositionIterator(format_option_.GetPosListFormatOption()))
                                             : new InDocPositionIterator(format_option_.GetPosListFormatOption());
    }

    ++segment_cursor_;
    return true;
}

} // namespace infinity