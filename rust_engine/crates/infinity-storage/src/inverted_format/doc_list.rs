//! Doc-list format, encoder and in-memory decoder.
//!
//! Port of `src/storage/invertedindex/format/{doc_list_format_option,
//! doc_list_encoder,inmem_doc_list_decoder}*` and
//! `src/unit_test/storage/invertedindex/format/inmem_doc_list_decoder_ut.cpp`.

use super::posting_slice::{
    FieldType, PostingByteSlice, PostingField, PostingFields, ReaderState, SkipListFormat,
    SkipListReader, SkipListWriter, MAX_DOC_PER_RECORD, MAX_DOC_PER_RECORD_BIT_NUM,
};

pub const OF_NONE: u8 = 0;
pub const OF_TERM_PAYLOAD: u8 = 1;
pub const OF_DOC_PAYLOAD: u8 = 2;
pub const OF_POSITION_LIST: u8 = 4;
pub const OF_TERM_FREQUENCY: u8 = 8;
pub const OF_BLOCK_MAX: u8 = 16;
pub const OPTION_FLAG_NONE: u8 = OF_NONE;

/// `DocListFormatOption` (`doc_list_format_option.cppm:19-58`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct DocListFormatOption {
    pub has_tf_list: bool,
    pub has_doc_payload: bool,
    pub has_block_max: bool,
    pub short_list_vbyte_compress: bool,
}

impl DocListFormatOption {
    pub fn new(option_flag: u8) -> Self {
        let has_tf_list = option_flag & OF_TERM_FREQUENCY != 0;
        let has_block_max = option_flag & OF_BLOCK_MAX != 0;
        assert!(
            !has_block_max || has_tf_list,
            "Invalid option flag: block_max must be used with term frequency list."
        );
        Self {
            has_tf_list,
            has_doc_payload: option_flag & OF_DOC_PAYLOAD != 0,
            has_block_max,
            short_list_vbyte_compress: false,
        }
    }

    pub fn has_tf(&self) -> bool {
        self.has_tf_list
    }

    pub fn set_short_list_vbyte_compress(&mut self, flag: bool) {
        self.short_list_vbyte_compress = flag;
    }

    /// `DocListFormat::Init` (`doc_list_format_option.cppm:141-175`).
    pub fn doc_list_fields(&self) -> PostingFields {
        let mut fields = PostingFields::new();
        fields.add(PostingField {
            location: 0,
            offset: 0,
            ty: FieldType::U32,
        });
        let mut offset = 4;
        if self.has_tf_list {
            fields.add(PostingField {
                location: fields.len(),
                offset,
                ty: FieldType::U32,
            });
            offset += 4;
        }
        if self.has_doc_payload {
            fields.add(PostingField {
                location: fields.len(),
                offset,
                ty: FieldType::U16,
            });
        }
        fields
    }

    /// `DocSkipListFormat::Init`.
    pub fn skip_list_format(&self) -> SkipListFormat {
        SkipListFormat::for_doc(self.has_tf_list, self.has_block_max)
    }
}

/// `DocListEncoder` (`doc_list_encoder.cppm:13-75`).
#[derive(Clone, Debug)]
pub struct DocListEncoder {
    option: DocListFormatOption,
    buffer: PostingByteSlice,
    skiplist: SkipListWriter,
    last_doc_id: u32,
    last_doc_payload: u16,
    current_tf: u32,
    total_tf: u32,
    block_max_tf: u32,
    block_max_percentage: f32,
    df: u32,
}

