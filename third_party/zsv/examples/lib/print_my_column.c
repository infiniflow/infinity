/**
 * Simple example using libzsv to call a row handler after each row is parsed
 *
 * In this example, we will use libzsv to parse a CSV file, look for a specified
 * column of data, and for each row of data, output only that column
 *
 * Example:
 *   `echo "hi,there,you\na,b,c\nd,e,f" | ./print_my_column there
 * Outputs:
 *   there
 *
 * In our implementation, we will define two row handlers. The first handler
 * will be used to handle the first row, which we assume to be the header,
 * and will search for the target column to identify what position it is in
 * (or whether we can't find it). The second handler will be used to process
 * each data row, and will output the target column
 */
static void print_my_column(void *ctx);
static void find_my_column(void *ctx);

#include <stdio.h>
#include <zsv.h>
#include <string.h>

/**
 * First, we define a structure that will contain all the information that
 * our row handler will need
 */
struct my_data {
  zsv_parser parser;
  const char *target_column_name; /* name of column to find and output */
  size_t target_column_position;  /* will be set to the position of the column to output */
  char not_found;                 /* if we can't find the column, we'll set this flag */
};

/**
 * Our first callback will search each cell in the (header) row. If it finds a match
 * it sets the `target_column_position`, otherwise it halts further processing and
 * sets the `not_found` flag
 */
static void find_my_column(void *ctx) {
  struct my_data *data = ctx;
  size_t target_column_name_len = strlen(data->target_column_name);

  /* iterate through each cell */
  size_t cell_count = zsv_cell_count(data->parser);
  char found = 0;
  for(size_t i = 0; i < cell_count; i++) {
    struct zsv_cell c = zsv_get_cell(data->parser, i);
    if(c.len == target_column_name_len && !memcmp(data->target_column_name, c.str, c.len)) {
      data->target_column_position = i;
      found = 1;
      break;
    }
  }

  if(!found) {
    /**
     * We couldn't find the target column name in our header row. Output a message and abort
     */
    fprintf(stderr, "Could not find column %.*s\n", (int)target_column_name_len, data->target_column_name);

    /**
     * Once `zsv_abort()` is called, the parser will no longer invoke any callbacks
     * and `zsv_parse_more()` will return `zsv_status_cancelled`
     */
    zsv_abort(data->parser);

    /**
     * set a flag that we will later use to determine our program's exit code
     */
    data->not_found = 1;
  } else {
    /**
     * we found the column we are looking for. print its name, and change our row handler
     * by calling `zsv_set_row_handler()`
     */
    printf("%s\n", data->target_column_name);
    zsv_set_row_handler(data->parser, print_my_column);
  }
}

/**
 * Output data from the selected column
 */
static void print_my_column(void *ctx) {
  struct my_data *data = ctx;

  /* use zsv_get_cell() to get our cell data */
  struct zsv_cell c = zsv_get_cell(data->parser, data->target_column_position);

  /* print the data, followed by a newline */
  printf("%.*s\n", (int)c.len, c.str);
}

/**
 * Main routine. Our example will take a single argument specifying a column
 * name, read from stdin, and output, for each row, the specified column
 */
int main(int argc, const char *argv[]) {
  if(argc < 2) {
    fprintf(stderr, "Usage: print_my_column column_name < input.csv\n");
    fprintf(stderr, "Example:\n"
            "  echo \"A,B,C\\nA1,B1,C1\\nA2,B2,\\nA3,,C3\\n,,C3\" | %s B\n\n", argv[0]);
    return 0;
  }

  /**
   * Initialize context data
   */
  struct my_data data = { 0 };
  data.target_column_name = argv[1];

  /**
   * Initialize parser options
   */
  struct zsv_opts opts = { 0 };
  opts.row_handler = find_my_column;
  opts.ctx = &data;

  /**
   * Create a parser
   */
  data.parser = zsv_new(&opts);

  /**
   * Continuously parse our input until we have no more input
   * or an error has occurred (such as not finding the specified
   * column name in the first row)
   */
  while(zsv_parse_more(data.parser) == zsv_status_ok)
    ;

  /**
   * Clean up
   */
  zsv_finish(data.parser);
  zsv_delete(data.parser);

  return data.not_found;
}
