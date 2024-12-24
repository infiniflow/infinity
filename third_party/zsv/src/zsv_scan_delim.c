#ifdef ZSV_SUPPORT_PULL_PARSER

#define zsv_internal_save_reg(x) scanner->pull.regs->delim.x = x
#define zsv_internal_save_regs(loc)                                                                                    \
  do {                                                                                                                 \
    scanner->pull.regs->delim.location = loc;                                                                          \
    scanner->pull.buff = buff;                                                                                         \
    scanner->pull.bytes_read = bytes_read;                                                                             \
    zsv_internal_save_reg(i);                                                                                          \
    zsv_internal_save_reg(bytes_chunk_end);                                                                            \
    zsv_internal_save_reg(bytes_read);                                                                                 \
    zsv_internal_save_reg(delimiter);                                                                                  \
    zsv_internal_save_reg(c);                                                                                          \
    zsv_internal_save_reg(skip_next_delim);                                                                            \
    zsv_internal_save_reg(quote);                                                                                      \
    zsv_internal_save_reg(mask_total_offset);                                                                          \
    zsv_internal_save_reg(mask);                                                                                       \
    zsv_internal_save_reg(mask_last_start);                                                                            \
  } while (0)

#define zsv_internal_restore_reg(x) x = scanner->pull.regs->delim.x
#define zsv_internal_restore_regs()                                                                                    \
  do {                                                                                                                 \
    buff = scanner->pull.buff;                                                                                         \
    bytes_read = scanner->pull.bytes_read;                                                                             \
    zsv_internal_restore_reg(i);                                                                                       \
    zsv_internal_restore_reg(bytes_chunk_end);                                                                         \
    zsv_internal_restore_reg(bytes_read);                                                                              \
    zsv_internal_restore_reg(delimiter);                                                                               \
    zsv_internal_restore_reg(c);                                                                                       \
    zsv_internal_restore_reg(skip_next_delim);                                                                         \
    zsv_internal_restore_reg(quote);                                                                                   \
    zsv_internal_restore_reg(mask_total_offset);                                                                       \
    zsv_internal_restore_reg(mask);                                                                                    \
    zsv_internal_restore_reg(mask_last_start);                                                                         \
    memset(&v.dl, scanner->opts.delimiter, sizeof(zsv_uc_vector));                                                     \
    memset(&v.nl, '\n', sizeof(zsv_uc_vector));                                                                        \
    memset(&v.cr, '\r', sizeof(zsv_uc_vector));                                                                        \
    memset(&v.qt, scanner->opts.no_quotes > 0 ? 0 : '"', sizeof(v.qt));                                                \
  } while (0)
#endif

static enum zsv_status ZSV_SCAN_DELIM(struct zsv_scanner *scanner, unsigned char *buff, size_t bytes_read) {
  struct {
    zsv_uc_vector dl;
    zsv_uc_vector nl;
    zsv_uc_vector cr;
    zsv_uc_vector qt;
  } v;

  size_t i;
  size_t bytes_chunk_end;
  char delimiter;
  unsigned char c;
  char skip_next_delim;
  int quote;
  size_t mask_total_offset;
  zsv_mask_t mask;
  int mask_last_start;

#ifdef ZSV_SUPPORT_PULL_PARSER
  if (scanner->pull.regs->delim.location) {
    zsv_internal_restore_regs();
    if (scanner->pull.regs->delim.location == 1)
      goto zsv_cell_and_row_dl_1;
    goto zsv_cell_and_row_dl_2;
  }
#endif
  bytes_read += scanner->partial_row_length;
  i = scanner->partial_row_length;
  skip_next_delim = 0;
  bytes_chunk_end = bytes_read >= sizeof(zsv_uc_vector) ? bytes_read - sizeof(zsv_uc_vector) + 1 : 0;
  delimiter = scanner->opts.delimiter;
  scanner->partial_row_length = 0;