impl DocListEncoder {
    pub fn new(option: DocListFormatOption) -> Self {
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.doc_list_fields());
        let mut skiplist = SkipListWriter::new();
        skiplist.init(&option.skip_list_format());
        Self {
            option,
            buffer,
            skiplist,
            last_doc_id: 0,
            last_doc_payload: 0,
            current_tf: 0,
            total_tf: 0,
            block_max_tf: 0,
            block_max_percentage: 0.0,
            df: 0,
        }
    }

    pub fn get_current_tf(&self) -> u32 {
        self.current_tf
    }

    pub fn get_total_tf(&self) -> u32 {
        self.total_tf
    }

    pub fn get_df(&self) -> u32 {
        self.df
    }

    pub fn get_last_doc_id(&self) -> u32 {
        self.last_doc_id
    }

    /// `DocListEncoder::AddPosition`.
    pub fn add_position(&mut self) {
        self.current_tf += 1;
        self.total_tf += 1;
    }

    /// `DocListEncoder::EndDocument`.
    pub fn end_document(&mut self, doc_id: u32, doc_len: u32, doc_payload: u16) {
        self.add_document(doc_id, doc_payload, self.current_tf, doc_len);
        self.df += 1;
        self.current_tf = 0;
    }

    fn add_document(&mut self, doc_id: u32, doc_payload: u16, tf: u32, doc_len: u32) {
        self.buffer.push_back_u32(0, doc_id.wrapping_sub(self.last_doc_id));
        let mut n = 1;
        if self.option.has_tf_list {
            self.buffer.push_back_u32(n, tf);
            n += 1;
        }
        if self.option.has_doc_payload {
            self.buffer.push_back_u16(n, doc_payload);
        }
        self.buffer.end_push_back();
        self.last_doc_id = doc_id;
        self.last_doc_payload = doc_payload;
        self.block_max_tf = self.block_max_tf.max(tf);
        assert!(tf > 0 && tf <= doc_len);
        self.block_max_percentage = self
            .block_max_percentage
            .max(tf as f32 / doc_len as f32);
        if self.buffer.need_flush(MAX_DOC_PER_RECORD) {
            self.flush_doc_list_buffer();
        }
    }

    fn add_skip_list_item(&mut self, item_size: u32) {
        if self.option.has_block_max {
            assert!(self.block_max_percentage > 0.0 && self.block_max_percentage <= 1.0);
            let max_percentage_field =
                (self.block_max_percentage * f32::from(u16::MAX)).ceil() as u32;
            assert!(max_percentage_field <= u32::from(u16::MAX));
            self.skiplist.add_item_4(
                self.last_doc_id,
                self.total_tf,
                self.block_max_tf,
                max_percentage_field as u16,
                item_size,
            );
        } else if self.option.has_tf_list {
            self.skiplist
                .add_item_3(self.last_doc_id, self.total_tf, item_size);
        } else {
            self.skiplist.add_item_2(self.last_doc_id, item_size);
        }
    }

    fn flush_doc_list_buffer(&mut self) {
        let flush_size = self.buffer.flush();
        if flush_size > 0 {
            self.add_skip_list_item(flush_size as u32);
        }
        self.block_max_tf = 0;
        self.block_max_percentage = 0.0;
    }

    /// `DocListEncoder::Flush`.
    pub fn flush(&mut self) {
        self.flush_doc_list_buffer();
        self.skiplist.flush();
    }

    pub fn doc_list_buffer(&mut self) -> &mut PostingByteSlice {
        &mut self.buffer
    }

    /// `DocListEncoder::Dump`.
    pub fn dump(&mut self, spill: bool) -> Vec<u8> {
        let mut out = Vec::new();
        if spill {
            crate::inverted_format::write_vint(&mut out, u64::from(self.last_doc_id));
            crate::inverted_format::write_vint(&mut out, u64::from(self.last_doc_payload));
            crate::inverted_format::write_vint(&mut out, u64::from(self.current_tf));
            crate::inverted_format::write_vint(&mut out, u64::from(self.total_tf));
            crate::inverted_format::write_vint(&mut out, u64::from(self.df));
            crate::inverted_format::write_vint(&mut out, u64::from(self.block_max_tf));
            out.extend_from_slice(&self.block_max_percentage.to_bits().to_le_bytes());
        } else {
            self.flush();
            crate::inverted_format::write_vint(&mut out, self.skiplist.bytes().len() as u64);
            crate::inverted_format::write_vint(&mut out, self.buffer.bytes.len() as u64);
        }
        out.extend_from_slice(self.skiplist.bytes());
        out.extend_from_slice(&self.buffer.dump(spill));
        out
    }

    /// `DocListEncoder::GetInMemDocListDecoder`.
    pub fn in_mem_decoder(&self) -> InMemDocListDecoder {
        let format = self.option.skip_list_format();
        let mut skiplist_reader = SkipListReader::new(&format);
        skiplist_reader.load_posting_byte_slice(self.skiplist.slice());
        let mut decoder = InMemDocListDecoder::new(self.option);
        decoder.init(self.df, Some(skiplist_reader), self.buffer.snapshot());
        decoder
    }
}

