********************************************************************************
   ZSV/lib: Memory usage
********************************************************************************

ZSVLIB makes efficient use of memory through the following techniques:

* Memory copying is minimized. Bytes are read from the input (e.g. via `fread`)
  and copied to a buffer. Usually, by the time the cell() and row() callbacks
  are called, no further copying is performed, and `zsv_get_cell()` will return
  a pointer back to the location in the buffer where the data was originally
  copied into from the initial read. Exceptions to this are:

    - escaped double-quotes are removed using a memmove call (e.g. `"aaa""aaa"`
      becomes `aaa"aaa`). Note: we are considering
      adding an option to skip this step; feel free to let us know if you have
      an opinion on that

    - when the end of the buffer is reached, any partial row content if moved
      to the beginning of the row. This occurs on average once every N rows,
      where N is the average number of rows contained in each chunk of data
      read from the input. By default, the buffer size is 256k, which typically
      yields a high enough value of N that the impact of end-of-buffer copy is
      negigible. If your table rows are so large as to make this copy operation
      have a noticeable performance impact, use a higher buffer size

    - when a cell value is fetched, the parser returns the cell contents, the
      length, and a flag indicating whether the contents contain any delimiter or
      embedded dbl-quote that will require the value to be further processed if
      output as CSV (or TSV, JSON etc). This allows the caller to skip unnecessary
      scanning of cell contents in the common case where no quoting is required

* Each row's entire contents are stored in a single contiguous block of memory.
  To operate on the entire block of row memory, you can simply operate on the memory
  that starts at the first cell's `.str` address and ends with the last cell's
  `.str + .len` address. This can be advantageous for bulk operations, especially
  those that can be vectorized

* The maximum row size is a function of the maximum size of the internal buffer,
  which is set (either to a default or a caller-specified value) when the parser
  is initialized