//! Position-list format, encoder and in-memory decoder.
//!
//! Port of `src/storage/invertedindex/format/{position_list_format_option,
//! position_list_encoder,position_list_decoder,inmem_position_list_decoder}*`
//! and `src/unit_test/storage/invertedindex/format/{position_list_encoder_ut,
//! position_list_decoder_ut,inmem_position_list_decoder_ut}.cpp`.

use super::posting_slice::{
    FieldType, PostingByteSlice, PostingField, PostingFields, ReaderState, SkipListFormat,
    SkipListReader, SkipListWriter,
};
use crate::inverted_format::write_vint;

/// `MAX_POS_PER_RECORD` (`index_defines.cppm:46`).
pub const MAX_POS_PER_RECORD: usize = 128;
/// `MAX_UNCOMPRESSED_POS_LIST_SIZE` (`index_defines.cppm:48`).
pub const MAX_UNCOMPRESSED_POS_LIST_SIZE: u32 = 5;

pub const OF_POSITION_LIST: u8 = 4;
pub const OF_TERM_FREQUENCY: u8 = 8;

/// `PositionListFormatOption` (`position_list_format_option.cppm:14-28`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct PositionListFormatOption {
    pub has_position_list: bool,
}

impl PositionListFormatOption {
    pub fn new(option_flag: u8) -> Self {
        Self {
            has_position_list: option_flag & OF_POSITION_LIST != 0,
        }
    }

    /// `PositionListFormat::Init`: one delta-coded `u32` position row.
    pub fn fields(&self) -> PostingFields {
        let mut fields = PostingFields::new();
        fields.add(PostingField {
            location: 0,
            offset: 0,
            ty: FieldType::U32,
        });
        fields
    }

    /// `PositionSkipListFormat::Init`: `[u32 total_pos][u32 offset]`.
    pub fn skip_list_format(&self) -> SkipListFormat {
        SkipListFormat::for_position()
    }
}

/// `PositionListEncoder` (`position_list_encoder.cppm:19-57`).
#[derive(Clone, Debug)]
pub struct PositionListEncoder {
    option: PositionListFormatOption,
    buffer: PostingByteSlice,
    skiplist: SkipListWriter,
    last_pos_in_cur_doc: u32,
    total_pos_count: u32,
}

impl PositionListEncoder {
    pub fn new(option_flag: u8) -> Self {
        let option = PositionListFormatOption::new(option_flag);
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.fields());
        let mut skiplist = SkipListWriter::new();
        skiplist.init(&option.skip_list_format());
        Self {
            option,
            buffer,
            skiplist,
            last_pos_in_cur_doc: 0,
            total_pos_count: 0,
        }
    }

    pub fn get_total_pos_count(&self) -> u32 {
        self.total_pos_count
    }

    /// `PositionListEncoder::AddPosition`.
    pub fn add_position(&mut self, pos: u32) {
        self.buffer
            .push_back_u32(0, pos.wrapping_sub(self.last_pos_in_cur_doc));
        self.buffer.end_push_back();
        self.last_pos_in_cur_doc = pos;
        self.total_pos_count += 1;
        if self.buffer.need_flush(MAX_POS_PER_RECORD) {
            self.flush_position_buffer();
        }
    }

    /// `PositionListEncoder::EndDocument`.
    pub fn end_document(&mut self) {
        self.last_pos_in_cur_doc = 0;
    }

    fn flush_position_buffer(&mut self) {
        let flush_size = self.buffer.flush();
        if flush_size > 0 {
            self.skiplist
                .add_item_2(self.total_pos_count, flush_size as u32);
        }
    }

    /// `PositionListEncoder::Flush`.
    pub fn flush(&mut self) {
        self.flush_position_buffer();
        self.skiplist.flush();
    }

    /// `PositionListEncoder::GetBufferedByteSlice`.
    pub fn buffered_byte_slice(&self) -> &PostingByteSlice {
        &self.buffer
    }

    /// `PositionListEncoder::GetInMemPositionListDecoder`.
    pub fn in_mem_decoder(&self) -> InMemPositionListDecoder {
        let format = self.option.skip_list_format();
        let mut skiplist_reader = SkipListReader::new(&format);
        skiplist_reader.load_posting_byte_slice(self.skiplist.slice());
        let mut decoder = InMemPositionListDecoder::new(self.option);
        decoder.init(
            i64::from(self.total_pos_count),
            Some(skiplist_reader),
            std::mem::size_of::<SkipListReader>() as u32,
            self.buffer.snapshot(),
        );
        decoder
    }

    /// `PositionListEncoder::Dump`.
    pub fn dump(&mut self, spill: bool) -> Vec<u8> {
        let mut out = Vec::new();
        if spill {
            write_vint(&mut out, u64::from(self.last_pos_in_cur_doc));
            write_vint(&mut out, u64::from(self.total_pos_count));
        } else {
            self.flush();
            write_vint(&mut out, self.skiplist.bytes().len() as u64);
            write_vint(&mut out, self.buffer.bytes.len() as u64);
        }
        out.extend_from_slice(self.skiplist.bytes());
        out.extend_from_slice(&self.buffer.dump(spill));
        out
    }

    /// `PositionListEncoder::GetDumpLength`'s payload (used by callers that need
    /// the size before dumping).
    pub fn dump_length(&self) -> usize {
        let skiplist_len = self.skiplist.bytes().len();
        let buffer_len = self.buffer.bytes.len();
        vint_len(skiplist_len as u64) + vint_len(buffer_len as u64) + skiplist_len + buffer_len
    }
}