/// The result of `DecodeSkipList`; `last_doc_id` is written even when the
/// requested start is past the decoded record (C++ writes the out-parameter
/// before returning `false`).
#[derive(Clone, Copy, Debug)]
pub struct SkipListDecode {
    pub success: bool,
    pub prev_last_doc_id: u32,
    pub last_doc_id: u32,
    pub current_ttf: i64,
}

/// `InMemDocListDecoder` (`inmem_doc_list_decoder.cppm:13-54`).
pub struct InMemDocListDecoder {
    option: DocListFormatOption,
    skiplist_reader: Option<SkipListReader>,
    doc_list_buffer: Option<PostingByteSlice>,
    doc_list_reader: ReaderState,
    df: u32,
    finish_decoded: bool,
    finish_copy_prepared_doc_buffer: bool,
    doc_buffer_to_copy: Vec<u32>,
    decode_count: usize,
    offset: u32,
    record_len: u32,
    last_doc_id_in_prev_record: u32,
    last_doc_id: u32,
    current_ttf: u32,
    skipped_item_count: u32,
}

impl InMemDocListDecoder {
    pub fn new(option: DocListFormatOption) -> Self {
        Self {
            option,
            skiplist_reader: None,
            doc_list_buffer: None,
            doc_list_reader: ReaderState::default(),
            df: 0,
            finish_decoded: false,
            finish_copy_prepared_doc_buffer: false,
            doc_buffer_to_copy: Vec::new(),
            decode_count: 0,
            offset: 0,
            record_len: 0,
            last_doc_id_in_prev_record: 0,
            last_doc_id: 0,
            current_ttf: 0,
            skipped_item_count: 0,
        }
    }

    /// `InMemDocListDecoder::Init`.
    pub fn init(
        &mut self,
        df: u32,
        skiplist_reader: Option<SkipListReader>,
        doc_list_buffer: PostingByteSlice,
    ) {
        self.df = df;
        self.skiplist_reader = skiplist_reader;
        self.doc_list_reader.open();
        self.doc_list_buffer = Some(doc_list_buffer);
    }

    fn decode_without_skip_list(
        &mut self,
        last_doc_id_in_prev_record: u32,
        offset: u32,
        start_doc_id: u32,
    ) -> (bool, u32) {
        self.finish_decoded = false;
        let Some(buffer) = &self.doc_list_buffer else {
            return (false, self.last_doc_id);
        };
        self.doc_list_reader
            .seek(buffer, offset as usize);
        let Some(values) = self
            .doc_list_reader
            .decode_u32(buffer, MAX_DOC_PER_RECORD)
        else {
            return (false, self.last_doc_id);
        };
        self.decode_count = values.len();
        self.doc_buffer_to_copy = values;
        self.finish_decoded = true;
        let mut last_doc_id = last_doc_id_in_prev_record;
        for value in &self.doc_buffer_to_copy {
            last_doc_id = last_doc_id.wrapping_add(*value);
        }
        self.last_doc_id = last_doc_id;
        if start_doc_id > last_doc_id {
            return (false, last_doc_id);
        }
        (true, last_doc_id)
    }

