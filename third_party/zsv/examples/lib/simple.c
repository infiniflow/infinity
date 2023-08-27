#include <stdio.h>
#include <string.h>
#include <zsv.h>

/**
 * Simple example using libzsv to call a row handler after each row is parsed
 *
 * In this super simple example, we will use libzsv to parse a CSV file
 * and call our custom row handler after each row is parsed. Our row handler
 * will check each cell in the row to determine if it is blank, and output
 * the row number, the total number of cells and the number of blank cells
 *
 * Example:
 *   `echo 'abc,def\nghi,,,' | build/simple -`
 * Outputs:
 *   Row 1 has 2 columns of which 0 are non-blank
 *   Row 2 has 4 columns of which 3 are non-blank
 */

/**
 * First, we define a structure that will contain all the information that
 * our row handler will need
 */
struct my_data {
  zsv_parser parser; /* used to access the parsed data */
  size_t row_num;    /* used to track the current row number */
};

/**
 * Our row handler is passed a pointer to our structure
 */
void my_row_handler(void *ctx) {
  struct my_data *data = ctx;

  /* get a cell count */
  size_t cell_count = zsv_cell_count(data->parser);

  /* iterate through each cell in this row, to count blanks */
  size_t nonblank = 0;
  for(size_t i = 0; i < cell_count; i++) {
    /* use zsv_get_cell() to get our cell data */
    struct zsv_cell c = zsv_get_cell(data->parser, i);

    /* check if the cell data length is zero */
    if(c.len > 0)
      nonblank++;
  }

  /* print our results for this row */
  printf("Row %zu has %zu columns of which %zu %s non-blank\n", ++data->row_num, cell_count, nonblank, nonblank == 1 ? "is" : "are");
}

/**
 * Main routine. Our program will take a single argument (a file name, or -)
 * and output, for each row, the numbers of total and blank cells
 */
int main(int argc, const char *argv[]) {

  /**
   * Process our arguments; output usage and/or errors if appropriate
   */
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
   * Configure our parser options. Here, all we do is specify our row handler
   * and the pointer that will be passed to our row handler. There are many
   * other options to control the parser behavior (CSV vs TSV vs other
   * delimited; header row span etc)-- for details, see
   * ../../include/zsv/api.h
   */
  struct zsv_opts opts = { 0 };
  opts.row_handler = my_row_handler;
  struct my_data data = { 0 };
  opts.ctx = &data;
  opts.stream = f;

  /**
   * Create a parser
   */
  data.parser = zsv_new(&opts);

  /**
   * Continuously parse our input until we have no more input
   */
  enum zsv_status stat;
  while((stat = zsv_parse_more(data.parser)) == zsv_status_ok)
    ;

  /**
   * Clean up
   */
  zsv_finish(data.parser);
  zsv_delete(data.parser);

  if(f != stdin)
    fclose(f);

  /**
   * If there was a parse error, print it
   */
  if(stat != zsv_status_no_more_input) {
    fprintf(stderr, "Parse error: %s\n", zsv_parse_status_desc(stat));
    return 1;
  }

  return 0;
}
