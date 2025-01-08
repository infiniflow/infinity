static inline char row_fx(struct zsv_scanner *scanner, unsigned char *buff, size_t row_start, size_t row_end) {
  size_t cell_start = row_start;
  size_t row_length = row_end - row_start;
  for (unsigned i = 0; i < scanner->fixed.count; i++) {
    size_t cell_end = row_start + (scanner->fixed.offsets[i] > row_length ? row_length : scanner->fixed.offsets[i]);
    size_t cell_length = cell_end - cell_start;
    unsigned char *s = buff + cell_start;
    if (UNLIKELY(scanner->opts.cell_handler != NULL))
      scanner->opts.cell_handler(scanner->opts.ctx, s, cell_length);
    struct zsv_cell c = {s, cell_length, 1, 0};
    scanner->row.cells[scanner->row.used++] = c;

    cell_start = cell_end;
  }
  if (VERY_LIKELY(scanner->opts.row_handler != NULL))
    scanner->opts.row_handler(scanner->opts.ctx);
  scanner->row.used = 0;
  return scanner->abort;
}

static enum zsv_status zsv_scan_fixed(struct zsv_scanner *scanner, unsigned char *buff, size_t bytes_read) {
  bytes_read += scanner->partial_row_length;
  unsigned char c;
  size_t bytes_chunk_end = bytes_read >= sizeof(zsv_uc_vector) ? bytes_read - sizeof(zsv_uc_vector) + 1 : 0;

  scanner->partial_row_length = 0;

  // dl_v and qt_v are unused, we just leave them to reuse vec_delims()
  zsv_uc_vector dl_v;
  memset(&dl_v, 0, sizeof(zsv_uc_vector));
  zsv_uc_vector nl_v;
  memset(&nl_v, '\n', sizeof(zsv_uc_vector));
  zsv_uc_vector cr_v;
  memset(&cr_v, '\r', sizeof(zsv_uc_vector));
  zsv_uc_vector qt_v;
  memset(&qt_v, 0, sizeof(zsv_uc_vector));
  size_t mask_total_offset = 0;
  zsv_mask_t mask = 0;
  int mask_last_start = 0;

  scanner->buffer_end = bytes_read;
  for (size_t i = scanner->partial_row_length;; i++) {
    if (UNLIKELY(mask == 0)) {
      mask_last_start = i;
      if (LIKELY(i < bytes_chunk_end)) {
        // keep going until we get a delim or we are at the eof
        mask_total_offset = vec_delims(buff + i, bytes_read - i, &dl_v, &nl_v, &cr_v, &qt_v, &mask);
        if (mask_total_offset)
          i += mask_total_offset;
      } else { // we only have a few bytes left, so manually parse
        for (unsigned i2 = i; i2 < bytes_read; i2++)
          if (strchr("\n\r", buff[i2]))
            mask += 1 << (i2 - i);
      }
      if (UNLIKELY(mask == 0))
        break;
    }

    size_t next_offset = __builtin_ffs(mask);
    i = mask_last_start + next_offset - 1;
    mask = clear_lowest_bit(mask);

    c = buff[i];
    if (LIKELY(c == '\n')) {
      if (scanner_last == '\r') { // ignore; we are outside a cell and last char was rowend
        scanner->row_start = i + 1;
      } else {
        // this is a row end
        scanner->scanned_length = i;
        if (VERY_UNLIKELY(row_fx(scanner, buff, scanner->row_start, i)))
          return zsv_status_cancelled; // abort
        scanner->row_start = i + 1;
      }
    } else if (UNLIKELY(c == '\r')) {
      scanner->scanned_length = i;
      if (VERY_UNLIKELY(row_fx(scanner, buff, scanner->row_start, i)))
        return zsv_status_cancelled;
      scanner->row_start = i + 1;
    }
  }

  // save bytes_read-- we will need to shift any remaining partial row
  // before we read next from our input. however, we intentionally refrain
  // from doing this until the next parse_more() call, so that the entirety
  // of all rows parsed thus far are still available until that next call
  scanner->old_bytes_read = bytes_read;
  return zsv_status_ok;
}