    /// `InMemDocListDecoder::DecodeSkipList`.
    pub fn decode_skip_list(&mut self, start_doc_id: u32) -> SkipListDecode {
        enum Outcome {
            Matched(u32, u32, u32, u32, i32),
            Exhausted(u32, u32, u32, i32),
        }
        let outcome = match self.skiplist_reader.as_mut() {
            None => {
                let (success, last_doc_id) =
                    self.decode_without_skip_list(self.last_doc_id_in_prev_record, 0, start_doc_id);
                return SkipListDecode {
                    success,
                    prev_last_doc_id: 0,
                    last_doc_id,
                    current_ttf: 0,
                };
            }
            Some(reader) => match reader.skip_to(start_doc_id) {
                Some(result) => Outcome::Matched(
                    result.doc_id,
                    result.prev_doc_id,
                    result.offset,
                    result.delta,
                    reader.get_skipped_item_count(),
                ),
                None => Outcome::Exhausted(
                    reader.get_last_key_in_buffer(),
                    reader.get_last_value_in_buffer(),
                    reader.get_current_ttf(),
                    reader.get_skipped_item_count(),
                ),
            },
        };
        match outcome {
            Outcome::Matched(doc_id, prev_doc_id, offset, delta, skipped) => {
                self.last_doc_id_in_prev_record = prev_doc_id;
                self.last_doc_id = doc_id;
                self.offset = offset;
                self.record_len = delta;
                self.skipped_item_count = skipped as u32;
                self.current_ttf = self
                    .skiplist_reader
                    .as_ref()
                    .map_or(0, SkipListReader::get_prev_ttf);
                SkipListDecode {
                    success: true,
                    prev_last_doc_id: prev_doc_id,
                    last_doc_id: doc_id,
                    current_ttf: i64::from(self.current_ttf),
                }
            }
            Outcome::Exhausted(last_key, offset, ttf, skipped) => {
                self.last_doc_id_in_prev_record = last_key;
                self.offset = offset;
                self.current_ttf = ttf;
                self.skipped_item_count = skipped as u32;
                let (success, last_doc_id) =
                    self.decode_without_skip_list(last_key, offset, start_doc_id);
                SkipListDecode {
                    success,
                    prev_last_doc_id: last_key,
                    last_doc_id,
                    current_ttf: i64::from(self.current_ttf),
                }
            }
        }
    }

    /// `InMemDocListDecoder::DecodeCurrentDocIDBuffer`.
    pub fn decode_current_doc_id_buffer(&mut self, doc_buffer: &mut Vec<u32>) -> bool {
        if self.finish_decoded {
            doc_buffer.clear();
            doc_buffer.extend_from_slice(&self.doc_buffer_to_copy[..self.decode_count]);
            self.finish_copy_prepared_doc_buffer = true;
            return true;
        }
        let Some(buffer) = &self.doc_list_buffer else {
            return false;
        };
        self.doc_list_reader
            .seek(buffer, self.offset as usize);
        match self
            .doc_list_reader
            .decode_u32(buffer, MAX_DOC_PER_RECORD)
        {
            Some(values) => {
                doc_buffer.clear();
                doc_buffer.extend_from_slice(&values);
                true
            }
            None => false,
        }
    }

    /// `InMemDocListDecoder::DecodeCurrentTFBuffer`.
    pub fn decode_current_tf_buffer(&mut self, tf_buffer: &mut Vec<u32>) -> bool {
        let Some(buffer) = &self.doc_list_buffer else {
            return false;
        };
        match self
            .doc_list_reader
            .decode_u32(buffer, MAX_DOC_PER_RECORD)
        {
            Some(values) => {
                tf_buffer.clear();
                tf_buffer.extend_from_slice(&values);
                true
            }
            None => false,
        }
    }

    /// `InMemDocListDecoder::DecodeCurrentDocPayloadBuffer`.
    pub fn decode_current_doc_payload_buffer(&mut self, payload: &mut Vec<u16>) -> bool {
        let Some(buffer) = &self.doc_list_buffer else {
            return false;
        };
        match self
            .doc_list_reader
            .decode_u16(buffer, MAX_DOC_PER_RECORD)
        {
            Some(values) => {
                payload.clear();
                payload.extend_from_slice(&values);
                true
            }
            None => false,
        }
    }