fn vint_len(mut value: u64) -> usize {
    let mut len = 1;
    while value >= 0x80 {
        value >>= 7;
        len += 1;
    }
    len
}

/// `InDocPositionState` (`in_doc_pos_state.cppm:13-58`).
#[derive(Clone, Copy, Debug, Default)]
pub struct InDocPositionState {
    pub tf: u32,
    record_offset: i32,
    offset_in_record: i32,
    total_position_count: u32,
    option: PositionListFormatOption,
}

impl InDocPositionState {
    pub fn new(option: PositionListFormatOption) -> Self {
        Self {
            tf: 0,
            record_offset: 0,
            offset_in_record: 0,
            total_position_count: 0,
            option,
        }
    }

    pub fn set_term_freq(&mut self, tf: u32) {
        self.tf = tf;
    }

    pub fn get_term_freq(&self) -> u32 {
        self.tf
    }

    pub fn set_record_offset(&mut self, offset: i32) {
        self.record_offset = offset;
    }

    pub fn get_record_offset(&self) -> i32 {
        self.record_offset
    }

    pub fn set_offset_in_record(&mut self, offset: i32) {
        self.offset_in_record = offset;
    }

    pub fn get_offset_in_record(&self) -> i32 {
        self.offset_in_record
    }

    pub fn set_total_pos_count(&mut self, count: u32) {
        self.total_position_count = count;
    }

    pub fn get_total_pos_count(&self) -> u32 {
        self.total_position_count
    }

    pub fn option(&self) -> PositionListFormatOption {
        self.option
    }
}

/// `InMemPositionListDecoder` (`inmem_position_list_decoder.cppm:17-40`).
pub struct InMemPositionListDecoder {
    option: PositionListFormatOption,
    total_tf: i64,
    skiplist_reader: Option<SkipListReader>,
    skiplist_reader_real_size: u32,
    pos_list_buffer: Option<PostingByteSlice>,
    pos_list_reader: ReaderState,
    decoded_pos_count: u32,
    record_offset: u32,
    pre_record_ttf: u32,
    offset_in_record: i32,
    last_decode_offset: u32,
    need_reopen: bool,
}

impl InMemPositionListDecoder {
    pub fn new(option: PositionListFormatOption) -> Self {
        Self {
            option,
            total_tf: 0,
            skiplist_reader: None,
            skiplist_reader_real_size: 0,
            pos_list_buffer: None,
            pos_list_reader: ReaderState::default(),
            decoded_pos_count: 0,
            record_offset: 0,
            pre_record_ttf: 0,
            offset_in_record: 0,
            last_decode_offset: 0,
            need_reopen: true,
        }
    }

    /// `InMemPositionListDecoder::Init`.
    pub fn init(
        &mut self,
        total_tf: i64,
        skiplist_reader: Option<SkipListReader>,
        skiplist_reader_real_size: u32,
        pos_list_buffer: PostingByteSlice,
    ) {
        self.total_tf = total_tf;
        self.skiplist_reader = skiplist_reader;
        self.skiplist_reader_real_size = skiplist_reader_real_size;
        self.pos_list_reader.open();
        self.pos_list_buffer = Some(pos_list_buffer);
        self.decoded_pos_count = 0;
    }

