//
// Created by jinhai on 23-8-27.
//

#include <cstdio>
#include <iostream>
#include <string>
#include <filesystem>

#include "csv_config.h"

extern "C" {
#include "zsv.h"
}

struct my_data {
    zsv_parser parser; /* used to access the parsed data */
    size_t row_num;    /* used to track the current row number */
};

/**
 * Our row handler is passed a pointer to our structure
 */
void
my_row_handler(void* ctx) {
    struct my_data* data = static_cast<my_data*>(ctx);

    /* get a cell count */
    size_t cell_count = zsv_cell_count(data->parser);

    /* iterate through each cell in this row, to count blanks */
    size_t nonblank = 0;
    for(size_t i = 0; i < cell_count; i++) {
        /* use zsv_get_cell() to get our cell data */
        struct zsv_cell c = zsv_get_cell(data->parser, i);

        printf("%s, ", c.str);
        /* check if the cell data length is zero */
        if(c.len > 0)
            nonblank++;
    }
    printf("\n");

    /* print our results for this row */
    printf("Row %zu has %zu columns of which %zu %s non-blank\n",
           ++data->row_num,
           cell_count,
           nonblank,
           nonblank == 1 ? "is" : "are");
}

auto
main() -> int {

    std::cout << std::filesystem::current_path() << std::endl;

    std::string filename = std::string(CSV_DATA_PATH) + "/test/flatten.csv";

    FILE* f = fopen(filename.c_str(), "rb");
    if(!f) {
        perror(filename.c_str());
        return 1;
    }

    /**
     * Configure our parser options. Here, all we do is specify our row handler
     * and the pointer that will be passed to our row handler. There are many
     * other options to control the parser behavior (CSV vs TSV vs other
     * delimited; header row span etc)-- for details, see
     * ../../include/zsv/api.h
     */
    struct zsv_opts opts = {};
    opts.row_handler = my_row_handler;
    struct my_data data = {};
    opts.ctx = &data;
    opts.stream = f;
    opts.delimiter = ',';


    /**
     * Create a parser
     */
    data.parser = zsv_new(&opts);

    /**
     * Continuously parse our input until we have no more input
     */
    enum zsv_status stat;
    while((stat = zsv_parse_more(data.parser)) == zsv_status_ok) {

        break;
    }


    /**
     * Clean up
     */
    zsv_finish(data.parser);
    zsv_delete(data.parser);

    if(f != stdin)
        fclose(f);

    if(stat == zsv_status_ok) {
        return 0;
    }

    /**
     * If there was a parse error, print it
     */
    if(stat != zsv_status_no_more_input) {
        fprintf(stderr, "Parse error: %s\n", zsv_parse_status_desc(stat));
        return 1;
    }

    return 0;
}
