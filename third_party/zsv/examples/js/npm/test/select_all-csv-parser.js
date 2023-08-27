const process= require('node:process');
const { PerformanceObserver, performance } = require('node:perf_hooks');
const fs = require('fs');
const csv = require('csv-parser')

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
    data: [],                     // object to hold all data parsed thus far
    bytesRead: 0                  // how many bytes we've parsed thus far
  };
}

/**
 * Define the steps to take after all parsing has completed
 */
function finish(ctx) {
  let endTime = performance.now()   /* check the time */

  /* output a message describing the parse volume and performance */
  console.error('Parsed ' + ctx.rowcount +
                ' rows in ' + (endTime - ctx.startTime) + 'ms\n' +
                'You can view the parsed data in your browser dev tools console (rt-click and select Inspect)');
  
  /**
   * output the parsed data (we could have also done this while we parsed, and not
   * bothered to accumulate it, to save memory)
   */
  console.log(ctx.data);
}


let ctx = createContext();

let opts = {};
if(process.argv.length > 3 && process.argv[3]) {
  let indexes = JSON.parse(process.argv[3]);
  opts.mapHeaders = ({ header, index }) => (indexes.indexOf(index) > -1 ? header : null);
}

/* read stdin if we have no arguments, else the first argument */
const readStream = process.argv.length < 3 || !process.argv[2] ? process.stdin : fs.createReadStream(process.argv[2])
readStream.on('error', (error) => console.log(error.message));
readStream
  .pipe(csv(opts))
  .on('data', (row) => {
    ctx.rowcount++;
    ctx.data.push(row);
  })
  .on('end', () => {
    finish(ctx);
  });

