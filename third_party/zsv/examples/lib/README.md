# Using libzsv

## Compilable / runnable examples

This directory contains the following examples:

| file     | description |
| -- | -- |
| [pull.c](pull.c) | Same as simple.c, but uses pull parsing via `zsv_pull_next_row()`|
| [simple.c](simple.c) | parse a CSV file and for each row, output the row number, the total number of cells and the number of blank cells |
| [print_my_column.c](print_my_column.c) | parse a CSV file, look for a specified column of data, and for each row of data, output only that column |
| [parse_by_chunk.c](parse_by_chunk.c) | read a CSV file in chunks, parse each chunk, and output number of rows. This example uses `zsv_parse_bytes()` (whereas the other two examples use `zsv_parse_more()`) |

## Building

To build the examples in this directory, cd to here (`cd examples/lib`) and then run `make build`

For further make options, run `make`

## API overview

### Example

A typical usage conssits of:
- defining row and/or cell handler functions
- connecting an input stream, and
- running the parser

For example, to simply count lines:
```
void count_row(void *ctx) {
  unsigned *count = ctx;
  (*count)++;
}

zsv_parser p = zsv_new(NULL);
if(!p)
  fprintf(stderr, "Out of memory!\n");
else {
  size_t count = 0;
  zsv_set_input(stdin);
  zsv_set_row_handler(p, count_row);
  zsv_set_context(p, &count);
  zsv_finish(p);
  zsv_delete(p);
  printf("Parsed %zu rows\n", count);
}
```

### Getting data into the parser

Data can be passed into libzsv through any of the following:
1. passing a FILE pointer
2. passing a custom read function (such as `fread` and context pointer (such as `FILE *`))
3. passing a byte array

In the first two approaches, which are marginally faster due to less memory copying,
data is read in chunks by the library. In the third approach, each chunk is
passed manually by your code to the library.

### Getting parsed data from the parser to your code

You can either pull data from the parser, or push it to your code.

#### Pull parsing

With pull parsing, your code "pulls" each row using `zsv_next_row()`, which
returns `zsv_status_row` until no more rows are left to parse

```
zsv_parser parser = zsv_new(...);
while(zsv_next_row(parser) == zsv_status_row) { /* for each row */
    // do something
}
```

An full example is at [pull.c](pull.c).

#### Cell iteration

For either pull or push parsing, the current row and each of its cells can be
fetched using `zsv_cell_count()` and `zsv_get_cell()`:

```
size_t cell_count = zsv_cell_count(parser);

for(size_t i = 0; i < cell_count; i++) {      /* for each cell in this row */
  struct zsv_cell c = zsv_get_cell(parser, i);
  /* now c.len contains the cell length in bytes */
  /* if c.len > 0, c.str contains the cell contents */
  printf("Got cell: %.*s\n", c.len, c.len ? c.str : "");
}
```

#### Push parsing

Data is "pushed" to handler callback, for each cell and/or row that is parsed. Each
series of "pushes" is invoked via `zsv_parse_more()`, which reads the next chunk of
bytes from your source and parses all of its contents.

Push parsing in conjunction with a file or custom reader performs the fastest
of all of the approaches, but the difference is too small to be measurable for
any but the most trivial of tasks.

##### Row handling

With push parsing, you specify a cell and/or row handler.
Usually, only a row handler will suffice. If your row handler code
needs access to row contents (which is usually the case), then
you just need to ensure its context argument points to a structure
that contains your zsv parser, so that the row
contents can be retrieved.

For example, we could process each cell as follows:
```
static void my_row_handler(void *ctx) {
  struct my_data *data = ctx;

  /* print every other cell */
  size_t cell_count = zsv_cell_count(data->parser);
  for(size_t i = 0, j = zsv_cell_count(data->parser); i < j; i++) {
    /* use zsv_get_cell() and do something */
  }
}

...

struct my_data d = { 0 };
d.parser = zsv_new(NULL);
zsv_set_row_handler(d.parser, my_row_handler);
zsv_set_context(p, &d);
```

### Other options
Many other options are supported through the use of `struct zsv_opts`, only some of which are described
in this introduction. For further detail, please visit
[zsv/common.h](../../include/zsv/common.h).

#### Delimiter
The `delimiter` option can specify any single-character delimiter other than
newline, form feed or quote. The default value is a comma.

#### Blank leading rows
By default, libzsv skips any leading blank rows, before it makes any callbacks. This behavior
can be disabled by setting the `keep_empty_header_rows` option flag.

#### Maximum number of rows
By default, libzsv assumes a maximum number of columns per table of 1024. This
can be changed by setting the `max_columns` option.

#### Header row span
If your header spans more than one row, you can instruct libzsv to merge header cells
with a single space before it makes its initial row handler call by setting `header_span`
to a number greater than 1. For example, if your input is as follows:
```
Street,Phone,City,Zip
Address,Number,,Code
888 Madison,212-555-1212,New York,10001
```

and you set `header_span` to 2, then in your first row handler call, the cell values
will be `Street Address`, `Phone Number`, `City` and `Zip Code`, after which the
row handler will be called for each single row that is parsed.
