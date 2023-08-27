#include <stdio.h>
#include <string.h>
#include <zsv.h>

/**
 * Simple example using libzsv as a pull parser for CSV input
 *
 * This is the same as simple.c, but uses pull parsing instead of push parsing
 *
 * We will check each cell in the row to determine if it is blank, and output
 * the row number, the total number of cells and the number of blank cells
 *
 * Example:
 *   `echo 'abc,def\nghi,,,' | build/simple -`
 * Outputs:
 *   Row 1 has 2 columns of which 0 are non-blank
 *   Row 2 has 4 columns of which 3 are non-blank
 *
 * With pull parsing, the parser just repeatedly calls zsv_next_row() so long
 * as it returns `zsv_status_row`
 */

/**
 * Main routine. Our program will take a single argument (a file name, or -)
 * and output, for each row, the numbers of total and blank cells
 */
int main(int argc, const char *argv[]) {
  if(argc != 2) {
    fprintf(stderr, "Reads a CSV file or stdin, and for each row,\n"
            " output counts of total and blank cells\n");
    fprintf(stderr, "Usage: simple <filename or dash(-) for stdin>\n");
    fprintf(stderr, "Example:\n"
            "  echo \"A1,B1,C1\\nA2,B2,\\nA3,,C3\\n,,C3\" | %s -\n\n", argv[0]);
    return 0;
  }

  FILE *f = strcmp(argv[1], "-") ? fopen(argv[1], "rb") : stdin;
  if(!f) {
    perror(argv[1]);
    return 1;
  }

  /**
   * Create a parser
   */
  struct zsv_opts opts = { 0 };
  opts.stream = f;
  zsv_parser parser = zsv_new(&opts);
  if(!parser) {
    fprintf(stderr, "Could not allocate parser!\n");
    return -1;
  }

  /**
   * iterate through all rows
   */
  size_t row_num = 0;
  while(zsv_next_row(parser) == zsv_status_row) {
    row_num++;

    /* get a cell count */
    size_t cell_count = zsv_cell_count(parser);

    /* iterate through each cell in this row, to count blanks */
    size_t nonblank = 0;
    for(size_t i = 0; i < cell_count; i++) {
      struct zsv_cell c = zsv_get_cell(parser, i);
      /* use r.values[] and r.lengths[] to get cell data */
      /* Here, we only care about lengths */
      if(c.len > 0)
        nonblank++;
    }

    /* print our results for this row */
    printf("Row %zu has %zu columns of which %zu %s non-blank\n", row_num,
           cell_count, nonblank, nonblank == 1 ? "is" : "are");
  }
  /**
   * Clean up
   */
  zsv_delete(parser);

  if(f != stdin)
    fclose(f);

  return 0;
}