  // to do: move into one-time execution code?
  // (but, will also locate away from function stack)
  quote = scanner->opts.no_quotes > 0 ? -1 : '"';  // ascii code 34
  memset(&v.dl, delimiter, sizeof(zsv_uc_vector)); // ascii code 44
  memset(&v.nl, '\n', sizeof(zsv_uc_vector));      // ascii code 10
  memset(&v.cr, '\r', sizeof(zsv_uc_vector));      // ascii code 13
  memset(&v.qt, scanner->opts.no_quotes > 0 ? 0 : '"', sizeof(v.qt));

  if (scanner->quoted & ZSV_PARSER_QUOTE_PENDING) {
    // if we're here, then the last chunk we read ended with a lone quote char inside
    // a quoted cell, and we are waiting to find out whether it is followed by
    // another dbl-quote e.g. if the end of the last chunk is |, we had:
    //   ...,"hel"|"o"
    //   ...,"hel"|,...
    //   ...,"hel"|p,...
    scanner->quoted -= ZSV_PARSER_QUOTE_PENDING;
    if (buff[i] != quote) {
      scanner->quoted |= ZSV_PARSER_QUOTE_CLOSED;
      scanner->quoted -= ZSV_PARSER_QUOTE_UNCLOSED;
      scanner->quote_close_position = i - scanner->cell_start - 1;
    } else {
      scanner->quoted |= ZSV_PARSER_QUOTE_NEEDED;
      scanner->quoted |= ZSV_PARSER_QUOTE_EMBEDDED;
      i++;
    }
  }

#define scanner_last (i ? buff[i - 1] : scanner->last)