    /// `InMemPositionListDecoder::SkipTo`.
    pub fn skip_to(&mut self, current_ttf: i64, state: &mut InDocPositionState) -> bool {
        if self.total_tf <= current_ttf {
            return false;
        }
        if self.skiplist_reader.is_none() {
            assert!(self.total_tf <= MAX_POS_PER_RECORD as i64);
            state.set_record_offset(0);
            state.set_offset_in_record(current_ttf as i32);
            return true;
        }
        if i64::from(self.decoded_pos_count) > current_ttf {
            state.set_offset_in_record(current_ttf as i32 - self.pre_record_ttf as i32);
            return true;
        }
        let result = self
            .skiplist_reader
            .as_mut()
            .unwrap()
            .skip_to(current_ttf as u32 + 1);
        let offset = if let Some(result) = result {
            self.pre_record_ttf = self
                .skiplist_reader
                .as_ref()
                .unwrap()
                .get_prev_key();
            self.decoded_pos_count = result.doc_id;
            result.offset
        } else {
            let reader = self.skiplist_reader.as_ref().unwrap();
            self.pre_record_ttf = reader.get_current_key();
            self.decoded_pos_count = self.total_tf as u32;
            reader.get_last_value_in_buffer()
        };
        self.record_offset = offset;
        state.set_record_offset(offset as i32);
        state.set_offset_in_record(current_ttf as i32 - self.pre_record_ttf as i32);
        true
    }

    /// `InMemPositionListDecoder::LocateRecord`.
    pub fn locate_record(&mut self, state: &InDocPositionState, tf: &mut u32) -> bool {
        self.record_offset = state.get_record_offset() as u32;
        self.offset_in_record = state.get_offset_in_record();
        *tf = state.tf;

        if !self.need_reopen && self.last_decode_offset == self.record_offset {
            return false;
        }
        if self.need_reopen || self.last_decode_offset > self.record_offset {
            self.pos_list_reader.open();
            self.last_decode_offset = 0;
        }
        if let Some(buffer) = &self.pos_list_buffer {
            self.pos_list_reader
                .seek(buffer, self.record_offset as usize);
        }
        self.need_reopen = false;
        true
    }

    /// `InMemPositionListDecoder::DecodeRecord`.
    pub fn decode_record(&mut self, pos_buffer: &mut Vec<u32>, pos_buffer_len: u32) -> u32 {
        self.last_decode_offset = self.pos_list_reader.tell() as u32;
        let Some(buffer) = &self.pos_list_buffer else {
            pos_buffer.clear();
            return 0;
        };
        match self
            .pos_list_reader
            .decode_u32(buffer, pos_buffer_len as usize)
        {
            Some(values) => {
                pos_buffer.clear();
                pos_buffer.extend_from_slice(&values);
                values.len() as u32
            }
            None => {
                pos_buffer.clear();
                0
            }
        }
    }

    pub fn get_record_offset(&self) -> u32 {
        self.record_offset
    }

    pub fn get_offset_in_record(&self) -> i32 {
        self.offset_in_record
    }

    pub fn option(&self) -> PositionListFormatOption {
        self.option
    }

    pub fn total_tf(&self) -> i64 {
        self.total_tf
    }

