module;

import stl;
import byte_slice;
import byte_slice_reader;
import file_reader;
import file_writer;
import posting_list_format;
import vbyte_compressor;
import index_defines;
module term_meta;

namespace infinity {

TermMetaLoader::TermMetaLoader(const PostingFormatOption &option) : option_(option) {}
void TermMetaLoader::Load(ByteSliceReader *byte_slice_reader, TermMeta &term_meta) const {
    df_t df = (df_t)byte_slice_reader->ReadVUInt32();
    term_meta.SetDocFreq(df);
    if (option_.HasTermFrequency()) {
        term_meta.SetTotalTermFreq((tf_t)byte_slice_reader->ReadVUInt32());
    } else {
        term_meta.SetTotalTermFreq(df);
    }
    if (option_.HasTermPayload()) {
        termpayload_t payload;
        byte_slice_reader->Read((void *)(&payload), sizeof(payload));
        term_meta.SetPayload(payload);
    } else {
        term_meta.SetPayload(0);
    }
    term_meta.doc_start_ = byte_slice_reader->ReadVUInt64();
    term_meta.pos_start_ = byte_slice_reader->ReadVUInt64();
    term_meta.pos_end_ = byte_slice_reader->ReadVUInt64();
}

void TermMetaLoader::Load(const SharedPtr<FileReader> &reader, TermMeta &term_meta) const {
    df_t df = (df_t)reader->ReadVInt();
    term_meta.SetDocFreq(df);
    if (option_.HasTermFrequency()) {
        term_meta.SetTotalTermFreq((tf_t)reader->ReadVInt());
    } else {
        term_meta.SetTotalTermFreq(df);
    }
    if (option_.HasTermPayload()) {
        termpayload_t payload;
        reader->Read((char *)(&payload), sizeof(payload));
        term_meta.SetPayload(payload);
    } else {
        term_meta.SetPayload(0);
    }
    term_meta.doc_start_ = reader->ReadVLong();
    term_meta.pos_start_ = reader->ReadVLong();
    term_meta.pos_end_ = reader->ReadVLong();
}

void TermMetaLoader::Load(u8 *&data_cursor, SizeT &left_size, TermMeta &term_meta) const {
    auto df = VByteCompressor::DecodeVInt32(data_cursor, (u32 &)left_size);
    term_meta.SetDocFreq(df);
    if (option_.HasTermFrequency()) {
        auto ttf = VByteCompressor::DecodeVInt32(data_cursor, (u32 &)left_size);
        term_meta.SetTotalTermFreq(ttf);
    } else {
        term_meta.SetTotalTermFreq(df);
    }
    if (option_.HasTermPayload()) {
        termpayload_t payload = *(termpayload_t *)data_cursor;
        data_cursor += sizeof(termpayload_t);
        left_size -= sizeof(termpayload_t);

        term_meta.SetPayload(payload);
    } else {
        term_meta.SetPayload(0);
    }
    term_meta.doc_start_ = VByteCompressor::DecodeVInt64(data_cursor, (u32 &)left_size);
    term_meta.pos_start_ = VByteCompressor::DecodeVInt64(data_cursor, (u32 &)left_size);
    term_meta.pos_end_ = VByteCompressor::DecodeVInt64(data_cursor, (u32 &)left_size);
}

u32 TermMetaDumper::CalculateStoreSize(const TermMeta &term_meta) const {
    u32 len = VByteCompressor::GetVInt32Length(term_meta.GetDocFreq());
    if (option_.HasTermFrequency()) {
        len += VByteCompressor::GetVInt32Length(term_meta.GetTotalTermFreq());
    }
    if (option_.HasTermPayload()) {
        len += sizeof(termpayload_t);
    }
    len += VByteCompressor::GetVInt64Length(term_meta.doc_start_);
    len += VByteCompressor::GetVInt64Length(term_meta.pos_start_);
    len += VByteCompressor::GetVInt64Length(term_meta.pos_end_);
    return len;
}

void TermMetaDumper::Dump(const SharedPtr<FileWriter> &file, const TermMeta &term_meta) const {
    file->WriteVInt(term_meta.GetDocFreq());
    if (option_.HasTermFrequency()) {
        file->WriteVInt(term_meta.GetTotalTermFreq());
    }
    if (option_.HasTermPayload()) {
        termpayload_t payload = term_meta.GetPayload();
        file->Write((char *)(&payload), sizeof(payload));
    }
    file->WriteVLong(term_meta.doc_start_);
    file->WriteVLong(term_meta.pos_start_);
    file->WriteVLong(term_meta.pos_end_);
}

} // namespace infinity