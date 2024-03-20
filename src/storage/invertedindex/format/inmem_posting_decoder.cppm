module;

export module inmem_posting_decoder;
import inmem_doc_list_decoder;
import inmem_position_list_decoder;

namespace infinity {

export class InMemPostingDecoder {
public:
    InMemPostingDecoder() : doc_list_decoder_(nullptr), position_list_decoder_(nullptr) {}
    ~InMemPostingDecoder() = default;

    void SetDocListDecoder(InMemDocListDecoder *doc_list_decoder) { doc_list_decoder_ = doc_list_decoder; }

    InMemDocListDecoder *GetInMemDocListDecoder() const { return doc_list_decoder_; }

    void SetPositionListDecoder(InMemPositionListDecoder *position_list_decoder) { position_list_decoder_ = position_list_decoder; }

    InMemPositionListDecoder *GetInMemPositionListDecoder() const { return position_list_decoder_; }

private:
    InMemDocListDecoder *doc_list_decoder_;
    InMemPositionListDecoder *position_list_decoder_;
};

} // namespace infinity