    /// `InMemDocListDecoder::GetSeekedDocCount`.
    pub fn get_seeked_doc_count(&self) -> u32 {
        self.skipped_item_count << MAX_DOC_PER_RECORD_BIT_NUM
    }

    /// `InMemDocListDecoder::GetBlockMaxInfo`.
    pub fn get_block_max_info(&self) -> (u32, u16) {
        if !self.finish_decoded {
            if let Some(reader) = &self.skiplist_reader {
                return reader.get_block_max_info();
            }
        }
        (u32::MAX, u16::MAX)
    }

    pub fn option(&self) -> DocListFormatOption {
        self.option
    }

    pub fn df(&self) -> u32 {
        self.df
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::inverted_format::posting_slice::SKIP_LIST_BUFFER_SIZE;

    const DEFAULT_DOCUMENT_LENGTH: u32 = 10;

    fn test_decode_with_option_flag(
        flag: u8,
        doc_num: usize,
        docids: &[u32],
        tfs: Option<&[u32]>,
        doc_payloads: Option<&[u16]>,
    ) {
        let option = DocListFormatOption::new(flag);
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.doc_list_fields());

        let mut prev_doc_id = 0u32;
        for (i, &docid) in docids.iter().enumerate().take(doc_num) {
            let mut row = 0usize;
            buffer.push_back_u32(row, docid.wrapping_sub(prev_doc_id));
            row += 1;
            prev_doc_id = docid;
            if option.has_tf_list {
                buffer.push_back_u32(row, tfs.unwrap()[i]);
                row += 1;
            }
            if option.has_doc_payload {
                buffer.push_back_u16(row, doc_payloads.unwrap()[i]);
            }
            buffer.end_push_back();
        }

        let mut decoder = InMemDocListDecoder::new(option);
        decoder.init(2, None, buffer);

        let decode = decoder.decode_skip_list(0);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id.wrapping_add(doc_buffer[0]);
        assert_eq!(docids[0], first_doc_id);
        assert_eq!(docids[doc_num - 1], decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(docids[0], doc_buffer[0]);
        for i in 1..doc_num {
            assert_eq!(docids[i] - docids[i - 1], doc_buffer[i]);
        }
        if option.has_tf_list {
            let mut tf_buffer = Vec::new();
            assert!(decoder.decode_current_tf_buffer(&mut tf_buffer));
            assert_eq!(tf_buffer, tfs.unwrap());
        }
        if option.has_doc_payload {
            let mut payload_buffer = Vec::new();
            assert!(decoder.decode_current_doc_payload_buffer(&mut payload_buffer));
            assert_eq!(payload_buffer, doc_payloads.unwrap());
        }
    }

    fn create_decoder(doc_count: u32, need_flush: bool, need_tf: bool) -> InMemDocListDecoder {
        // C++ `NO_TERM_FREQUENCY` = term payload | doc payload.
        let mut op_flag = OF_DOC_PAYLOAD;
        if need_tf {
            op_flag |= OF_TERM_FREQUENCY;
        }
        let option = DocListFormatOption::new(op_flag);
        let mut encoder = DocListEncoder::new(option);
        for i in 0..doc_count {
            encoder.add_position();
            encoder.add_position();
            encoder.end_document(i, DEFAULT_DOCUMENT_LENGTH, (i * 2) as u16);
        }
        if need_flush {
            encoder.flush();
        }
        encoder.in_mem_decoder()
    }

    fn test_decode(doc_count: u32, need_flush: bool, need_tf: bool) {
        let mut decoder = create_decoder(doc_count, need_flush, need_tf);

        let mut doc_buffer: Vec<u32> = vec![0; doc_count as usize];
        let mut doc_payload: Vec<u16> = vec![0; doc_count as usize];
        let mut tf_buffer: Vec<u32> = vec![0; doc_count as usize];

        let mut last_doc_id = 0u32;
        let mut i = 0u32;
        while i < doc_count / MAX_DOC_PER_RECORD as u32 {
            let decode = decoder.decode_skip_list(i * MAX_DOC_PER_RECORD as u32);
            assert!(decode.success);
            let prev_last_doc_id = decode.prev_last_doc_id;
            last_doc_id = decode.last_doc_id;
            let base = (i * MAX_DOC_PER_RECORD as u32) as usize;
            let mut docs = Vec::new();
            assert!(decoder.decode_current_doc_id_buffer(&mut docs));
            doc_buffer[base..base + docs.len()].copy_from_slice(&docs);
            let first_doc_id = prev_last_doc_id.wrapping_add(doc_buffer[base]);
            assert_eq!(i * MAX_DOC_PER_RECORD as u32, first_doc_id);
            assert_eq!(
                (i + 1) * MAX_DOC_PER_RECORD as u32 - 1,
                last_doc_id
            );
            assert_eq!(
                i * MAX_DOC_PER_RECORD as u32,
                decoder.get_seeked_doc_count()
            );
            let expect_ttf = if need_tf {
                i64::from(2 * i * MAX_DOC_PER_RECORD as u32)
            } else {
                0
            };
            assert_eq!(expect_ttf, decode.current_ttf);
            if need_tf {
                let mut tfs = Vec::new();
                assert!(decoder.decode_current_tf_buffer(&mut tfs));
                tf_buffer[base..base + tfs.len()].copy_from_slice(&tfs);
            }
            let mut payload = Vec::new();
            assert!(decoder.decode_current_doc_payload_buffer(&mut payload));
            doc_payload[base..base + payload.len()].copy_from_slice(&payload);
            i += 1;
        }
        if !doc_count.is_multiple_of(MAX_DOC_PER_RECORD as u32) {
            let decode = decoder.decode_skip_list(i * MAX_DOC_PER_RECORD as u32);
            assert!(decode.success);
            let prev_last_doc_id = decode.prev_last_doc_id;
            last_doc_id = decode.last_doc_id;
            let base = (i * MAX_DOC_PER_RECORD as u32) as usize;
            let mut docs = Vec::new();
            assert!(decoder.decode_current_doc_id_buffer(&mut docs));
            doc_buffer[base..base + docs.len()].copy_from_slice(&docs);
            let first_doc_id = prev_last_doc_id.wrapping_add(doc_buffer[base]);
            assert_eq!(i * MAX_DOC_PER_RECORD as u32, decoder.get_seeked_doc_count());
            assert_eq!(i * MAX_DOC_PER_RECORD as u32, first_doc_id);
            assert_eq!(doc_count - 1, last_doc_id);
            let expect_ttf = if need_tf {
                i64::from(2 * i * MAX_DOC_PER_RECORD as u32)
            } else {
                0
            };
            assert_eq!(expect_ttf, decode.current_ttf);
            if need_tf {
                let mut tfs = Vec::new();
                assert!(decoder.decode_current_tf_buffer(&mut tfs));
                tf_buffer[base..base + tfs.len()].copy_from_slice(&tfs);
            }
            let mut payload = Vec::new();
            assert!(decoder.decode_current_doc_payload_buffer(&mut payload));
            doc_payload[base..base + payload.len()].copy_from_slice(&payload);
        }

        let mut pre_doc_id = 0u32;
        for i in 0..doc_count {
            assert_eq!(
                i.wrapping_sub(pre_doc_id),
                doc_buffer[i as usize],
                "n={doc_count} i={i} doc"
            );
            pre_doc_id = i;
            assert_eq!(
                (i * 2) as u16,
                doc_payload[i as usize],
                "n={doc_count} i={i} payload"
            );
            if need_tf {
                assert_eq!(2, tf_buffer[i as usize]);
            }
        }
        assert!(!decoder.decode_skip_list(last_doc_id.wrapping_add(1)).success);
    }

    #[test]
    fn inmem_doc_list_decoder_test1() {
        let option = DocListFormatOption::new(OPTION_FLAG_NONE);
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.doc_list_fields());

        let doc1 = 1u32;
        let doc2 = 10u32;
        buffer.push_back_u32(0, doc1);
        buffer.end_push_back();
        buffer.push_back_u32(0, doc2 - doc1);
        buffer.end_push_back();

        let mut decoder = InMemDocListDecoder::new(option);
        decoder.init(2, None, buffer);

        let decode = decoder.decode_skip_list(0);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(doc1, first_doc_id);
        assert_eq!(doc2, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(doc1, doc_buffer[0]);
        assert_eq!(doc2 - doc1, doc_buffer[1]);
        assert!(decoder.decode_skip_list(doc2).success);
    }

    #[test]
    fn inmem_doc_list_decoder_test2() {
        let option = DocListFormatOption::new(OPTION_FLAG_NONE);
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.doc_list_fields());
        for i in 0..128u32 {
            buffer.push_back_u32(0, i);
            buffer.end_push_back();
        }
        buffer.flush();

        let mut decoder = InMemDocListDecoder::new(option);
        decoder.init(2, None, buffer);

        let decode = decoder.decode_skip_list(0);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(0, first_doc_id);
        assert_eq!(8128, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(0, doc_buffer[0]);
        assert_eq!(1, doc_buffer[1]);
        assert_eq!(2, doc_buffer[2]);

        let mut last_doc_id = decode.last_doc_id;
        let failed = decoder.decode_skip_list(last_doc_id);
        assert!(!failed.success);
        last_doc_id = failed.last_doc_id;

        decoder.doc_list_buffer_mut_for_test().push_back_u32(0, 128);
        decoder.doc_list_buffer_mut_for_test().end_push_back();

        let failed = decoder.decode_skip_list(last_doc_id);
        assert!(!failed.success);
        last_doc_id = failed.last_doc_id;

        decoder.doc_list_buffer_mut_for_test().flush();

        assert!(decoder.decode_skip_list(last_doc_id).success);
    }

    // Helper used only by test2 to reach the encoder-owned buffer. Test2 builds
    // the buffer directly, so this reaches the decoder's snapshot instead.
    impl InMemDocListDecoder {
        fn doc_list_buffer_mut_for_test(&mut self) -> &mut PostingByteSlice {
            self.doc_list_buffer.as_mut().unwrap()
        }
    }

    fn make_setup_encoder() -> DocListEncoder {
        let option = DocListFormatOption::new(OPTION_FLAG_NONE);
        let mut encoder = DocListEncoder::new(option);
        for i in 0..128 {
            encoder.add_position();
            encoder.end_document(i, DEFAULT_DOCUMENT_LENGTH, 0);
        }
        for i in 128..256 {
            encoder.add_position();
            encoder.end_document(i * 2, DEFAULT_DOCUMENT_LENGTH, 0);
        }
        encoder.add_position();
        encoder.end_document(1000, DEFAULT_DOCUMENT_LENGTH, 0);
        encoder
    }

    #[test]
    fn inmem_doc_list_decoder_test3() {
        let encoder = make_setup_encoder();
        let mut decoder = encoder.in_mem_decoder();

        let decode = decoder.decode_skip_list(256);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(256, first_doc_id);
        assert_eq!(510, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(129, doc_buffer[0]);
        assert_eq!(2, doc_buffer[1]);

        let decode = decoder.decode_skip_list(1000);
        assert!(decode.success);
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(1000, first_doc_id);
        assert_eq!(1000, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(490, doc_buffer[0]);

        assert!(decoder.decode_skip_list(decode.last_doc_id).success);
    }

    #[test]
    fn inmem_doc_list_decoder_test4() {
        let mut encoder = make_setup_encoder();
        encoder.add_position();
        encoder.end_document(1001, DEFAULT_DOCUMENT_LENGTH, 0);
        encoder.add_position();
        encoder.end_document(1002, DEFAULT_DOCUMENT_LENGTH, 0);

        encoder.doc_list_buffer().flush();

        let mut decoder = encoder.in_mem_decoder();
        let decode = decoder.decode_skip_list(1000);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(1000, first_doc_id);
        assert_eq!(1002, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(490, doc_buffer[0]);
        assert_eq!(1, doc_buffer[1]);
        assert_eq!(1, doc_buffer[2]);

        assert!(!decoder.decode_skip_list(decode.last_doc_id).success);
    }

    #[test]
    fn inmem_doc_list_decoder_test5() {
        let mut encoder = make_setup_encoder();
        // The C++ test pushes straight into the encoder's `PostingByteSlice`
        // without adding skip-list items.
        for i in 0..300u32 {
            encoder.doc_list_buffer().push_back_u32(0, i);
            encoder.doc_list_buffer().end_push_back();
            if encoder.doc_list_buffer().need_flush(MAX_DOC_PER_RECORD) {
                encoder.doc_list_buffer().flush();
            }
        }
        let mut decoder = encoder.in_mem_decoder();

        let decode = decoder.decode_skip_list(1000);
        assert!(decode.success);
        let mut doc_buffer = Vec::new();
        assert!(decoder.decode_current_doc_id_buffer(&mut doc_buffer));
        let first_doc_id = decode.prev_last_doc_id + doc_buffer[0];
        assert_eq!(1000, first_doc_id);
        assert_eq!(9001, decode.last_doc_id);
        assert_eq!(0, decode.current_ttf);
        assert_eq!(490, doc_buffer[0]);
        assert_eq!(0, doc_buffer[1]);
        assert_eq!(1, doc_buffer[2]);
        assert_eq!(2, doc_buffer[3]);

        assert!(decoder.decode_skip_list(decode.last_doc_id).success);
    }

    #[test]
    fn inmem_doc_list_decoder_test6() {
        test_decode_with_option_flag(OF_TERM_FREQUENCY, 2, &[1, 10], Some(&[2, 4]), None);
    }

    #[test]
    fn inmem_doc_list_decoder_test7() {
        test_decode_with_option_flag(OF_DOC_PAYLOAD, 2, &[1, 10], None, Some(&[2, 4]));
    }

    #[test]
    fn inmem_doc_list_decoder_test8() {
        test_decode_with_option_flag(
            OF_TERM_FREQUENCY | OF_DOC_PAYLOAD,
            2,
            &[1, 10],
            Some(&[2, 4]),
            Some(&[2, 4]),
        );
    }

    const MAX_UNCOMPRESSED_DOC_LIST_SIZE: u32 = 5;

    fn decode_set() -> Vec<(u32, bool, bool)> {
        let mut set = Vec::new();
        for need_flush in [false, true] {
            for need_tf in [false, true] {
                for n in [
                    1,
                    MAX_UNCOMPRESSED_DOC_LIST_SIZE + 1,
                    MAX_DOC_PER_RECORD as u32,
                    MAX_DOC_PER_RECORD as u32 + 10,
                    MAX_DOC_PER_RECORD as u32 * 5 + 1,
                    MAX_DOC_PER_RECORD as u32 * 10 + 1,
                    MAX_DOC_PER_RECORD as u32 * SKIP_LIST_BUFFER_SIZE as u32 + 1,
                    MAX_DOC_PER_RECORD as u32 * SKIP_LIST_BUFFER_SIZE as u32 * 2 + 1,
                ] {
                    set.push((n, need_flush, need_tf));
                }
            }
        }
        set
    }

    #[test]
    fn inmem_doc_list_decoder_test9() {
        for &(n, need_flush, need_tf) in &decode_set() {
            if !need_flush && !need_tf {
                test_decode(n, false, false);
            }
        }
    }

    #[test]
    fn inmem_doc_list_decoder_test10() {
        for &(n, need_flush, need_tf) in &decode_set() {
            if need_flush && !need_tf {
                test_decode(n, true, false);
            }
        }
    }

    #[test]
    fn inmem_doc_list_decoder_test11() {
        for &(n, need_flush, need_tf) in &decode_set() {
            if !need_flush && need_tf {
                test_decode(n, false, true);
            }
        }
    }

    #[test]
    fn inmem_doc_list_decoder_test12() {
        for &(n, need_flush, need_tf) in &decode_set() {
            if need_flush && need_tf {
                test_decode(n, true, true);
            }
        }
    }
}
