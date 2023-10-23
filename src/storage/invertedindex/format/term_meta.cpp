#include "term_meta.h"
#include "storage/compression/vbyte_compressor.h"

namespace infinity {

TermInfoLoader::TermInfoLoader(const PostingFormatOption &option) : option_(option) {}
void TermInfoLoader::Load(ByteSliceReader *byte_slice_reader, TermInfo &term_meta) const {
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
}

void TermInfoLoader::Load(const std::shared_ptr<FileReader> &reader, TermInfo &term_meta) const {
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
}

void TermInfoLoader::Load(uint8_t *&data_cursor, size_t &left_size, TermInfo &term_meta) const {
    auto df = VByteCompressor::DecodeVInt32(data_cursor, (uint32_t &)left_size);
    term_meta.SetDocFreq(df);
    if (option_.HasTermFrequency()) {
        auto ttf = VByteCompressor::DecodeVInt32(data_cursor, (uint32_t &)left_size);
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
}

uint32_t TermInfoDumper::CalculateStoreSize(const TermInfo &term_meta) const {
    uint32_t len = VByteCompressor::GetVInt32Length(term_meta.GetDocFreq());
    if (option_.HasTermFrequency()) {
        len += VByteCompressor::GetVInt32Length(term_meta.GetTotalTermFreq());
    }
    if (option_.HasTermPayload()) {
        len += sizeof(termpayload_t);
    }
    return len;
}

void TermInfoDumper::Dump(const std::shared_ptr<FileWriter> &file, const TermInfo &term_meta) const {
    file->WriteVInt(term_meta.GetDocFreq());
    if (option_.HasTermFrequency()) {
        file->WriteVInt(term_meta.GetTotalTermFreq());
    }
    if (option_.HasTermPayload()) {
        termpayload_t payload = term_meta.GetPayload();
        file->Write((char *)(&payload), sizeof(payload));
    }
}

} // namespace infinity