module;

export module column_index_iterator;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import dict_reader;
import local_file_system;
namespace infinity {

export class ColumnIndexIterator {
public:
    ColumnIndexIterator(const InvertedIndexConfig &index_config, u64 column_id);

    ~ColumnIndexIterator();

    void Init(const Segment &segment);

    bool HasNext();

    PostingDecoder *Next(const String &term);

private:
    void DecodeTermMeta();

    void DecodeDocList();

    void DecodeTfBitmap();

    void DecodePosList();

    InvertedIndexConfig index_config_;
    PostingFormatOption format_option_;
    u64 column_id_;

    SharedPtr<DictionaryIterator> dict_iterator_;
    SharedPtr<FileReader> posting_file_;
    ByteSlice *doc_list_slice_{nullptr};
    ByteSlice *pos_list_slice_{nullptr};

    SharedPtr<ByteSliceReader> doc_list_reader_;
    SharedPtr<ByteSliceReader> pos_list_reader_;
    SharedPtr<Bitmap> tf_bitmap_;

    TermMeta term_meta_;

    SharedPtr<PostingDecoder> posting_decoder_;
    LocalFileSystem fs_;
};

} // namespace infinity