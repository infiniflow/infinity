# zsv-lib

zsv-lib is a node package of the high-performance zsv parser library for CSV and other delimited
or fixed-width tabular text data

This is a low-level library for "unopinionated" CSV parsing. Note that the related zsv CLI,
which offers additional features such as ad hoc SQL, header manipulation, serializing, flattening,
converting to JSON or SQLITE3 etc, is not part of this package, but may in the future be offered
as part of a separate npm package.

Note: the zsv C library supports a very wide range of options; not all are yet exposed in this
package. Please feel free to post a ticket at https://github.com/liquidaty/zsv/issues
if you'd like to request any feature that is available in the underlying library but
not exposed in this package (or that does not yet exist in the underlying library at all)

Please (see here for further notes re performance)[../README.md#Performance].

For more information about zsv+lib, please visit https://github.com/liquidaty/zsv

## Installation

```bash
npm install zsv-lib
```

## Usage

libzsv works generally as follows:
1. Create a custom a row handler function and create a parser with that function
2. Push chunks of data through the parser
3. The parser calls your custom function for each row that is parsed
4. Your row handler extracts data from the parser and processes it however you want

See the source for a (simple example that counts rows)[test/count.js]
or (another simple example that outputs row data as json)[test/select_all.js]

THIS PACKAGE IS STILL IN ALPHA. THE API USED IN THE EXAMPLES IS EXPECTED TO CHANGE.

### API

Note: libzsv supports a wide range of options; not all are yet exposed in this
package. Please feel free to post a ticket at https://github.com/liquidaty/zsv/issues
if you'd like to request any feature
that is either unexposed from the underlying library, or does not yet exist in the
underlying library


```
/**
 * Create a parser
 * @param  rowHandler callback that takes a single (optional) argument
 * @return parser
 */
new(rowHandler, ctx)

/**
 * Incrementally parse a chunk of data
 * @param  parser
 * @param  chunk  byte array of data to parse
 * @return 0 on success, non-zero on error
 */
parseBytes(parser, chunk)

/**
 * Get the number of cells in the current row
 * This function should be called from within your rowHandler callback
 * @param  parser
 * @return number of cells in the current row
 */
cellCount(parser)

/**
 * Get the value of a cell in the current row
 * This function should be called from within your rowHandler callback
 * @param  parser
 * @param  index  0-based index (cell position)
 * @return string value of the cell in the specified index position
 */
getCell(parser, index)

/**
 * Finish parsing, after the final `parseBytes()` call has already been made
 * @param  parser
 */
finish(parser)

/**
 * Destroy a parse that was created with `new()`
 * @param  parser
 */
delete(parser)

/**
 * Queue code to run only after the zsv-lib web assembly module has finished loading
 * @param callback function to call after the zsv-lib module has finished loaded
 */
runOnLoad(callback)

/**
 * Abort parsing prematurely, and do not make any more rowHandler calls
 * @param  parser
 */
abort(z)

```

## Contributing
Pull requests are welcome. Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)
