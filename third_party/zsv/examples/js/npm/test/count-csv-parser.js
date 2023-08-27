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
                ' rows in ' + (endTime - ctx.startTime) + 'ms\n');
}


let ctx = createContext();
/* read stdin if we have no arguments, else the first argument */
const readStream = process.argv.length < 3 ? process.stdin : fs.createReadStream(process.argv[2])
readStream.on('error', (error) => console.log(error.message));
readStream
  .pipe(csv())
  .on('data', () => {
    ctx.rowcount++;
  })
  .on('end', () => {
    finish(ctx);
  });