  mask_total_offset = 0;
  mask = 0;
  scanner->buffer_end = bytes_read;
  for (; i < bytes_read; i++) {
    if (UNLIKELY(mask == 0)) {
      mask_last_start = i;
      if (VERY_LIKELY(i < bytes_chunk_end)) {
        // keep going until we get a delim or we are at the eof
        mask_total_offset = vec_delims(buff + i, bytes_read - i, &v.dl, &v.nl, &v.cr, &v.qt, &mask);
        if (LIKELY(mask_total_offset != 0)) {
          i += mask_total_offset;
          if (VERY_UNLIKELY(mask == 0 && i == bytes_read))
            break; // vector processing ended on exactly our buffer end
        }
      } else if (skip_next_delim) {
        skip_next_delim = 0;
        continue;
      }
    }
    if (VERY_LIKELY(mask)) {
      size_t next_offset = NEXT_BIT(mask);
      i = mask_last_start + next_offset - 1;
      mask = clear_lowest_bit(mask);
      if (VERY_UNLIKELY(skip_next_delim)) {
        skip_next_delim = 0;
        continue;
      }
    }

    // to do: consolidate csv and tsv/scanner->delimiter parsers
    c = buff[i];
    if (LIKELY(c == delimiter)) { // case ',':
      if ((scanner->quoted & ZSV_PARSER_QUOTE_UNCLOSED) == 0) {
        scanner->scanned_length = i;
        cell_dl(scanner, buff + scanner->cell_start, i - scanner->cell_start);
        scanner->cell_start = i + 1;
        c = 0;
        continue; // this char is not part of the cell content
      } else
        // we are inside an open quote, which is needed to escape this char
        scanner->quoted |= ZSV_PARSER_QUOTE_NEEDED;
    } else if (UNLIKELY(c == '\r')) {
      if ((scanner->quoted & ZSV_PARSER_QUOTE_UNCLOSED) == 0) {
        scanner->scanned_length = i;
        enum zsv_status stat = cell_and_row_dl(scanner, buff + scanner->cell_start, i - scanner->cell_start);
        if (VERY_UNLIKELY(stat))
          return stat;
#ifdef ZSV_SUPPORT_PULL_PARSER
        if (scanner->pull.now) {
          scanner->pull.now = 0;
          scanner->row.used = scanner->pull.row_used;
          zsv_internal_save_regs(1);
          return zsv_status_row;
        }
      zsv_cell_and_row_dl_1:
        scanner->row.used = 0;
        scanner->pull.regs->delim.location = 0;
#endif
        scanner->cell_start = i + 1;
        scanner->row_start = i + 1;
        scanner->data_row_count++;
        continue; // this char is not part of the cell content
      } else
        // we are inside an open quote, which is needed to escape this char
        scanner->quoted |= ZSV_PARSER_QUOTE_NEEDED;
    } else if (UNLIKELY(c == '\n')) {
      if ((scanner->quoted & ZSV_PARSER_QUOTE_UNCLOSED) == 0) {
        if (scanner_last == '\r') { // ignore; we are outside a cell and last char was rowend
          scanner->cell_start = i + 1;
          scanner->row_start = i + 1;
        } else {
          // this is a row end
          scanner->scanned_length = i;
          enum zsv_status stat = cell_and_row_dl(scanner, buff + scanner->cell_start, i - scanner->cell_start);
          if (VERY_UNLIKELY(stat))
            return stat;
#ifdef ZSV_SUPPORT_PULL_PARSER
          if (scanner->pull.now) {
            scanner->pull.now = 0;
            scanner->row.used = scanner->pull.row_used;
            zsv_internal_save_regs(2);
            return zsv_status_row;
          }
        zsv_cell_and_row_dl_2:
          scanner->row.used = 0;
          scanner->pull.regs->delim.location = 0;
#endif
          scanner->cell_start = i + 1;
          scanner->row_start = i + 1;
          scanner->data_row_count++;
        }
        continue; // this char is not part of the cell content
      } else
        // we are inside an open quote, which is needed to escape this char
        scanner->quoted |= ZSV_PARSER_QUOTE_NEEDED;
    } else if (LIKELY(c == quote)) {
      if (i == scanner->cell_start && !scanner->buffer_exceeded) {
        scanner->quoted = ZSV_PARSER_QUOTE_UNCLOSED;
        scanner->quote_close_position = 0;
        c = 0;
      } else if (scanner->quoted & ZSV_PARSER_QUOTE_UNCLOSED) {
        // the cell started with a quote that is not yet closed
        if (VERY_LIKELY(i + 1 < bytes_read)) {
          if (LIKELY(buff[i + 1] != quote)) {
            // buff[i] is the closing quote (not an escaped quote)
            scanner->quoted |= ZSV_PARSER_QUOTE_CLOSED;
            scanner->quoted -= ZSV_PARSER_QUOTE_UNCLOSED;

            // keep track of closing quote position to handle the edge case
            // where content follows the closing quote e.g. cell content is:
            //  "this-cell"-did-not-need-quotes
            if (LIKELY(scanner->quote_close_position == 0))
              scanner->quote_close_position = i - scanner->cell_start;
          } else {
            // next char is also '"'
            // e.g. cell content is: "this "" is a dbl quote"
            //           cursor is here => ^
            // include in cell content and don't further process
            scanner->quoted |= ZSV_PARSER_QUOTE_NEEDED;
            scanner->quoted |= ZSV_PARSER_QUOTE_EMBEDDED;
            skip_next_delim = 1;
          }
        } else // we are at the end of this input chunk
          scanner->quoted |= ZSV_PARSER_QUOTE_PENDING;
      } else {
        // cell_length > 0 and cell did not start w quote, so
        // we have a quote in middle of an unquoted cell
        // process as a normal char
        scanner->quoted |= ZSV_PARSER_QUOTE_EMBEDDED;
        scanner->quote_close_position = 0;
      }
    }
  }
  scanner->scanned_length = i;

  // save bytes_read-- we will need to shift any remaining partial row
  // before we read next from our input. however, we intentionally refrain
  // from doing this until the next parse_more() call, so that the entirety
  // of all rows parsed thus far are still available until that next call
  scanner->old_bytes_read = bytes_read;

  return zsv_status_ok;
}
