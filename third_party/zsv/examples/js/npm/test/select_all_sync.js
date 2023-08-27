const process= require('node:process');
const { PerformanceObserver, performance } = require('node:perf_hooks');
const fs = require('fs');
const zsvParser = require('zsv-lib');

/**
 * Example using libzsv to parse CSV input and execute a custom row handler function as each row is parsed
 */

/**
 * We will use a separate context for each parser, which is a pattern that allows us to run multiple
 * parsers at the same time independently, although this example only runs one at a time
 */
function createContext() {
  return {
    rowcount: 0,                  // how many rows we've parsed so far
    startTime: performance.now(), // when the run was started
    data: []                     // object to hold all data parsed thus far
  };
}

/**
 * Define a row handler which will be called each time a row is parsed, and which
 * accesses all data through a context object
 */
function rowHandler(row, ctx, z) {
  ctx.rowcount++;
  /* if we had created the parser with option rowData === false,
     we could fetch the row data ourselves with the below:

  let count = parser.cellCount();
  let row = [];
  for(let i = 0; i < count; i++)
    row.push(parser.getCell(i));
  */
  ctx.data.push(row);
}

/**
 * Define the steps to take after all parsing has completed
 */
function finish(ctx, parser) {
  if(parser) {
    parser.finish();                  /* finish parsing */
    let endTime = performance.now()   /* check the time */

    /* output a message describing the parse volume and performance */
    console.error('Parsed ' + parser.getBytesRead() + ' bytes; ' + ctx.rowcount +
                  ' rows in ' + (endTime - ctx.startTime) + 'ms\n' +
                  'You can view the parsed data in your browser dev tools console (rt-click and select Inspect)');

    /**
     * output the parsed data (we could have also done this while we parsed, and not
     * bothered to accumulate it, to save memory)
     */
    console.log(ctx.data);

    /* destroy the parser */
    parser.delete();
  }
}

/**
 * After the zsv-lib module has loaded, read from stdin or the specified file,
 * parse the input, apply the row handler for each parsed row and finish
 */
zsvParser.runOnLoad(function() {

  /* get a new context */
  let ctx = createContext();

  /* read stdin if we have no arguments, else the first argument */
  const readFile = process.argv.length < 3 ? process.stdin : { fd: fs.openSync(process.argv[2], 'r') };

  /* initialize parser */
  let parser = zsvParser.new(rowHandler, ctx, { sync: readFile });

  /* set the parser input */
  parser.syncInput(readFile);

  /* parse */
  while(parser.parseMore() == 0);

  /* finish */
  finish(ctx, parser);
});
