#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zsv.h>

/** zsv example parsing in chunks
 *
 * This sample code shows how to use `zsv_parse_bytes()`
 * to parse a chunk of bytes, instead of using `zsv_parse_more()`
 * to pull data from a stream
 *
 * Note that, when given a choice, using `zsv_parse_more()`
 * may be slightly more efficient / performant as is requires
 * less memory copying
 *
 * In this example, we just count rows, but you could substitute in any
 * row handler you want
 **/

/**
 * Create a structure to hold our data while we parse
 * In this case, we are just going to keep track of row count
 */
struct chunk_parse_data {
  unsigned count;
};

/**
 * Our row handler function will take a pointer to our data
 * and increment its count by 1
 */
static void chunk_parse_row(void *dat) {
  struct chunk_parse_data *data = dat;
  data->count++;
}

/**
 * Main routine will output a help message if called with -h or --help,
 * otherwise will read from stdin and run through the CSV parser
 */
int main(int argc, const char *argv[]) {
  if(argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
    printf("Usage: parse_by_chunk < myfile.csv\n\n");
    printf("Reads stdin in chunks, parses each chunk using `zsv_parse_bytes()`,\n");
    printf("and outputs the number of rows parsed.\n");
    return 0;
  }

  FILE *f = stdin; /* read from stdin */

  /**
   * create a vanilla parser
   */
  zsv_parser p = zsv_new(NULL);
  if(!p)
    fprintf(stderr, "Out of memory!");
  else {
    /**
     * Configure the parser to use our row handler, and to pass
     * it our data when it's called
     */
    struct chunk_parse_data d = { 0 };
    zsv_set_row_handler(p, chunk_parse_row);
    zsv_set_context(p, &d);

    /**
     * Allocate a buffer that we will fetch data from and pass to the parser.
     * In this example we use a heap buffer, but we could just as well
     * have allocated it on the stack
     */
    int chunk_size = 4096;
    unsigned char *buff = malloc(chunk_size);

    /**
     * Read and parse each chunk until the end of the stream is reached
     */
    while(1) {
      size_t bytes_read = fread(buff, 1, chunk_size, f);
      if(!bytes_read)
        break;
      zsv_parse_bytes(p, buff, bytes_read);
    }

    /**
     * Finish any remaining parsing
     */
    zsv_finish(p);

    /**
     * Clean up
     */
    zsv_delete(p);
    free(buff);

    /**
     * Print result
     */
    printf("Count: %u\n", d.count);
  }
}
