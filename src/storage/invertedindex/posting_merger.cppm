module;

export module posting_merger;

import stl;
import memory_pool;
import file_writer;
import doc_list_encoder;
import inmem_posting_decoder;
import pos_list_encoder;
import posting_list_format;
import index_defines;
import term_meta;
import segment;

namespace infinity {
class PostingDumper;
export class PostingMerger {
public:
    PostingMerger();
    ~PostingMerger();

    void Merge(const Segment &segment);

private:
    PostingFormatOption format_option_;
    SharedPtr<PostingDumper> posting_dumper_;
    df_t df_;
    ttf_t ttf_;
};
} // namespace infinity