    pub fn skiplist_reader_real_size(&self) -> u32 {
        self.skiplist_reader_real_size
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::inverted_format::posting_slice::{MAX_DOC_PER_RECORD, SKIP_LIST_BUFFER_SIZE};

    const NO_PAYLOAD: u8 = OF_POSITION_LIST | OF_TERM_FREQUENCY;

    fn test_decode_with_option_flag(flag: u8, tf: u32, pos_list: &[u32], need_flush: bool) {
        let mut encoder = PositionListEncoder::new(flag);
        for &pos in pos_list.iter().take(tf as usize) {
            encoder.add_position(pos);
        }        encoder.end_document();
        if need_flush {
            encoder.flush();
        }
        let mut decoder = encoder.in_mem_decoder();
        let option = PositionListFormatOption::new(flag);
        let mut state = InDocPositionState::new(option);
        assert!(decoder.skip_to(0, &mut state));

        let mut temp_tf = 0;
        assert!(decoder.locate_record(&state, &mut temp_tf));
        assert_eq!(0, state.get_record_offset());
        assert_eq!(0, state.get_offset_in_record());

        let mut pos_buffer = Vec::new();
        let mut pos_idx = 0usize;
        let mut last_pos = 0u32;
        let mut i = 0;
        while i < tf / MAX_POS_PER_RECORD as u32 {
            let decode_count = decoder.decode_record(&mut pos_buffer, MAX_POS_PER_RECORD as u32);
            assert_eq!(MAX_POS_PER_RECORD as u32, decode_count);
            for &delta in &pos_buffer {
                assert_eq!(pos_list[pos_idx], last_pos.wrapping_add(delta));
                last_pos = last_pos.wrapping_add(delta);
                pos_idx += 1;
            }
            i += 1;
        }
        if !tf.is_multiple_of(MAX_POS_PER_RECORD as u32) {
            let decode_count = decoder.decode_record(&mut pos_buffer, MAX_POS_PER_RECORD as u32);
            assert_eq!(tf % MAX_POS_PER_RECORD as u32, decode_count);
            for &delta in &pos_buffer {
                assert_eq!(pos_list[pos_idx], last_pos.wrapping_add(delta));
                last_pos = last_pos.wrapping_add(delta);
                pos_idx += 1;
            }
        }
    }

    fn inner_test_decode(flag: u8, tf: u32) {
        let pos_list: Vec<u32> = (0..tf).map(|i| i * 2 + 1).collect();
        test_decode_with_option_flag(flag, tf, &pos_list, false);
        test_decode_with_option_flag(flag, tf, &pos_list, true);
    }

    fn inner_test_skip_and_locate_and_decode(flag: u8, ttf: u32, need_flush: bool) {
        let mut encoder = PositionListEncoder::new(flag);
        let mut sum = 0u32;
        for i in 0..ttf {
            sum = sum.wrapping_add(i);
            encoder.add_position(sum);
        }
        encoder.end_document();
        if need_flush {
            encoder.flush();
        }
        let mut decoder = encoder.in_mem_decoder();
        let option = PositionListFormatOption::new(flag);
        let mut state = InDocPositionState::new(option);

        let mut record_offset = 0i32;
        let mut temp_tf = 0u32;
        let mut pos_buffer = Vec::new();
        for i in 0..ttf {
            assert!(decoder.skip_to(i64::from(i), &mut state));
            if i.is_multiple_of(MAX_POS_PER_RECORD as u32) {
                assert!(decoder.locate_record(&state, &mut temp_tf));
                record_offset = state.get_record_offset();
                let decode_count =
                    decoder.decode_record(&mut pos_buffer, MAX_POS_PER_RECORD as u32);
                let expect = std::cmp::min(ttf - i, MAX_POS_PER_RECORD as u32);
                assert_eq!(expect, decode_count);
                for (j, delta) in pos_buffer.iter().enumerate() {
                    assert_eq!(i + j as u32, *delta);
                }
            } else {
                assert!(!decoder.locate_record(&state, &mut temp_tf));
                assert_eq!(record_offset, state.get_record_offset());
            }
            assert_eq!((i % MAX_POS_PER_RECORD as u32) as i32, state.get_offset_in_record());
        }
    }

    fn big_positions() -> Vec<u32> {
        vec![
            1,
            MAX_UNCOMPRESSED_POS_LIST_SIZE + 1,
            MAX_POS_PER_RECORD as u32,
            MAX_POS_PER_RECORD as u32 + 10,
            MAX_POS_PER_RECORD as u32 * 5 + 1,
            MAX_POS_PER_RECORD as u32 * 10 + 1,
            MAX_POS_PER_RECORD as u32 * SKIP_LIST_BUFFER_SIZE as u32 + 1,
            MAX_POS_PER_RECORD as u32 * SKIP_LIST_BUFFER_SIZE as u32 * 2 + 1,
        ]
    }

    #[test]
    fn inmem_position_list_decoder_test1() {
        let mut encoder = PositionListEncoder::new(NO_PAYLOAD);
        encoder.add_position(3);
        encoder.end_document();
        let mut decoder = encoder.in_mem_decoder();
        let option = PositionListFormatOption::new(NO_PAYLOAD);
        let mut state = InDocPositionState::new(option);
        assert!(decoder.skip_to(0, &mut state));
        let mut tf = 0;
        assert!(decoder.locate_record(&state, &mut tf));
        assert_eq!(0, state.get_record_offset());
        assert_eq!(0, state.get_offset_in_record());
        let mut pos_buffer = Vec::new();
        assert_eq!(
            1,
            decoder.decode_record(&mut pos_buffer, MAX_POS_PER_RECORD as u32)
        );
        assert_eq!(3, pos_buffer[0]);
    }

    #[test]
    fn inmem_position_list_decoder_test2() {
        let flag = OF_POSITION_LIST;
        let pos_list = [1, 5, 6, 19, 20];
        test_decode_with_option_flag(flag, 5, &pos_list, false);
        test_decode_with_option_flag(flag, 5, &pos_list, true);
    }

    #[test]
    fn inmem_position_list_decoder_test3() {
        let flag = OF_POSITION_LIST;
        for tf in big_positions() {
            inner_test_decode(flag, tf);
        }
    }

    #[test]
    fn inmem_position_list_decoder_test4() {
        let option = PositionListFormatOption::new(OF_POSITION_LIST);
        let mut state = InDocPositionState::new(option);
        let mut buffer = PostingByteSlice::new();
        buffer.init(&option.fields());
        let mut decoder = InMemPositionListDecoder::new(option);
        decoder.init(100, None, 0, buffer);

        assert!(!decoder.skip_to(101, &mut state));
        assert!(decoder.skip_to(50, &mut state));
        assert_eq!(0, state.get_record_offset());
        assert_eq!(50, state.get_offset_in_record());
    }

    #[test]
    fn inmem_position_list_decoder_test5() {
        let flag = OF_POSITION_LIST;
        for need_flush in [true, false] {
            for ttf in big_positions() {
                inner_test_skip_and_locate_and_decode(flag, ttf, need_flush);
            }
        }
    }

    #[test]
    fn position_list_encoder_test1() {
        let option_flag = OF_POSITION_LIST | OF_TERM_FREQUENCY;
        let mut encoder = PositionListEncoder::new(option_flag);
        for i in 0..=MAX_DOC_PER_RECORD as u32 + 1 {
            encoder.add_position(i);
        }
        encoder.end_document();
        let bytes = encoder.dump(false);
        assert!(!bytes.is_empty());
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("position");
        std::fs::write(&path, &bytes).unwrap();
        assert!(path.exists());
        std::fs::remove_file(&path).unwrap();
    }

    #[test]
    fn position_list_encoder_test2() {
        let option_flag = OF_POSITION_LIST | OF_TERM_FREQUENCY;
        let mut encoder = PositionListEncoder::new(option_flag);
        for i in 1..=MAX_DOC_PER_RECORD as u32 + 1 {
            encoder.add_position(i);
        }
        encoder.end_document();

        let mut reader = ReaderState::default();
        reader.open();
        let first = reader
            .decode_u32(encoder.buffered_byte_slice(), MAX_POS_PER_RECORD)
            .unwrap();
        assert_eq!(MAX_POS_PER_RECORD, first.len());
        assert!(first.iter().all(|value| *value == 1));
        let second = reader
            .decode_u32(encoder.buffered_byte_slice(), MAX_POS_PER_RECORD)
            .unwrap();
        assert_eq!(vec![1u32], second);
        assert!(reader
            .decode_u32(encoder.buffered_byte_slice(), MAX_POS_PER_RECORD)
            .is_none());
    }

    #[test]
    fn position_list_decoder_test1() {
        let option_flag = OF_POSITION_LIST | OF_TERM_FREQUENCY;
        let mut encoder = PositionListEncoder::new(option_flag);
        for i in 0..MAX_DOC_PER_RECORD as u32 + 2 {
            encoder.add_position(i);
        }
        encoder.end_document();

        let mut decoder = encoder.in_mem_decoder();
        let mut pos_buffer = vec![0u32; 10240];
        let pos_count1 = decoder.decode_record(&mut pos_buffer, MAX_POS_PER_RECORD as u32);
        let mut second = Vec::new();
        let pos_count2 = decoder.decode_record(&mut second, 2);
        assert_eq!(MAX_POS_PER_RECORD as u32, pos_count1);
        assert_eq!(2, pos_count2);
        assert_eq!(MAX_POS_PER_RECORD as u32 + 2, pos_count1 + pos_count2);

        let mut all = pos_buffer[..pos_count1 as usize].to_vec();
        all.extend_from_slice(&second);
        for (i, value) in all.iter().enumerate() {
            let expected_delta = u32::from(i > 0);
            assert_eq!(expected_delta, *value);
        }
    }
}
